#pragma once

#include <yara.h>

// #include <yara/types.h>

#define DEFAULT_SCAN_CALLBACK default_scan_callback 

typedef struct SCANNER {
    YR_RULES *yr_rules;
    YR_COMPILER *yr_compiler;
} SCANNER;

int scanner_init(SCANNER **scanner);
int scan_file(SCANNER *scanner, const char *file, YR_CALLBACK_FUNC callback);
int scan_folder(SCANNER *scanner, const char *dir, YR_CALLBACK_FUNC callback);
int default_scan_callback(YR_SCAN_CONTEXT *context, int message, void *message_data, void *user_data);
