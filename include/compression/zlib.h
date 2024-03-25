/**
 * @file zlib.h
 * @brief module for compress and decompress files zip.
 */

#pragma once

#include <zlib/zlib.h>

#include "compiler/compiler_attribute.h"
#include "config.h"
#include "err/err.h"

typedef struct ZLIB
{
  ZLIB_CONFIG config;
  int         fd_in;
  int         fd_out;
  z_stream    stream;
} ZLIB;

ERR
init_zlib(ZLIB **zlib, ZLIB_CONFIG config) warn_unused_result;

ERR
decompress_file(ZLIB **zlib) warn_unused_result;

ERR
compress_file(ZLIB **zlib) warn_unused_result;

void
exit_zlib(ZLIB **zlib);
