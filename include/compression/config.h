#pragma once

#include <zlib.h>

typedef struct CONFIG_ZLIB
{
  const char *file_name_in;
  const char *file_name_out;
  int         dirfd_out;

} CONFIG_ZLIB;