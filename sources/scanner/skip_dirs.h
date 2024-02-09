/**
 * @file skip_dirs.h
 * @brief Defines structures and functions for skip dirs scan.
 */

#pragma once

#include "compiler/compiler_attribute.h"
#include "uthash/include/uthash.h"

/**
 * @brief Struct for skip dirs in scan
 * 
 */
struct SKIP_DIRS
{
  char          *dir; /**< Directory path to be skipped. */
  UT_hash_handle hh;  /**< UTHASH handle for hash table integration. */
};

/**
 * @brief Adds directories to the list of directories to be skipped.
 *
 * This function adds an array of directory paths to the list of directories to be skipped during
 * scanning.
 *
 * @param[in,out] skip Pointer to a pointer to the SKIP_DIRS structure.
 * @param[in] path Array of directory paths to be added.
 * @param[in] n Number of elements in the 'path' array.
 */
void
add_skip_dirs(struct SKIP_DIRS **skip, const char *path[], size_t n);

/**
 * @brief Deletes the list of directories to be skipped and frees associated resources.
 *
 * @param[in,out] skip Pointer to a pointer to the SKIP_DIRS structure.
 */
void
del_skip_dirs(struct SKIP_DIRS **skip);

/**
 * @brief Gets the SKIP_DIRS entry for a specific directory path.
 *
 * This function retrieves the SKIP_DIRS entry for a specific directory path.
 *
 * @param[in] skip Pointer to the SKIP_DIRS structure.
 * @param[in] path Directory path to look up.
 * @return Returns a pointer to the SKIP_DIRS entry if found, NULL otherwise.
 */
struct SKIP_DIRS *
get_skipped(struct SKIP_DIRS **skip, const char *path) warn_unused_result;
