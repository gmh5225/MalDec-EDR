#include "telekinesis.h"

#include "logger/logger.h"
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

inline bool
check_driver_telekinesis_alive(TELEKINESIS *telekinesis)
{
  if (IS_NULL_PTR(telekinesis)) return false;
  return (access(telekinesis->config.driver_path, F_OK) == 0);
}

inline ERR
init_driver_telekinesis(TELEKINESIS **telekinesis, TELEKINESIS_CONFIG config)
{
  *telekinesis           = malloc(sizeof(struct TELEKINESIS));
  (*telekinesis)->config = config;

  if (!check_driver_telekinesis_alive(*telekinesis))
  {
    LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE Driver %s not alive",
                                 (*telekinesis)->config.driver_name));
    return ERR_FAILURE;
  };

  if (((*telekinesis)
               ->fd_telekinesis = // In order to use this call, one needs an open file descriptor.
       // Often the open(2) call has unwanted side effects, that can be
       // avoided under Linux by giving it the O_NONBLOCK flag.
       open((*telekinesis)->config.driver_path, O_RDWR | O_NONBLOCK)) < 0)
  {
    LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE Error in open driver %s : %ld "
                                 "(%s)",
                                 (*telekinesis)->config.driver_name, errno,
                                 strerror(errno)));
    return ERR_FAILURE;
  }

  return ERR_SUCCESS;
}

void
exit_driver_telekinesis(TELEKINESIS **telekinesis)
{
  close((*telekinesis)->fd_telekinesis);
  free(*telekinesis);
  NO_USE_AFTER_FREE(*telekinesis);
}