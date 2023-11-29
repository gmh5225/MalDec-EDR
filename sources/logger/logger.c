#include "logger/logger.h"

void init_logger(const char *path)
{
    logger_initConsoleLogger(NULL);
    logger_initFileLogger(path, 0, 0);
    logger_setLevel(LogLevel_DEBUG);
}