#include "inspector/inspector.h"
#include "logger/logger.h"
#include <errno.h>

#define SQL_INSERT_QUARANTINE                                                  \
  "INSERT OR IGNORE INTO quarantine (filename, filepath, datatime, detected) " \
  "VALUES (?, ?, ?, ?)"

#define SQL_VIEW_QUARANTINE "SELECT * FROM quarantine"

inline ERR
add_quarantine_inspector(INSPECTOR *inspector, QUARANTINE_FILES *file)
{
  ZLIB_CONFIG config = (ZLIB_CONFIG){.filename_in  = file->filepath,
                                     .filename_out = file->filename,
                                     .fd_dir_out   = inspector->qua_fd_dir,
                                     .chunk        = 16384};

  if (IS_ERR_FAILURE(init_zlib(&inspector->zlib, config)))
  {
    return ERR_FAILURE;
  }

  if (IS_ERR_FAILURE(compress_file(&inspector->zlib))) { return ERR_FAILURE; }

  int rc = sqlite3_prepare(inspector->db, SQL_INSERT_QUARANTINE, -1,
                           &inspector->stmt, NULL);
  if (rc != SQLITE_OK)
  {
    LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE %d  (%s)", rc,
                                 sqlite3_errmsg(inspector->db)));
    return ERR_FAILURE;
  }
  // Bind dos parâmetros
  sqlite3_bind_text(inspector->stmt, 1, file->filename, -1, SQLITE_STATIC);
  sqlite3_bind_text(inspector->stmt, 2, file->filepath, -1, SQLITE_STATIC);
  sqlite3_bind_text(inspector->stmt, 3, file->datatime, -1, SQLITE_STATIC);
  sqlite3_bind_text(inspector->stmt, 4, file->detected, -1, SQLITE_STATIC);

  // Executa a inserção
  if ((rc = sqlite3_step(inspector->stmt)) != SQLITE_DONE)
  {
    LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE %d  (%s)", rc,
                                 sqlite3_errmsg(inspector->db)));
    return ERR_FAILURE;
  }

  // Finaliza a consulta
  sqlite3_finalize(inspector->stmt);

  return ERR_SUCCESS;
}

inline ERR
del_quarantine_inspector(INSPECTOR *inspector, QUARANTINE_FILES *file)
{
  if (unlinkat(inspector->qua_fd_dir, file->filename, 0) < 0)
  {
    LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE %d  (%s), '%s'", errno,
                                 strerror(errno), file->filename));
    return ERR_FAILURE;
  }
  return ERR_SUCCESS;
}

inline ERR
restore_quarantine_inspector(INSPECTOR *inspector, QUARANTINE_FILES *file)
{
  printf("%s %s\n", file->filename, inspector->config.database);
  return ERR_SUCCESS;
}

static inline void
print_line(const int count, const int width)
{
  for (int i = 0; i < count; i++)
  {
    fprintf(stdout, "+%*s", width, "---------------------------");
  }
  fprintf(stdout, "+\n");
}

static inline int
view_callback_quarantine(void *unused, const int count, char **data,
                         char **columns)
{
  unused = unused;

  const int column_width = 25;

  // Imprime cabeçalho da tabela
  print_line(count, column_width);
  for (int idx = 0; idx < count; idx++)
  {
    fprintf(stdout, "| %-*s", column_width, columns[idx]);
  }
  fprintf(stdout, "|\n");

  // Imprime separador entre cabeçalho e dados
  print_line(count, column_width);

  // Imprime os dados
  for (int i = 0; i < count; i++)
  {
    fprintf(stdout, "| %-*s", column_width, data[i]);
  }
  fprintf(stdout, "|\n");

  // Imprime rodapé da tabela
  print_line(count, column_width);

  return 0;
}

inline ERR
view_quarantine_inspector(INSPECTOR *inspector)
{
  char *sqlite_err_msg = NULL;
  int   rc             = sqlite3_exec(inspector->db, SQL_VIEW_QUARANTINE,
                                      view_callback_quarantine, NULL, &sqlite_err_msg);
  if (rc != SQLITE_OK)
  {
    LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE %d  (%s)", rc, sqlite_err_msg));
    return ERR_FAILURE;
  }
  return ERR_SUCCESS;
}