#include <systemd/sd-bus.h>
#include <stdio.h>
#include <string.h>

#include "bus-ifc.h"

#define DBUS_CLEAN(slot, bus) { sd_bus_slot_unref(slot); sd_bus_unref(bus); }\

// TODO: Remove printf calls

static int method_echo(sd_bus_message *m, void *userdata, sd_bus_error *ret_error)
{
    char *msg = NULL;
    int r = 0;

    r = sd_bus_message_read(m, "s", &msg);
    if (r < 0) {
        printf("Failed to connect to system bus: %s\n", r, strerror(r));
        return r;
    }

    return sd_bus_reply_method_return(m, "s", msg);
}

static const sd_bus_vtable DEFAULT_VTABLE[] = {
    SD_BUS_VTABLE_START(0),
    SD_BUS_METHOD("Echo", "s", "s", method_echo, SD_BUS_VTABLE_UNPRIVILEGED),
    SD_BUS_VTABLE_END
};

int start_dbus_interface(const char *path, const char *interface)
{
    sd_bus_slot *slot = NULL;
    sd_bus *bus = NULL;
    int r = 0;

    // NOTE: Probably I'll need to change this to sd_bus_open_system
    r = sd_bus_open_user(&bus);
    if (r < 0) {
        printf("Failed to connect to system bus: %s\n", r, strerror(r));
        DBUS_CLEAN(slot, bus);
        return r;
    }

    r = sd_bus_add_object_vtable(bus,
                                 &slot,
                                 path,
                                 interface,
                                 DEFAULT_VTABLE,
                                 NULL);

    if (r < 0) {
        printf("Failed to issue method call: %s\n", strerror(-r));
        DBUS_CLEAN(slot, bus);
        return r;
    }

    r = sd_bus_request_name(bus, interface, 0);
    if (r < 0) {
        printf("Failed to acquire service name: %s\n", strerror(-r));
        DBUS_CLEAN(slot, bus);
        return r;
    }

    for (;;) {
            /* Process requests */
            r = sd_bus_process(bus, NULL);
            if (r < 0) {
                fprintf(stderr, "Failed to process bus: %s\n", strerror(-r));
                DBUS_CLEAN(slot, bus);
                return r;
            }

            if (r > 0) /* we processed a request, try to process another one, right-away */
                continue;

            /* Wait for the next request to process */
            r = sd_bus_wait(bus, (uint64_t) -1);
            if (r < 0) {
                fprintf(stderr, "Failed to wait on bus: %s\n", strerror(-r));
                DBUS_CLEAN(slot, bus);
                return r;
            }
        }
}
