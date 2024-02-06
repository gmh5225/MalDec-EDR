#pragma once

#include "compression/zlib.h"
#include "config.h"
#include "err/err.h"
#include <sqlite3.h>

#define DEFAULT_VIEW_QUARANTINE default_view_quarantine

typedef struct INSPECTOR
{
  INSPECTOR_CONFIG config;
  ZLIB            *zlib;
  int              ins_fd_dir;
  int              qua_fd_dir;
  sqlite3         *db;
  sqlite3_stmt    *stmt;
} INSPECTOR;

ERR
init_inspector(INSPECTOR      **inspector,
               INSPECTOR_CONFIG config) warn_unused_result;

ERR
add_quarantine_inspector(INSPECTOR        *inspector,
                         QUARANTINE_FILES *file) warn_unused_result;

ERR
del_quarantine_inspector(INSPECTOR        *inspector,
                         QUARANTINE_FILES *file) warn_unused_result;

ERR
restore_quarantine_inspector(INSPECTOR        *inspector,
                             QUARANTINE_FILES *file) warn_unused_result;

ERR
view_quarantine_inspector(INSPECTOR *inspector,
                          int (*callback)(void *, int, char **,
                                          char **)) warn_unused_result;

void
exit_inspector(INSPECTOR **inspector);

int
default_view_quarantine(void *unused, const int count, char **data,
                        char **columns) warn_unused_result;

ERR
insert_quarantine_db(INSPECTOR        **inspector,
                     QUARANTINE_FILES **file) warn_unused_result;

ERR
select_where_quarantine_db(INSPECTOR        **inspector,
                           QUARANTINE_FILES **file) warn_unused_result;

ERR
select_quarantine_db(INSPECTOR **inspector,
                     int (*callback)(void *, int, char **,
                                     char **)) warn_unused_result;