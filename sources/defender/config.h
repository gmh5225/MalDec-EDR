/**
 * @file config.h
 * @brief Header file for the main configuration
 */
#pragma once

#include "compiler/compiler_attribute.h"

typedef struct packed(8) DEFENDER_CONFIG
{
  const char *settings_json_path;
}
DEFENDER_CONFIG;