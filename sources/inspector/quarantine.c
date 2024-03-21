#include "inspector/inspector.h"
#include "logger/logger.h"
#include <errno.h>

inline ERR
add_quarantine_inspector(INSPECTOR *inspector, QUARANTINE_FILES *file)
{
  ZLIB_CONFIG config = (ZLIB_CONFIG){.filename_in  = file->filepath,
                                     .filename_out = file->filename,
                                     .fd_dir_out   = inspector->qua_fd_dir,
                                     .chunk        = 16384};

  if (IS_ERR_FAILURE(init_zlib(&inspector->zlib, config)))
  {
    LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE Error init zlib %s",
                                 file->filename));
    return ERR_FAILURE;
  }

  if (IS_ERR_FAILURE(compress_file(&inspector->zlib)))
  {
    LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE Not compress file %s",
                                 file->filename));
    return ERR_FAILURE;
  }

  if (IS_ERR_FAILURE(insert_quarantine_db(&inspector, &file)))
  {
    LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE Not insert file in quarantine"));
    return ERR_FAILURE;
  }

  if (unlink(file->filepath) < 0)
  {
    LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE %d  (%s), '%s'", errno,
                                 strerror(errno), file->filename));
    return ERR_FAILURE;
  }

  LOG_INFO(LOG_MESSAGE_FORMAT("File '%s' added from quarantine",
                              file->filename));

  exit_zlib(&inspector->zlib);
  exit_stmt_finalize(&inspector);
  return ERR_SUCCESS;
}

inline ERR
del_quarantine_inspector(INSPECTOR *inspector, QUARANTINE_FILES *file)
{
  if (IS_ERR_FAILURE(select_where_quarantine_db(&inspector, &file)))
  {
    LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE Not select where file id '%i' "
                                 "table "
                                 "quarantine",
                                 file->id));
    return ERR_FAILURE;
  }

  if (unlinkat(inspector->qua_fd_dir, file->filename, 0) == -1)
  {
    LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE %d  (%s), '%s'", errno,
                                 strerror(errno), file->filename));
    return ERR_FAILURE;
  }

  exit_stmt_finalize(&inspector);

  if (IS_ERR_FAILURE(delete_where_quarantine_db(&inspector, &file)))
  {
    LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE Not delete where file id '%i' "
                                 "table "
                                 "quarantine",
                                 file->id));
    return ERR_FAILURE;
  }

  LOG_INFO(
          LOG_MESSAGE_FORMAT("File id '%i' deleted from quarantine", file->id));

  exit_stmt_finalize(&inspector);

  return ERR_SUCCESS;
}

inline ERR
restore_quarantine_inspector(INSPECTOR *inspector, QUARANTINE_FILES *file)
{
  if (select_where_quarantine_db(&inspector, &file))
  {
    LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE Not select where file '%i' table "
                                 "quarantine",
                                 file->id));
    return ERR_FAILURE;
  }

  ZLIB_CONFIG config = (ZLIB_CONFIG){.filename_in  = file->filename,
                                     .filename_out = file->filepath,
                                     .fd_dir_in    = inspector->qua_fd_dir,
                                     .chunk        = 16384};
  if (IS_ERR_FAILURE(init_zlib(&inspector->zlib, config)))
  {
    LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE Error init zlib %s",
                                 file->filename));
    return ERR_FAILURE;
  }

  if (IS_ERR_FAILURE(decompress_file(&inspector->zlib)))
  {
    LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE Not decompress file %s",
                                 file->filename));
    return ERR_FAILURE;
  }

  LOG_INFO(LOG_MESSAGE_FORMAT("File '%s' restored from quarantine",
                              file->filename));

  exit_stmt_finalize(&inspector);
  exit_zlib(&inspector->zlib);
  return ERR_SUCCESS;
}

inline ERR
view_quarantine_inspector(INSPECTOR *inspector,
                          int (*callback)(void *, int, char **, char **))
{
  if (IS_ERR_FAILURE(select_all_quarantine_db(&inspector, callback)))
  {
    LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE Not select table quarantine"));
    return ERR_FAILURE;
  }

  return ERR_SUCCESS;
}

inline ERR
sync_quarantine_inspector(INSPECTOR *inspector,
                          int (*callback)(void *ins, int, char **, char **))
{
  if (IS_ERR_FAILURE(select_all_quarantine_db(&inspector, callback)))
  {
    LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE Not select table quarantine"));
    return ERR_FAILURE;
  }

  LOG_INFO(
          LOG_MESSAGE_FORMAT("Database '%s' synced", inspector->config.database));
  return ERR_SUCCESS;
}