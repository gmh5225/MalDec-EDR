#pragma once

#include <stddef.h>
#include <sys/types.h>

typedef struct INOTIFY_CONFIG
{
  size_t quantity_fds;
  const char **paths;
  time_t time;

} INOTIFY_CONFIG;