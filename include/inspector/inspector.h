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
init_inspector(INSPECTOR      **inspector,
               INSPECTOR_CONFIG config) warn_unused_result;

ERR
add_quarentine_inspector(INSPECTOR *inspector) warn_unused_result;

ERR
del_quarentine_inspector(INSPECTOR *inspector) warn_unused_result;

ERR
mov_quarentine_inspector(INSPECTOR *inspector) warn_unused_result;

ERR
view_quarentine_inspector(INSPECTOR *inspector) warn_unused_result;

void
exit_inspector(INSPECTOR **inspector);