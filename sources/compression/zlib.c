#include "compression/zlib.h"

#include "logger/logger.h"
#include <alloca.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>

// Block size for read/write
#define CHUNK 16384

ERR
init_zlib(ZLIB **zlib, CONFIG_ZLIB config)
{
  *zlib = malloc(sizeof(struct ZLIB));
  ALLOC_ERR_FAILURE(*zlib);

  (*zlib)->config = config;

  (*zlib)->fd_dir_in = open((*zlib)->config.dir_in, O_RDONLY);
  if ((*zlib)->fd_dir_in < 0)
  {
    LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE %d  (%s), '%s'", errno,
                                 strerror(errno), (*zlib)->config.dir_in));
    return ERR_FAILURE;
  }
  (*zlib)->fd_in =
          openat((*zlib)->fd_dir_in, (*zlib)->config.file_name_in, O_RDONLY);
  if ((*zlib)->fd_in < 0)
  {
    LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE %d  (%s), '%s'", errno,
                                 strerror(errno),
                                 (*zlib)->config.file_name_in));
    return ERR_FAILURE;
  }

  (*zlib)->fd_dir_out = open((*zlib)->config.dir_out, O_RDONLY);
  if ((*zlib)->fd_dir_out < 0)
  {
    LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE %d  (%s), '%s'", errno,
                                 strerror(errno), (*zlib)->config.dir_out));
    return ERR_FAILURE;
  }

  (*zlib)->fd_out = openat((*zlib)->fd_dir_out, (*zlib)->config.file_name_out,
                           O_WRONLY | O_CREAT | O_TRUNC, 0644);

  if ((*zlib)->fd_out < 0)
  {
    LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE %d  (%s), '%s'", errno,
                                 strerror(errno),
                                 (*zlib)->config.file_name_out));
    return ERR_FAILURE;
  }

  return ERR_SUCCESS;
}

ERR
decompress_file(ZLIB **zlib)
{
  memset(&(*zlib)->stream, 0, sizeof((*zlib)->stream));

  int ret;
  if ((ret = inflateInit(&(*zlib)->stream)) != Z_OK)
  {
    LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE %d  (%s)", ret, zError(ret)));
    return ERR_FAILURE;
  }

  void *in  = alloca(CHUNK);
  void *out = alloca(CHUNK);

  ssize_t bytes_read, bytes_written;

  do {
    bytes_read = read((*zlib)->fd_in, in, CHUNK);
    if (bytes_read == EOF)
    {
      inflateEnd(&(*zlib)->stream);
      LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE %d  (%s), '%s'", errno,
                                   strerror(errno),
                                   (*zlib)->config.file_name_in));
      return ERR_FAILURE;
    }

    if (bytes_read == 0) break;

    (*zlib)->stream.avail_in = bytes_read;
    (*zlib)->stream.next_in  = in;

    do {
      (*zlib)->stream.avail_out = CHUNK;
      (*zlib)->stream.next_out  = out;

      ret = inflate(&(*zlib)->stream, Z_NO_FLUSH);
      if (ret == Z_ERRNO || ret == Z_STREAM_ERROR || ret == Z_DATA_ERROR ||
          ret == Z_MEM_ERROR || ret == Z_BUF_ERROR)
      {
        inflateEnd(&(*zlib)->stream);
        LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE %d  (%s)", ret, zError(ret)));
        return ERR_FAILURE;
      }

      bytes_written =
              write((*zlib)->fd_out, out, CHUNK - (*zlib)->stream.avail_out);
      if (bytes_written == -1)
      {
        LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE %d  (%s), '%s'", errno,
                                     strerror(errno),
                                     (*zlib)->config.file_name_out));
        return ERR_FAILURE;
      }

    } while ((*zlib)->stream.avail_out == 0);

  } while (bytes_read > 0);

  inflateEnd(&(*zlib)->stream);

  return ERR_SUCCESS;
}

ERR
compress_file(ZLIB **zlib)
{
  memset(&(*zlib)->stream, 0, sizeof((*zlib)->stream));

  int ret;
  if ((ret = deflateInit(&(*zlib)->stream, Z_DEFAULT_COMPRESSION)) != Z_OK)
  {
    LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE %d  (%s)", ret, zError(ret)));
    return ERR_FAILURE;
  }

  void *in  = alloca(CHUNK);
  void *out = alloca(CHUNK);

  ssize_t bytes_read, bytes_written;

  do {
    bytes_read = read((*zlib)->fd_in, in, CHUNK);
    if (bytes_read == EOF)
    {
      deflateEnd(&(*zlib)->stream);
      LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE %d  (%s), '%s'", errno,
                                   strerror(errno),
                                   (*zlib)->config.file_name_in));
      return ERR_FAILURE;
    }

    (*zlib)->stream.avail_in = bytes_read;
    (*zlib)->stream.next_in  = in;

    do {
      (*zlib)->stream.avail_out = CHUNK;
      (*zlib)->stream.next_out  = out;

      ret = deflate(&(*zlib)->stream, Z_FINISH);
      if (ret == Z_ERRNO || ret == Z_STREAM_ERROR || ret == Z_DATA_ERROR ||
          ret == Z_MEM_ERROR || ret == Z_BUF_ERROR)
      {
        deflateEnd(&(*zlib)->stream);
        LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE %d  (%s)", ret, zError(ret)));
        return ERR_FAILURE;
      }

      bytes_written =
              write((*zlib)->fd_out, out, CHUNK - (*zlib)->stream.avail_out);
      if (bytes_written == -1)
      {
        LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE %d  (%s), '%s'", errno,
                                     strerror(errno),
                                     (*zlib)->config.file_name_out));
        return ERR_FAILURE;
      }

    } while ((*zlib)->stream.avail_out == 0);

  } while (bytes_read > 0);

  deflateEnd(&(*zlib)->stream);

  return ERR_SUCCESS;
}

void
exit_zlib(ZLIB **zlib)
{
  close((*zlib)->fd_in);
  close((*zlib)->fd_out);
  close((*zlib)->fd_dir_in);
  close((*zlib)->fd_dir_out);
  free(*zlib);
  NO_USE_AFTER_FREE(*zlib);
}