#pragma once

#include "inotify/paths.h"
#include <stddef.h>
#include <sys/types.h>

typedef struct INOTIFY_CONFIG
{
  struct PATHS *paths;
  size_t        quantity_fds;
  time_t        time;

} INOTIFY_CONFIG;