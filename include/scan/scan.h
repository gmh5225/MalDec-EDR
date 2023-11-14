#pragma once

#include <yara.h>
#include "scan/skip_dirs.h"

#define DEFAULT_SCAN_CALLBACK default_scan_callback 

typedef struct SCANNER {
    YR_RULES *yr_rules;
    YR_COMPILER *yr_compiler;
} SCANNER;

int scanner_init(SCANNER **scanner);
int scanner_destroy(SCANNER **scanner);
int scan_file(SCANNER *scanner, const char *file, YR_CALLBACK_FUNC callback);
int scan_dir(SCANNER *scanner, const char *dir, YR_CALLBACK_FUNC callback, struct skip_dirs *skip);
int default_scan_callback(YR_SCAN_CONTEXT *context, int message, void *message_data, void *user_data);
