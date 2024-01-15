/**
 * @file logger.h
 * @brief Header file for the LOGGER module.
 */

#pragma once

#include "c-logger/src/logger.h"
#include "compiler/compiler_attribute.h"
#include "config.h"
#include "err/err.h"

#define LOG_MESSAGE_FORMAT(msg, ...) "%s() : " msg, __FUNCTION__, ##__VA_ARGS__

/**
 * @struct LOGGER
 * @brief Structure representing the LOGGER module.
 */
typedef struct LOGGER
{
  LOGGER_CONFIG config; /**< Configuration for the LOGGER module. */
} LOGGER;

/**
 * @brief Initializes the LOGGER module.
 *
 * This function initializes the LOGGER module with the provided configuration.
 *
 * @param logger Pointer to a LOGGER pointer that will be set to the initialized LOGGER instance.
 * @param config Configuration for the LOGGER module.
 * @return Returns ERR_SUCCESS on success, or a ERROR value on failure.
 * @warning The caller is responsible for freeing the allocated memory for the LOGGER instance using
 * @ref exit_logger.
 */
ERR
init_logger(LOGGER **logger, LOGGER_CONFIG config) warn_unused_result;

/**
 * @brief Exits the LOGGER module.
 *
 * This function frees the resources associated with the LOGGER module.
 *
 * @param logger Pointer to a LOGGER pointer that will be set to NULL after freeing the resources.
 */
void
exit_logger(LOGGER **logger);
