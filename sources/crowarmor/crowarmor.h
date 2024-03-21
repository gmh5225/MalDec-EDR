#pragma once

#include "compiler/compiler_attribute.h"
#include "config.h"
#include "err/err.h"
#include "io/ioctl.h"
#include <stdbool.h>

/**
 * @struct CROWARMOR
 * @brief Structure representing the CrowArmor driver.
 */
typedef struct packed(8) CROWARMOR
{
  CROWARMOR_CONFIG config; /**< Configuration for the CrowArmor driver. */
  int              fd_crowarmor;
  struct crow      crow;
}
CROWARMOR;

ERR
init_driver_crowarmor(CROWARMOR      **crowarmor,
                      CROWARMOR_CONFIG config) warn_unused_result;

bool
check_driver_crowarmor_alive(CROWARMOR *crowarmor) warn_unused_result;

void
check_driver_crowarmor_activated(CROWARMOR *crowarmor);

void
exit_driver_crowarmor(CROWARMOR **crowarmor);