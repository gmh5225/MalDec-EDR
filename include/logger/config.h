/**
 * @file logger_config.h
 * @brief Header file for the LOGGER_CONFIG structure.
 */

#pragma once

#include "c-logger/src/logger.h"
#include <stdbool.h>

/**
 * @struct LOGGER_CONFIG
 * @brief Configuration structure for the LOGGER module.
 */
typedef struct LOGGER_CONFIG
{
  const char   *filename;         /**< The name of the log file. */
  long          max_file_size;    /**< The maximum size of the log file. */
  unsigned char max_backup_files; /**< The maximum number of backup log files. */
  LogLevel      level; /**< The logging level for terminal and file logging. */
  bool console; /**< Flag indicating whether to print logs to the terminal. */
} LOGGER_CONFIG;