#include "crowarmor/crowarmor.h"
#include "logger/logger.h"

#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

ERR
init_driver_crowarmor(CROWARMOR **crowarmor, CROWARMOR_CONFIG config)
{
  *crowarmor           = malloc(sizeof(struct CROWARMOR));
  (*crowarmor)->config = config;

  if (!check_driver_crowarmor_alive(*crowarmor))
  {
    LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE Driver %s not alive",
                                 (*crowarmor)->config.driver_name));
    return ERR_FAILURE;
  };

  if (((*crowarmor)
               ->fd_crowarmor = // In order to use this call, one needs an open file descriptor.
       // Often the open(2) call has unwanted side effects, that can be
       // avoided under Linux by giving it the O_NONBLOCK flag.
       open((*crowarmor)->config.driver_path, O_RDWR | O_NONBLOCK)) < 0)
  {
    LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE Error in open driver %s : %ld "
                                 "(%s)",
                                 (*crowarmor)->config.driver_name, errno,
                                 strerror(errno)));
    return ERR_FAILURE;
  }

  return ERR_SUCCESS;
}

inline bool
check_driver_crowarmor_alive(CROWARMOR *crowarmor)
{
  if (IS_NULL_PTR(crowarmor)) return false;
  return (access(crowarmor->config.driver_path, F_OK) == 0);
}

bool
check_driver_crowarmor_activated(CROWARMOR *crowarmor)
{
  if (!check_driver_crowarmor_alive(crowarmor))
  {
    LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE Driver %s not alive",
                                 (crowarmor)->config.driver_name));
    return ERR_FAILURE;
  };

  // ioctl(crowarmor->fd_crowarmor);

  return ERR_SUCCESS;
}

void
exit_driver_crowarmor(CROWARMOR **crowarmor)
{
  close((*crowarmor)->fd_crowarmor);
  free(*crowarmor);
  NO_USE_AFTER_FREE(*crowarmor);
}