/**
 * @file edr.h
 * @brief Header file for the EDR module.
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
 * @struct EDR
 * @brief Structure representing the EDR module.
 */
typedef struct packed(8) EDR
{
  LOGGER             *logger;  /**< Pointer to the LOGGER instance. */
  SCANNER            *scanner; /**< Pointer to the SCANNER instance. */
  struct json_object *cjson;   /**< Pointer to the configuration JSON object. */
  TELEKINESIS *telekinesis; /**< Pointer to the TELEKINESIS Driver instance. */
  INOTIFY     *inotify;     /**< Pointer to the INOTIFY instance. */
  INSPECTOR   *inspector;   /**< Pointer to the INSPECTOR instance. */
  CROWARMOR   *crowarmor;   /**< Pointer to the CROWARMOR instance. */
  EDR_CONFIG config;   /**< Configuration settings for EDR. */
}
EDR;

/**
 * @brief Initializes the EDR module.
 * @param edr Pointer to a pointer to the EDR instance.
 * @param config Configuration settings for EDR.
 */
void
init_edr(EDR **edr, EDR_CONFIG config);

/**
 * @brief Initializes the LOGGER component within the EDR module.
 * @param edr Pointer to the EDR instance.
 */
void
init_logger_main(EDR **edr);

/**
 * @brief Initializes the INOTIFY component within the EDR module.
 * @param edr Pointer to the EDR instance.
 */
void
init_inotify_main(EDR **edr);

/**
 * @brief Initializes the SCANNER component within the EDR module.
 * @param edr Pointer to the EDR instance.
 */
void
init_scanner_main(EDR **edr);

/**
 * @brief Initializes the TELEKINESIS component within the EDR module.
 * @param edr Pointer to the EDR instance.
 */
void
init_telekinesis_main(EDR **edr);

/**
 * @brief Initializes the CROWARMOR component within the EDR module.
 * 
 * @param edr 
 */
void
init_crowarmor_main(EDR **edr);

/**
 * @brief Initializes the CJSON component within the EDR module.
 * @param edr Pointer to the EDR instance.
 */
void
init_cjson_main(EDR **edr);

/**
 * @brief Initializes the INSPECTOR component within the EDR module.
 * 
 * @param edr 
 */
void
init_inspector_main(EDR **edr);

/**
 * @brief Exits and cleans up resources used by the EDR module.
 * @param edr Pointer to a pointer to the EDR instance.
 */
void
exit_edr(EDR **edr);
