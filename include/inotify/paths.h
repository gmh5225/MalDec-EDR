/**
 * @file paths.h
 * @brief Header file for handling directory paths.
 */

#pragma once

#include "compiler/compiler_attribute.h"
#include "uthash/include/uthash.h"

/**
 * @struct PATHS
 * @brief Structure representing a directory path entry.
 */
struct PATHS
{
  char          *path; /**< Directory path to be skipped. */
  UT_hash_handle hh;   /**< UTHASH handle for hash table integration. */
};

/**
 * @brief Adds directory paths to the PATHS structure.
 * @param paths - Pointer to a pointer to the PATHS structure.
 * @param path - Array of directory paths to be added.
 * @param n - Number of paths in the array.
 */
void
add_paths(struct PATHS **paths, const char *path[], size_t n);

/**
 * @brief Deletes and cleans up resources for the PATHS structure.
 * @param paths - Pointer to a pointer to the PATHS structure.
 */
void
del_paths(struct PATHS **paths);