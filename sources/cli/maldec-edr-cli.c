/**
 * @file main.c
 * @brief MalDec-EDR main program.
 * @note Main does not use LOG, only fprintf, without dependence on LOG, all components except json and logger do not use log to save to the file.
 */

#include <getopt.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <systemd/sd-bus.h>

#include "compiler/compiler_attribute.h"
#include "compression/zlib.h"
#include "config.h"
#include "daemon/bus-ifc/bus-ifc.h"
#include "edr/edr.h"
#include "err/err.h"
#include "inspector/inspector.h"
#include "version/version.h"

#define CALL_METHOD(method, sig, data...)                                \
  sd_bus_call_method(bus, DEFAULT_DBUS_INTERFACE, DEFAULT_DBUS_PATH,     \
                     DEFAULT_DBUS_INTERFACE, #method, &error, &msg, sig, \
                     data);
#define BUS_CHECK()                                                            \
  {                                                                            \
    r = sd_bus_message_read(msg, "i", &bus_ret);                               \
    if (r < 0)                                                                 \
    {                                                                          \
      fprintf(stderr, "Failed to parse response message: %s\n", strerror(-r)); \
    }                                                                          \
                                                                               \
    if (bus_ret < 0)                                                           \
    {                                                                          \
      fprintf(stderr, "Something failed on daemon: %s\n", strerror(-r));       \
    }                                                                          \
  }
static inline void no_return
help(char *prog_name)
{
  fprintf(stdout,
          "MalDec EDR (Endpoint detection and response)\n"
          "Usage: %s [OPTIONS]\n\n"
          "Options:\n\n"
          "  Scan :\n"
          "    -s, --scan <file>|<folder>    Scan a specific file or folder "
          "(default "
          "max-depth: X)\n"
          "    -y, --scan-inotify <time>     Enable real-time monitoring "
          "using "
          "inotify. "
          "If a file is created or changed, it will trigger a scan.\n"
          "    -q, --quick                   Perform a quick scan for faster "
          "results\n"
          "    --max-depth <depth>           Set the maximum depth for "
          "folder "
          "scans\n"
          "    --verbose                     Enable verbose mode to display "
          "detailed scan information\n\n"
          "  Quarantine Management:\n"
          "    --view-quarantine            View a list of files currently "
          "in quarantine\n"
          "    --sync-quarantine            Sync files in quarantine and "
          "database\n"
          "    --restore-quarantine <id>\n"
          "                                 Restore a file from quarantine to "
          "another path original\n"
          "    --delete-quarantine <id>\n"
          "                                 Delete a file from quarantine\n\n"
          "  CrowArmor Driver:\n"
          "    --status-crowarmor           Check the driver status and "
          "whether its features are active.\n\n"
          "  General Options:\n"
          "    -v, --version                 Display the version of Linux "
          "edr \n"
          "    -h, --help                    Display this help menu for "
          "Linux "
          "edr\n",
          prog_name);

  exit(ERR_SUCCESS);
}

static inline void no_return
pr_version()
{
  fprintf(stdout, "MalDec-EDR (MalDec Labs) %d.%d.%d\n",
          MALDEC_EDR_VERSION_MAJOR, MALDEC_EDR_VERSION_PATCHLEVEL,
          MALDEC_EDR_VERSION_SUBLEVEL);
  exit(ERR_SUCCESS);
}

void
process_command_line_options(sd_bus *bus, sd_bus_message *msg,
                             sd_bus_error error, int argc, char **argv)
{
  // Command-line options
  struct option long_options[] = {
          {"scan", required_argument, 0, 's'},
          {"scan-inotify", required_argument, 0, 'y'},
          {"verbose", no_argument, 0, 0},
          {"quick", no_argument, 0, 'q'},
          {"max-depth", required_argument, 0, 0},
          {"status-telekinesis", no_argument, 0, 0},
          {"status-crowarmor", no_argument, 0, 0},
          {"version", no_argument, 0, 'v'},
          {"help", no_argument, 0, 'h'},
          {"view-quarantine", no_argument, 0, 0},
          {"delete-quarantine", required_argument, 0, 0},
          {"restore-quarantine", required_argument, 0, 0},
          {"sync-quarantine", no_argument, 0, 0},
          {0, 0, 0, 0},
  };

  int option_index = 0;
  int c, r, bus_ret;

  // Options
  int         max_depth = 0;
  bool        verbose   = false;
  const char *filepath  = NULL;
  uint8_t     scan_type = 0;

  BUS_CHECK();

  while ((c = getopt_long(argc, argv, "qs:y:d:vh", long_options,
                          &option_index)) != -1)
  {
    switch (c)
    {
      case 0:
        // Scan
        if (strcmp(long_options[option_index].name, "max-depth") == 0)
        {
          max_depth = abs(atoi(optarg));
        }

        if (strcmp(long_options[option_index].name, "verbose") == 0)
        {
          verbose = true;
        }

        if (strcmp(long_options[option_index].name, "view-quarantine") == 0)
        {
          const char *json = NULL;
          r                = CALL_METHOD(QuarantineView, "", '\0');
          if (r < 0)
          {
            fprintf(stderr, "Error during \"QuarantineView\" method call: %s\n",
                    strerror(-r));
            return;
          }

          r = sd_bus_message_read(msg, "s", &json);
          if (r < 0)
          {
            fprintf(stderr, "Failed to parse response message: %s\n",
                    strerror(-r));
          }

          printf("%s\n", json);
        }

        // Quarantine for malwares
        if (strcmp(long_options[option_index].name, "sync-quarantine") == 0)
        {
          r = CALL_METHOD(QuarantineSync, "", '\0');
          if (r < 0)
          {
            fprintf(stderr, "Error during \"QuarantineSync\" method call: %s\n",
                    strerror(-r));
            return;
          }

          BUS_CHECK();
        }

        if (strcmp(long_options[option_index].name, "restore-"
                                                    "quarantine") == 0)
        {
          r = CALL_METHOD(QuarantineRestore, "u", atoi(optarg));
          if (r < 0)
          {
            fprintf(stderr,
                    "Error during \"QuarantineRestore\" method call: %s\n",
                    strerror(-r));
            return;
          }

          BUS_CHECK();
        }

        if (strcmp(long_options[option_index].name, "delete-"
                                                    "quarantine") == 0)
        {
          r = CALL_METHOD(QuarantineDelete, "u", atoi(optarg));
          if (r < 0)
          {
            fprintf(stderr,
                    "Error during \"QuarantineDelete\" method call: %s\n",
                    strerror(-r));
            return;
          }

          BUS_CHECK();
        }

        // Driver CrowArmor
        if (strcmp(long_options[option_index].name, "status-"
                                                    "crowarmo"
                                                    "r") == 0)
        {
          r = CALL_METHOD(CrowArmor, "", '\0');
          if (r < 0)
          {
            fprintf(stderr, "Error during \"CrowArmor\" method call: %s\n",
                    strerror(-r));
            return;
          }

          BUS_CHECK();
        }
        break;

      case 's': filepath = strdup(optarg); break;

      case 'q': scan_type |= QUICK_SCAN; break;

      case 'v':
        pr_version();
        break;
        break;

      case 'h':
      case ':':
      case '?':
      default: help(argv[0]); break;
    }
  }

  if (verbose != false || max_depth != 0 || filepath != NULL)
  {
    r = CALL_METHOD(InitParams, "bis", verbose, max_depth, filepath);
    if (r < 0)
    {
      fprintf(stderr, "Error during \"InitParams\" method call: %s\n",
              strerror(-r));
      return;
    }
  }
}

int
main(int argc, char **argv)
{
  if (argc < 2) { help(argv[0]); }

  sd_bus_error    error = SD_BUS_ERROR_NULL;
  sd_bus_message *msg   = NULL;
  sd_bus         *bus   = NULL;
  int             r;

  // NOTE: Probably I'll need to change this to sd_bus_open_system
  r = sd_bus_open_user(&bus);
  if (r < 0)
  {
    fprintf(stderr, "Failed to connect to system bus: %s\n", strerror(-r));
    return -1;
  }

  process_command_line_options(bus, msg, error, argc, argv);

  sd_bus_error_free(&error);
  sd_bus_message_unref(msg);
  sd_bus_unref(bus);

  return ERR_SUCCESS;
}