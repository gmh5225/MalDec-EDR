// See feature_test_macros(7)
#if !defined(_GNU_SOURCE) || !defined(_POSIX_C_SOURCE)
    // siginfo_t, sigaction()
    #define _GNU_SOURCE
    #define _POSIX_C_SOURCE
#endif

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <systemd/sd-bus.h>
#include <systemd/sd-daemon.h>
#include <unistd.h>

// #include "logger/logger.h"

static void
handler(int signum) {
    switch (signum) {
        case SIGTERM:
            // TODO: shut down the daemon and exit cleanly
            printf("SIGTERM\n");
            sd_notify(0, "STOPPING=1");
            break;
        case SIGHUP:
            printf("SIGHUP\n");
           // TODO: Reload config files
            sd_notify(0, "RELOADING=1\n"
                        "READY=1");
            break;
        default:
            break;
    }
}

// TODO: Add log support
int
signal_handler(void)
{
    struct sigaction action;

    action.sa_handler = handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    if (sigaction(SIGTERM, &action, NULL))
    {
        // LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE %d (%s)", errno, strerror(errno)));
        printf("ERR_FAILURE %d (%s)", errno, strerror(errno));
        return -1;
    }

    if (sigaction(SIGHUP, &action, NULL))
    {
        // LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE %d (%s)", errno, strerror(errno)));
        printf("ERR_FAILURE %d (%s)", errno, strerror(errno));
        return -1;
    }

    return 0;
}

int main(void)
{
    int r = 0;

    if ((r = signal_handler()) < 0) {
        return r;
    }

    sd_notifyf(0,
                "READY=1\n"
                "STATUS=Waiting for some action...\n"
                "MAINPID=%lu",
                (unsigned long)getpid());

    while (1) {}

    return 0;
}