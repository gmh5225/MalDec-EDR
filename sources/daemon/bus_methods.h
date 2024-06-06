/*
* Here will contain all the main function wrappers
* so that we can interface with dbus
*/

#pragma once

#include <systemd/sd-bus.h>

int
init_all(void);

int
method_init_params(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);

void
end_all(void);

int
method_scan(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);

int
method_driver_crowarmor(sd_bus_message *m, void *userdata,
                        sd_bus_error *ret_error);

int
method_quarantine_view(sd_bus_message *m, void *userdata,
                       sd_bus_error *ret_error);
int
method_quarantine_sync(sd_bus_message *m, void *userdata,
                       sd_bus_error *ret_error);
int
method_quarantine_restore(sd_bus_message *m, void *userdata,
                          sd_bus_error *ret_error);
int
method_quarantine_delete(sd_bus_message *m, void *userdata,
                         sd_bus_error *ret_error);