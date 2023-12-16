#pragma once

#include <stdint.h>

// TODO: Add more flags
#define QUICK_SCAN 1 << 0

typedef struct SCANNER_CONFIG {
    const char* rules;
    uint8_t scan_type;
    int32_t max_depth;
    char *file_path;
    struct skip_dirs *skip;
} SCANNER_CONFIG;