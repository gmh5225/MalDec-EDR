#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <systemd/sd-bus.h>

#include <unistd.h>

#include "sig-hdlr/handler.h"

// #include "logger/logger.h"

int main(void)
{
    int r = 0;

    if ((r = signal_handler(DEFAULT_HANDLER)) < 0) {
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