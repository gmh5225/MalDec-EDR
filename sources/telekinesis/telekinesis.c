#include "telekinesis.h"

#include "logger/logger.h"
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
    LOG_ERROR(LOG_MESSAGE_FORMAT("Driver %s not alive",
                                 (*telekinesis)->config.driver_name));
    return ERR_FAILURE;
  };

  return ERR_SUCCESS;
}

inline void
connect_driver_telekinesis(TELEKINESIS *telekinesis)
{
  if (!check_driver_telekinesis_alive(telekinesis))
  {
    LOG_ERROR(LOG_MESSAGE_FORMAT("Driver %s not alive",
                                 telekinesis->config.driver_name));
    return;
  }
}

void
exit_driver_telekinesis(TELEKINESIS **telekinesis)
{
  free(*telekinesis);
  NO_USE_AFTER_FREE(*telekinesis);
}