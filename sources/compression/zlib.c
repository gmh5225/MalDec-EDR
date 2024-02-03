#include "compression/zlib.h"

#include "logger/logger.h"
#include <alloca.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>

ERR
init_zlib(ZLIB **zlib, ZLIB_CONFIG config)
{
  *zlib = malloc(sizeof(struct ZLIB));
  ALLOC_ERR_FAILURE(*zlib);

  (*zlib)->config = config;

  (*zlib)->fd_in = open((*zlib)->config.filename_in, O_RDONLY);
  if ((*zlib)->fd_in < 0)
  {
    LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE %d  (%s), '%s'", errno,
                                 strerror(errno), (*zlib)->config.filename_in));
    return ERR_FAILURE;
  }

  if ((*zlib)->config.fd_dir_out < 0)
  {
    LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE fd '%i' it is not valid",
                                 (*zlib)->config.fd_dir_out));
    return ERR_FAILURE;
  }

  (*zlib)->fd_out = openat((*zlib)->config.fd_dir_out,
                           (*zlib)->config.filename_out,
                           O_WRONLY | O_CREAT | O_TRUNC, 0644);

  if ((*zlib)->fd_out < 0)
  {
    LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE %d  (%s), '%s'", errno,
                                 strerror(errno),
                                 (*zlib)->config.filename_out));
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

  void *in  = alloca((*zlib)->config.chunk);
  void *out = alloca((*zlib)->config.chunk);

  ssize_t bytes_read, bytes_written;

  do {
    bytes_read = read((*zlib)->fd_in, in, (*zlib)->config.chunk);
    if (bytes_read == EOF)
    {
      inflateEnd(&(*zlib)->stream);
      LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE %d  (%s), '%s'", errno,
                                   strerror(errno),
                                   (*zlib)->config.filename_in));
      return ERR_FAILURE;
    }

    if (bytes_read == 0) break;

    (*zlib)->stream.avail_in = bytes_read;
    (*zlib)->stream.next_in  = in;

    do {
      (*zlib)->stream.avail_out = (*zlib)->config.chunk;
      (*zlib)->stream.next_out  = out;

      ret = inflate(&(*zlib)->stream, Z_NO_FLUSH);
      if (ret == Z_ERRNO || ret == Z_STREAM_ERROR || ret == Z_DATA_ERROR ||
          ret == Z_MEM_ERROR || ret == Z_BUF_ERROR)
      {
        inflateEnd(&(*zlib)->stream);
        LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE %d  (%s)", ret, zError(ret)));
        return ERR_FAILURE;
      }

      bytes_written = write((*zlib)->fd_out, out,
                            (*zlib)->config.chunk - (*zlib)->stream.avail_out);
      if (bytes_written == -1)
      {
        LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE %d  (%s), '%s'", errno,
                                     strerror(errno),
                                     (*zlib)->config.filename_out));
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

  void *in  = alloca((*zlib)->config.chunk);
  void *out = alloca((*zlib)->config.chunk);

  ssize_t bytes_read, bytes_written;

  do {
    bytes_read = read((*zlib)->fd_in, in, (*zlib)->config.chunk);
    if (bytes_read == EOF)
    {
      deflateEnd(&(*zlib)->stream);
      LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE %d  (%s), '%s'", errno,
                                   strerror(errno),
                                   (*zlib)->config.filename_in));
      return ERR_FAILURE;
    }

    (*zlib)->stream.avail_in = bytes_read;
    (*zlib)->stream.next_in  = in;

    do {
      (*zlib)->stream.avail_out = (*zlib)->config.chunk;
      (*zlib)->stream.next_out  = out;

      ret = deflate(&(*zlib)->stream, Z_FINISH);
      if (ret == Z_STREAM_ERROR)
      {
        deflateEnd(&(*zlib)->stream);
        LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE %d  (%s)", ret, zError(ret)));
        return ERR_FAILURE;
      }

      bytes_written = write((*zlib)->fd_out, out,
                            (*zlib)->config.chunk - (*zlib)->stream.avail_out);
      if (bytes_written == -1)
      {
        LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE %d  (%s), '%s'", errno,
                                     strerror(errno),
                                     (*zlib)->config.filename_out));
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
  close((*zlib)->config.fd_dir_out);
  free(*zlib);
  NO_USE_AFTER_FREE(*zlib);
}