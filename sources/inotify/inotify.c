#include "inotify/inotify.h"
#include "logger.h"

#include <sys/inotify.h>
#include <stdlib.h>
#include <errno.h>

inline bool init_inotify()
{
    //int fd = inotify_init1(IN_NONBLOCK);
    LOG_INFO("console logging");
    //if (fd == -1)
    {
        // fprintf(stderr, "Inotify : scan_dir ERROR %s : %d (%s)\n", dir, errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
}