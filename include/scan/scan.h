#pragma once

#include <yara.h>
#include "scan/skip_dirs.h"
#include "scan/config.h"

#define DEFAULT_SCAN_CALLBACK default_scan_callback 

typedef struct SCANNER {
    YR_RULES *yr_rules;
    YR_COMPILER *yr_compiler;
    SCANNER_CONFIG config;
} SCANNER;

int scan(SCANNER *scanner);
int scanner_init(SCANNER **scanner, SCANNER_CONFIG config);
int scanner_destroy(SCANNER **scanner);
int scan_file(SCANNER *scanner, YR_CALLBACK_FUNC callback);
int scan_dir(SCANNER *scanner, YR_CALLBACK_FUNC callback, int32_t __current_depth);
int default_scan_callback(YR_SCAN_CONTEXT *context, int message, void *message_data, void *user_data);
