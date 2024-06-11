#pragma once

#include <stdint.h>

#include "config.h"

/**
 * @brief Structure representing an memory.
 */
typedef struct MEMORY
{
  MEMORY_CONFIG config; /**< Configuration for the inspector. */
  void         *head;
  void         *ptr;

} MEMORY;

/**
 * @brief Structure representing an memory.
 */
typedef struct BLOCK
{
  void    *mem;
  uint64_t size;

} BLOCK;

void
init_memory(MEMORY **memory, MEMORY_CONFIG config);

void
reclaim_memory();

void
free_block(BLOCK **block);

void
exit_memory(MEMORY **memory);
