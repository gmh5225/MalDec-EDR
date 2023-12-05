#include "c-logger/src/logger.h"

typedef struct LOGGER_CONFIG
{
    const char *filename;
    long max_file_size;
    unsigned char max_backup_files;
    LogLevel level;
} LOGGER_CONFIG;