#include "inspector/inspector.h"
#include "logger/logger.h"

#define SQL_INSERT_QUARANTINE                                                  \
  "INSERT OR IGNORE INTO quarantine (filename, filepath, datatime, detected) " \
  "VALUES (?, ?, ?, ?)"

#define SQL_SELECT_QUARANTINE "SELECT * FROM quarantine"

ERR
insert_quarantine_db(INSPECTOR **inspector, QUARANTINE_FILES **file)
{
  int rc = sqlite3_prepare((*inspector)->db, SQL_INSERT_QUARANTINE, -1,
                           &(*inspector)->stmt, NULL);
  if (rc != SQLITE_OK)
  {
    LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE %d  (%s)", rc,
                                 sqlite3_errmsg((*inspector)->db)));
    return ERR_FAILURE;
  }

  sqlite3_bind_text((*inspector)->stmt, 1, (*file)->filename, -1,
                    SQLITE_STATIC);
  sqlite3_bind_text((*inspector)->stmt, 2, (*file)->filepath, -1,
                    SQLITE_STATIC);
  sqlite3_bind_text((*inspector)->stmt, 3, (*file)->datatime, -1,
                    SQLITE_STATIC);
  sqlite3_bind_text((*inspector)->stmt, 4, (*file)->detected, -1,
                    SQLITE_STATIC);

  if ((rc = sqlite3_step((*inspector)->stmt)) != SQLITE_DONE)
  {
    LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE %d  (%s)", rc,
                                 sqlite3_errmsg((*inspector)->db)));
    return ERR_FAILURE;
  }

  sqlite3_finalize((*inspector)->stmt);

  return ERR_SUCCESS;
}

ERR
select_quarantine_db(INSPECTOR **inspector,
                     int (*callback)(void *, int, char **, char **))
{
  char *sqlite_err_msg = NULL;
  int rc = sqlite3_exec((*inspector)->db, SQL_SELECT_QUARANTINE, callback, NULL,
                        &sqlite_err_msg);
  if (rc != SQLITE_OK)
  {
    LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE %d  (%s)", rc, sqlite_err_msg));
    return ERR_FAILURE;
  }
  return ERR_SUCCESS;
}