#include "inotify/inotify.h"

#include "logger/logger.h"
#include <errno.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <unistd.h>

#include "err/err.h"

ERR
init_inotify(INOTIFY **inotify, INOTIFY_CONFIG config)
{
  ERR retval = ERR_FAILURE;

  *inotify = malloc(sizeof(struct INOTIFY));

  (*inotify)->fd_inotify = inotify_init1(IN_NONBLOCK);

  if ((*inotify)->fd_inotify == -1)
  {
    LOG_ERROR(
            LOG_MESSAGE_FORMAT("ERR_FAILURE %d (%s)", errno, strerror(errno)));
    return retval;
  }

  retval = ERR_SUCCESS;

  return retval;
}

void
exit_inotify(INOTIFY **inotify)
{
  if (!IS_NULL_PTR(*inotify))
  {
    close((*inotify)->fd_inotify);

    free(*inotify);
    NO_USE_AFTER_FREE(*inotify)
  }
}