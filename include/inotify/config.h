/**
 * @file config.h
 * @brief Header file for the INOTIFY configuration structure.
 */

#pragma once

#include "inotify/paths.h"
#include <stddef.h>
#include <sys/types.h>

/**
 * @struct INOTIFY_CONFIG
 * @brief Structure representing the configuration for INOTIFY.
 */
typedef struct INOTIFY_CONFIG
{
  struct PATHS *paths;        /**< Pointer to the PATHS structure. */
  size_t        quantity_fds; /**< Quantity of file descriptors. */
  time_t        time;         /**< Time configuration. */
  uint32_t      mask;         /**< Mask for get events */
} INOTIFY_CONFIG;