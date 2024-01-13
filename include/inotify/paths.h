#pragma once

#include "compiler/compiler_attribute.h"
#include "uthash/include/uthash.h"

struct PATHS
{
  char          *path; /**< Directory path to be skipped. */
  UT_hash_handle hh;   /**< UTHASH handle for hash table integration. */
};

void
add_paths(struct PATHS **paths, const char *path[], size_t n);

void
del_paths(struct PATHS **paths);