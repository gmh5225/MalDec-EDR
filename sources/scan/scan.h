/**
 * @file scanner.h
 * @brief Defines structures and functions for scanning files and directories using YARA rules.
 */

#pragma once

#include <yara.h>

#include "compiler/compiler_attribute.h"
#include "err/err.h"
#include "inotify/inotify.h"
#include "scan/config.h"
#include "scan/skip_dirs.h"

#define DEFAULT_SCAN_CALLBACK \
  default_scan_callback /**< Default scan callback function. */

#define DEFAULT_SCAN_INOTIFY default_scan_inotify /**< default scan inotify*/

/**
 * @struct SCANNER
 * @brief Structure representing a YARA scanner.
 */
typedef struct SCANNER
{
  YR_RULES      *yr_rules;    /**< Pointer to YARA rules. */
  YR_COMPILER   *yr_compiler; /**< Pointer to YARA compiler. */
  SCANNER_CONFIG config;      /**< Scanner configuration. */
} SCANNER;

/**
 * @struct SCANNER_CALLBACK_ARGS
 * @brief Structure representing arguments for the scan callback function.
 */
typedef struct SCANNER_CALLBACK_ARGS
{
  const char *file_path;     /**< Path of the file being scanned. */
  int         current_count; /**< Current count during scanning. */
  bool        verbose;       /**< Verbose mode flag. */
} SCANNER_CALLBACK_ARGS;

/**
 * @brief Scans a file or directory using the specified YARA scanner.
 *
 * This function scans a file or directory based on the provided YARA scanner and callback function.
 *
 * @param[in] scanner Pointer to the YARA scanner.
 * @return Returns ERR_SUCCESS on success, ERR_FAILURE on failure.
 */
ERR
scan(SCANNER *scanner) warn_unused_result;

/**
 * @brief Initializes a YARA scanner with the given configuration.
 *
 * @param[out] scanner Pointer to a pointer where the YARA scanner will be stored.
 * @param[in] config Configuration for the YARA scanner.
 * @return Returns ERR_SUCCESS on success, ERR_FAILURE on failure.
 */
ERR
init_scanner(SCANNER **scanner, SCANNER_CONFIG config) warn_unused_result;

/**
 * @brief Exits and frees resources associated with a YARA scanner.
 *
 * @param[in,out] scanner Pointer to the YARA scanner to be exited.
 * @return Returns ERR_SUCCESS on success, ERR_FAILURE on failure.
 */
ERR
exit_scanner(SCANNER **scanner) warn_unused_result;

/**
 * @brief Scans a single file using the specified YARA scanner and callback function.
 *
 * @param[in] scanner Pointer to the YARA scanner.
 * @param[in] callback YARA callback function.
 * @return Returns ERR_SUCCESS on success, ERR_FAILURE on failure.
 */
ERR
scan_file(SCANNER *scanner, YR_CALLBACK_FUNC callback) warn_unused_result;

/**
 * @brief 
 * 
 * @param scanner 
 * @param inotify 
 */
void
scan_listen(SCANNER *scanner, INOTIFY *inotify);

/**
 * @brief Scans a directory and its subdirectories using the specified YARA scanner and callback
 * function.
 *
 * @param[in] scanner Pointer to the YARA scanner.
 * @param[in] callback YARA callback function.
 * @param[in] current_depth Current depth during recursive scanning.
 * @return Returns ERR_SUCCESS on success, ERR_FAILURE on failure.
 */
ERR
scan_dir(SCANNER *scanner, YR_CALLBACK_FUNC callback,
         int32_t current_depth) warn_unused_result;

/**
 * @brief Default scan callback function.
 *
 * @param[in] context YARA scan context.
 * @param[in] message Type of the message.
 * @param[in] message_data Data associated with the message.
 * @param[in] user_data User data passed to the callback.
 * @return Returns yara call_back_macros.
 */
int
default_scan_callback(YR_SCAN_CONTEXT *context, int message, void *message_data,
                      void *user_data) warn_unused_result;

void
default_scan_inotify(INOTIFY *inotify, void *user_data);