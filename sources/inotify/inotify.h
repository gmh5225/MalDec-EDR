#pragma once

#include "compiler/compiler_attribute.h"
#include "config.h"
#include "scan/scan.h"

typedef struct INOTIFY
{
  INOTIFY_CONFIG config;
  SCANNER        scan;
  int            fd_inotify;

} INOTIFY;

ERR
init_inotify(INOTIFY **inotify, INOTIFY_CONFIG config) warn_unused_result;

void
exit_inotify(INOTIFY **inotify);