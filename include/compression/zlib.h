/**
 * @file zlib.h
 * @brief module for compress and decompress files zip.
 */

#pragma once

#include <zlib.h>

#include "compiler/compiler_attribute.h"
#include "config.h"
#include "err/err.h"

typedef struct ZLIB
{
  CONFIG_ZLIB config;
  int         fd_in;
  int         fd_out;
  int         fd_dir_out;
  int         fd_dir_in;
  z_stream    stream;
} ZLIB;

ERR
save_file_zlib(ZLIB **zlib);

ERR
init_zlib(ZLIB **zlib, CONFIG_ZLIB config);

ERR
decompress_file(ZLIB **zlib);

ERR
compress_file(ZLIB **zlib);

void
exit_zlib(ZLIB **zlib);
