/**
 * @struct DEFENDER_CONFIG
 * @brief Structure representing the configuration for the defender application.
 */
#pragma once

#include "cjson/cjson.h"
#include "compiler/compiler_attribute.h"
#include "inotify/inotify.h"
#include "logger/logger.h"
#include "scan/config.h"
#include "scan/scan.h"

struct packed(8) DEFENDER_CONFIG
{
  LOGGER  *logger;  /**< Pointer to the LOGGER instance. */
  SCANNER *scanner; /**< Pointer to the SCANNER instance. */
  struct json_object
          *config_json; /**< Pointer to the configuration JSON object. */
}
DEFENDER_CONFIG;