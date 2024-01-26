#pragma once

#include "config.h"
#include "err/err.h"

typedef struct INSPECTOR
{
  INSPECTOR_CONFIG config;
} INSPECTOR;

ERR
init_inspector(INSPECTOR **inspector, INSPECTOR_CONFIG config);

void
exit_inspector(INSPECTOR **inspector);