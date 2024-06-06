#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <systemd/sd-bus.h>
#include <unistd.h>

#include "bus-ifc/bus-ifc.h"
#include "sig-hdlr/handler.h"

int
main(void)
{
  int r = 0;

  if ((r = signal_handler(DEFAULT_HANDLER)) < 0) { return r; }

  notify_success();

  start_dbus_interface(DEFAULT_DBUS_PATH, DEFAULT_DBUS_INTERFACE);

  return 0;
}