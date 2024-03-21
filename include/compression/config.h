#pragma once

#include <zlib.h>

typedef struct ZLIB_CONFIG
{
  const char *filename_in;
  const char *filename_out;
  int         fd_dir_out;
  int         fd_dir_in;

  int chunk;

} ZLIB_CONFIG;