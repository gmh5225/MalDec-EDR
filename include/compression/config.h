#pragma once

#include <zlib.h>

typedef struct CONFIG_ZLIB
{
  const char *file_name_in;
  const char *file_name_out;
  const char *dir_out;
  const char *dir_in;

} CONFIG_ZLIB;