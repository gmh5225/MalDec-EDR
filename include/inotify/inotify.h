#pragma once

#include "compiler/compiler_attribute.h"
#include "config.h"
#include "err/err.h"
#include <poll.h>
#include <sys/inotify.h>

typedef struct INOTIFY
{
  INOTIFY_CONFIG config;
  char           buf;
  int            fd_inotify, poll_num;
  int           *wd;
  nfds_t         nfds;
  struct pollfd  fds[1];
} INOTIFY;

typedef void (*handles_events_t)(INOTIFY *inotify, void *buff);

ERR
init_inotify(INOTIFY **inotify, INOTIFY_CONFIG config) warn_unused_result;

void
listen_to_events_inotify(INOTIFY **inotify, void *user_data,
                         handles_events_t handles);

void
exit_inotify(INOTIFY **inotify);