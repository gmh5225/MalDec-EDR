/**
 * @file defender.h
 * @brief Header file for the DEFENDER module.
 */

#pragma once

#include "cjson/cjson.h"
#include "compiler/compiler_attribute.h"
#include "config.h"
#include "crowarmor/crowarmor.h"
#include "inotify/inotify.h"
#include "inspector/inspector.h"
#include "logger/logger.h"
#include "scanner/scanner.h"
#include "telekinesis/telekinesis.h"

/**
 * @struct DEFENDER
 * @brief Structure representing the DEFENDER module.
 */
typedef struct packed(8) DEFENDER
{
  LOGGER             *logger;  /**< Pointer to the LOGGER instance. */
  SCANNER            *scanner; /**< Pointer to the SCANNER instance. */
  struct json_object *cjson;   /**< Pointer to the configuration JSON object. */
  TELEKINESIS *telekinesis; /**< Pointer to the TELEKINESIS Driver instance. */
  INOTIFY     *inotify;     /**< Pointer to the INOTIFY instance. */
  INSPECTOR   *inspector;   /**< Pointer to the INSPECTOR instance. */
  CROWARMOR   *crowarmor;   /**< Pointer to the CROWARMOR instance. */
  DEFENDER_CONFIG config;   /**< Configuration settings for DEFENDER. */
}
DEFENDER;

/**
 * @brief Initializes the DEFENDER module.
 * @param defender Pointer to a pointer to the DEFENDER instance.
 * @param config Configuration settings for DEFENDER.
 */
void
init_defender(DEFENDER **defender, DEFENDER_CONFIG config);

/**
 * @brief Initializes the LOGGER component within the DEFENDER module.
 * @param defender Pointer to the DEFENDER instance.
 */
void
init_logger_main(DEFENDER **defender);

/**
 * @brief Initializes the INOTIFY component within the DEFENDER module.
 * @param defender Pointer to the DEFENDER instance.
 */
void
init_inotify_main(DEFENDER **defender);

/**
 * @brief Initializes the SCANNER component within the DEFENDER module.
 * @param defender Pointer to the DEFENDER instance.
 */
void
init_scanner_main(DEFENDER **defender);

/**
 * @brief Initializes the TELEKINESIS component within the DEFENDER module.
 * @param defender Pointer to the DEFENDER instance.
 */
void
init_telekinesis_main(DEFENDER **defender);

/**
 * @brief Initializes the CROWARMOR component within the DEFENDER module.
 * 
 * @param defender 
 */
void
init_crowarmor_main(DEFENDER **defender);

/**
 * @brief Initializes the CJSON component within the DEFENDER module.
 * @param defender Pointer to the DEFENDER instance.
 */
void
init_cjson_main(DEFENDER **defender);

/**
 * @brief Initializes the INSPECTOR component within the DEFENDER module.
 * 
 * @param defender 
 */
void
init_inspector_main(DEFENDER **defender);

/**
 * @brief Exits and cleans up resources used by the DEFENDER module.
 * @param defender Pointer to a pointer to the DEFENDER instance.
 */
void
exit_defender(DEFENDER **defender);
