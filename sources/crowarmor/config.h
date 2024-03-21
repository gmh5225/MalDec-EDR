#pragma once

#include "compiler/compiler_attribute.h"

/**
 * @struct CROWARMOR_CONFIG
 * @brief Configuration structure for the file CrowArmor driver.
 */
typedef struct packed(8) CROWARMOR_CONFIG
{
  const char *driver_path; /**< Path to the driver CrowArmor. */
  const char *driver_name; /**< Name to the driver*/
}
CROWARMOR_CONFIG;