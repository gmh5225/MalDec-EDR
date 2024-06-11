/**
 * @file inotify.h
 * @brief Header file for the INOTIFY wrapper.
 */

#pragma once

#include "compiler/compiler_attribute.h"
#include "config.h"
#include "err/err.h"
#include <poll.h>
#include <sys/inotify.h>

/**
 * @struct INOTIFY
 * @brief Structure representing the INOTIFY wrapper.
 */
typedef struct INOTIFY
{
  INOTIFY_CONFIG config;               /**< Configuration for INOTIFY. */
  char           buf;                  /**< Buffer for storing event data. */
  int            fd_inotify, poll_num; /**< File descriptors and poll number. */
  int           *wd;                   /**< Array of watch descriptors. */
  nfds_t         nfds;                 /**< Number of file descriptors. */
  struct pollfd  fds[1];               /**< Array of poll structures. */
} INOTIFY;

/**
 * @typedef handles_events_t
 * @brief Typedef for the callback function handling INOTIFY events.
 * @param inotify - Pointer to the INOTIFY structure.
 * @param buff - Pointer to the buffer containing event data.
 */
typedef void (*handles_events_t)(INOTIFY *inotify, void *buff);

/**
 * @brief Initializes the INOTIFY structure.
 * @param inotify - Pointer to a pointer to the INOTIFY structure.
 * @param config - Configuration for INOTIFY.
 * @return ERR indicating success or failure.
 */
ERR
init_inotify(INOTIFY **inotify, INOTIFY_CONFIG config) warn_unused_result;

/**
  * @brief Set the watch paths object
  * 
  * @param inotify  Pointer to a pointer to the INOTIFY structure.
  */
void
set_watch_paths_inotify(INOTIFY *inotify);

/**
 * @brief Listens to events using the INOTIFY structure.
 * @param inotify - Pointer to a pointer to the INOTIFY structure.
 * @param user_data - User-defined data to be passed to the callback.
 * @param handles - Callback function to handle INOTIFY events.
 */
void
listen_to_events_inotify(INOTIFY **inotify, void *user_data,
                         handles_events_t handles);

/**
 * @brief Exits and cleans up resources for the INOTIFY structure.
 * @param inotify - Pointer to a pointer to the INOTIFY structure.
 */
void
exit_inotify(INOTIFY **inotify);