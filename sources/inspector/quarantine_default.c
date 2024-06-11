#include "inspector/inspector.h"
#include "logger/logger.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

inline int
default_sync_quarantine(void *ins, const int count, char **data, char **columns)
{
  INSPECTOR *inspector = (INSPECTOR *)ins;

  for (int i = 0; i < count; i++)
  {
    if (!strncmp(columns[i], "filename", 9))
    {
      if (faccessat(inspector->qua_fd_dir, data[i], F_OK, 0) == 0)
      {
        LOG_INFO(LOG_MESSAGE_FORMAT("File '%s' synchronized with the database ",
                                    data[i]));
      }
      else
      {
        LOG_INFO(LOG_MESSAGE_FORMAT("File '%s' not synchronized with the "
                                    "database ",
                                    data[i]));
        QUARANTINE_FILE *file = malloc(sizeof(QUARANTINE_FILE));
        file->id              = atoi(data[0]);
        if (IS_ERR_FAILURE(delete_where_quarantine_db(&inspector, &file)))
        {
          LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE Not delete where file id "
                                       "'%i' "
                                       "table "
                                       "quarantine",
                                       file->id));
          return ERR_FAILURE;
        }
        LOG_INFO(LOG_MESSAGE_FORMAT("File '%s' synchronized with the database ",
                                    data[i]));

        exit_stmt_finalize(&inspector);
        free(file);
      }
    }
  }
  return 0;
}