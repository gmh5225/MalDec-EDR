// See feature_test_macros(7)
#if !defined(_GNU_SOURCE) || !defined(_POSIX_C_SOURCE)
    // siginfo_t, sigaction()
    #define _GNU_SOURCE
    #define _POSIX_C_SOURCE
#endif

#include <signal.h>
#include <systemd/sd-daemon.h>

#include "handler.h"

void _default_handler(int signum) {
    switch (signum) {
        case SIGTERM:
            // TODO: shut down the daemon and exit cleanly
            sd_notify(0, "STOPPING=1");
            break;
        case SIGHUP:
           // TODO: Reload config files
            sd_notify(0, "RELOADING=1\n"
                        "READY=1");
            break;
        default:
            break;
    }
}

// TODO: Add log support
int signal_handler(void(*hdlr)(int))
{
    struct sigaction action;
    int r = 0;

    action.sa_handler = hdlr;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    if ((r = sigaction(SIGTERM, &action, NULL)))
    {
        // LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE %d (%s)", errno, strerror(errno)));
        return r;
    }

    if ((r = sigaction(SIGHUP, &action, NULL)))
    {
        // LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE %d (%s)", r, strerror(r)));
        return r;
    }

    return r;
}

inline int notify_success(void)
{
    return sd_notifyf(0, "READY=1\n"
                         "STATUS=Waiting for some action...\n"
                         "MAINPID=%lu",
                         (unsigned long)getpid());
}