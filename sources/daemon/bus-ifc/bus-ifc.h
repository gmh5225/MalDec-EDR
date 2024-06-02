#pragma once

#define DEFAULT_DBUS_PATH "/org/maldec/edr"
#define DEFAULT_DBUS_INTERFACE "org.maldec.edr"

int
start_dbus_interface(const char *path, const char *interface);