#pragma once

#include "compression/zlib.h"
#include "config.h"
#include "err/err.h"

typedef struct INSPECTOR
{
  INSPECTOR_CONFIG config;
  int              ins_fd_dir;
  int              qua_fd_dir;
} INSPECTOR;

ERR
init_inspector(INSPECTOR **inspector, INSPECTOR_CONFIG config);

ERR
add_quarentine_inspector(INSPECTOR *inspector);

ERR
del_quarentine_inspector(INSPECTOR *inspector);

ERR
mov_quarentine_inspector(INSPECTOR *inspector);

ERR
view_quarentine_inspector(INSPECTOR *inspector);

void
exit_inspector(INSPECTOR **inspector);