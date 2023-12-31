/**
 * @file scanner_config.h
 * @brief Defines the configuration structure for a file scanner.
 */

#pragma once

#include "compiler/compiler_attribute.h"
#include <stdint.h>

// TODO: Add more flags
#define QUICK_SCAN 1 << 0

/**
 * @struct SCANNER_CONFIG
 * @brief Configuration structure for the file scanner.
 */
// clang-format off
typedef struct SCANNER_CONFIG
{
  const char *rules;     /**< Path to the rules file. */
  uint8_t     scan_type; /**< Type of scan (e.g., QUICK_SCAN). */
  int32_t     max_depth; /**< Maximum depth for scanning directories. */
  char       *file_path; /**< Path to the file or directory to be scanned. */
  bool        verbose;   /**< Verbose mode flag. */
  struct SKIP_DIRS *skip; /**< Pointer to a structure specifying directories to skip. */
} SCANNER_CONFIG;
// clang-format on