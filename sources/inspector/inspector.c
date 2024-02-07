#include "inspector/inspector.h"
#include "logger/logger.h"
#include <errno.h>
#include <fcntl.h>
#include <features.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define INSPECTOR_PERMISSIONS 0775
#define QUARANTINE_PERMISSIONS (S_IRUSR | S_IWUSR | S_IXUSR)
#define SQL_CREATE_TABLE_QUARANTINE                                \
  "CREATE TABLE IF NOT EXISTS quarantine (id integer primary key " \
  "autoincrement, filename varchar(255),   filepath "              \
  "varchar(255) UNIQUE, detected varchar(255), datetime DATETIME);"

static inline void
create_database_inspector(INSPECTOR *inspector)
{
  INSPECTOR_CONFIG config = inspector->config;

  const char *dir      = config.dir;
  const char *database = config.database;

  size_t size_dir      = strlen(dir);
  size_t size_database = strlen(database);

  char *database_path = calloc(size_dir + size_database + 1, sizeof(char));

  strncpy(database_path, dir, size_dir);
  strncat(database_path, database, size_database);

  int rc = sqlite3_open_v2(database_path, &(inspector->db),
                           SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE, NULL);
  if (rc)
  {
    LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE %d  (%s), '%s'", rc,
                                 sqlite3_errmsg(inspector->db), database_path));
    return;
  }

  char *sqlite_err_msg = NULL;
  rc = sqlite3_exec(inspector->db, SQL_CREATE_TABLE_QUARANTINE, NULL, NULL,
                    &sqlite_err_msg);
  if (rc != SQLITE_OK)
  {
    LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE %d  (%s), '%s'", rc,
                                 sqlite_err_msg, database_path));
  }

  free(database_path);
}

inline ERR
init_inspector(INSPECTOR **inspector, INSPECTOR_CONFIG config)
{
  *inspector = malloc(sizeof(struct INSPECTOR));
  ALLOC_ERR_FAILURE(*inspector);

  (*inspector)->config = config;

  struct stat sb;
  if (stat((*inspector)->config.dir, &sb) != 0 || !S_ISDIR(sb.st_mode))
  {
    if (mkdir((*inspector)->config.dir, INSPECTOR_PERMISSIONS) == -1)
    {
      LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE %d  (%s), '%s'", errno,
                                   strerror(errno), (*inspector)->config.dir));
      return ERR_FAILURE;
    }
  }

  (*inspector)->ins_fd_dir =
          open((*inspector)->config.dir, O_RDONLY | O_DIRECTORY, 0);
  if ((*inspector)->ins_fd_dir == -1)
  {
    LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE %d  (%s), '%s'", errno,
                                 strerror(errno), (*inspector)->config.dir));
    return ERR_FAILURE;
  }

  if (fstatat((*inspector)->ins_fd_dir, (*inspector)->config.quarantine.dir,
              &sb, 0) != 0 &&
      !S_ISDIR(sb.st_mode))
  {
    if (mkdirat((*inspector)->ins_fd_dir, (*inspector)->config.quarantine.dir,
                QUARANTINE_PERMISSIONS) == -1)
    {
      LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE %d (%s), '%s'", errno,
                                   strerror(errno),
                                   (*inspector)->config.quarantine.dir));
      return ERR_FAILURE;
    }
  }

  (*inspector)->qua_fd_dir = openat((*inspector)->ins_fd_dir,
                                    (*inspector)->config.quarantine.dir,
                                    O_RDONLY | O_DIRECTORY);

  if ((*inspector)->qua_fd_dir == -1)
  {
    LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE %d  (%s), '%s'", errno,
                                 strerror(errno),
                                 (*inspector)->config.quarantine.dir));
    return ERR_FAILURE;
  }

  create_database_inspector(*inspector);

  return ERR_SUCCESS;
}

inline void
exit_inspector(INSPECTOR **inspector)
{
  close((*inspector)->ins_fd_dir);
  close((*inspector)->qua_fd_dir);
  sqlite3_close_v2((*inspector)->db);
  free(*inspector);
  NO_USE_AFTER_FREE(*inspector);
}