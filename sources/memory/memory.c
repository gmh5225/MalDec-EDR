#include "memory/memory.h"
#include "err/err.h"
#include <stdlib.h>

void
init_memory(MEMORY **memory, MEMORY_CONFIG config)
{
  *memory = malloc(sizeof(struct MEMORY));
  ALLOC_ERR_FAILURE(*memory);

  (*memory)->config = config;
}

void
reclaim_memory()
{
}

void
free_block(BLOCK **block)
{
  *block = NULL;
}

void
exit_memory(MEMORY **memory)
{
  if (IS_NULL_PTR(*memory)) free(*memory);
}
