/**
 * @file config.h
 * @brief Defines the configuration structure for a file scanner.
 */

#pragma once

#include "compiler/compiler_attribute.h"
#include "inotify/inotify.h"
#include "inspector/inspector.h"
#include <stdint.h>

// TODO: Add more flags
#define QUICK_SCAN 1 << 0

typedef struct YARA_CONFIG
{
  const char *rules; /**< Path to the rules file. */
} YARA_CONFIG;

/**
 * @struct SCAN_CONFIG
 * @brief Configuration structure for the file scanner.
 */
typedef struct SCAN_CONFIG
{
  YARA_CONFIG yara;
  uint8_t     scan_type; /**< Type of scan (e.g., QUICK_SCAN). */
  int32_t     max_depth; /**< Maximum depth for scanning directories. */
  char       *file_path; /**< Path to the file or directory to be scanned. */
  bool        verbose;   /**< Verbose mode flag. */
  INOTIFY    *inotify;   /** < Pointer for inotify STRUCT */
  struct SKIP_DIRS *
          skip_dirs; /**< Pointer to a structure specifying directories to skip. */
  INSPECTOR *inspector;
} SCAN_CONFIG;