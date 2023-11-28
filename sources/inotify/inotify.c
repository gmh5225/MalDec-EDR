#include "inotify/inotify.h"
#include "logger.h"

#include <sys/inotify.h>
#include <stdlib.h>
#include "err/err.h"

int init_inotify()
{
    int retval = SUCCESS;
    // int fd = inotify_init1(IN_NONBLOCK);
    logger_initConsoleLogger(stderr);
    LOG_INFO("console logging");
    // if (fd == -1)
    {
        exit(EXIT_FAILURE);
    }

    return retval;
}