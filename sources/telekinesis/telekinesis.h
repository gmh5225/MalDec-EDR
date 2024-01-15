/**
 * @file telekinesis.h
 * @brief Header file for the Telekinesis driver.
 */

#pragma once

#include "compiler/compiler_attribute.h"
#include "config.h"
#include "err/err.h"
#include <stdbool.h>

/**
 * @struct TELEKINESIS
 * @brief Structure representing the Telekinesis driver.
 */
typedef struct packed(8) TELEKINESIS
{
  TELEKINESIS_CONFIG config; /**< Configuration for the Telekinesis driver. */
}
TELEKINESIS;

/**
 * @brief Initializes the Telekinesis driver.
 *
 * This function initializes the Telekinesis driver and allocates memory for it.
 *
 * @param[out] telekinesis Pointer to a pointer that will store the allocated TELEKINESIS structure.
 * @param[in] config Configuration for the Telekinesis driver.
 * @return An ERR value indicating the success or failure of the initialization.
 */
ERR
init_driver_telekinesis(TELEKINESIS      **telekinesis,
                        TELEKINESIS_CONFIG config) warn_unused_result;

/**
 * @brief Exits the Telekinesis driver.
 *
 * This function releases the resources allocated for the Telekinesis driver.
 *
 * @param[in,out] telekinesis Pointer to the TELEKINESIS structure.
 */
void
exit_driver_telekinesis(TELEKINESIS **telekinesis);

/**
 * @brief Checks if the Telekinesis driver is alive.
 *
 * This function checks whether the Telekinesis driver is still active.
 *
 * @param[in] telekinesis Pointer to the TELEKINESIS structure.
 * @return A boolean value indicating whether the Telekinesis driver is alive.
 */
bool
check_driver_telekinesis_alive(TELEKINESIS *telekinesis) warn_unused_result;

/**
 * @brief Connects to the Telekinesis driver.
 *
 * This function establishes a connection to the Telekinesis driver.
 *
 * @param[in] telekinesis Pointer to the TELEKINESIS structure.
 */
void
connect_driver_telekinesis(TELEKINESIS *telekinesis);
