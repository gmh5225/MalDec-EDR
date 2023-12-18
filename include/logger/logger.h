#pragma once

#include "compiler/compiler_attribute.h"
#include "c-logger/src/logger.h"
#include "config.h"

typedef struct LOGGER
{
    LOGGER_CONFIG config;
} LOGGER;

int init_logger(LOGGER** logger, LOGGER_CONFIG config) warn_unused_result;
void exit_logger(LOGGER** logger);