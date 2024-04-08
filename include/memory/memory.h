#pragma once

#include "config.h"

/**
 * @brief Structure representing an memory.
 */
typedef struct MEMORY
{
  MEMORY_CONFIG config; /**< Configuration for the inspector. */
  void         *head;

} MEMORY;

/**
 * @brief Structure representing an memory.
 */
typedef struct BLOCK
{
  void    *claim;
  uint64_t size;

} BLOCK;

void *
init_memory(MEMORY **memory, MEMORY_CONFIG config);

void *
reclaim_memory();

void
free_ptr();

void
exit_memory();
