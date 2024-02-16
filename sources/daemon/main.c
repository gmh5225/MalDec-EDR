#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <systemd/sd-bus.h>

#include <unistd.h>

#include "sig-hdlr/handler.h"
#include "bus-ifc/bus-ifc.h"

// #include "logger/logger.h"

int main(void)
{
    int r = 0;

    if ((r = signal_handler(DEFAULT_HANDLER)) < 0) {
        return r;
    }

    notify_success();

    start_dbus_interface("/net/defender/LinuxDefender", "net.defender.LinuxDefender");

    return 0;
}