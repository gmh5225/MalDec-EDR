#include "inotify/inotify.h"

#include <sys/inotify.h>
#include <stdlib.h>
#include "err/err.h"

int init_inotify()
{
    int retval = SUCCESS;
    int fd = inotify_init1(IN_NONBLOCK);
   
    if (fd == -1)
    {
        exit(EXIT_FAILURE);
    }

    return retval;
}