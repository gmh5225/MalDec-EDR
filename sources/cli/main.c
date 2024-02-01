/**
 * @file main.c
 * @brief Linux Defender Anti0Day main program.
 * @note Main does not use LOG, only fprintf, without dependence on LOG, all components except json and logger do not use log to save to the file.
 */

#include <getopt.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "compiler/compiler_attribute.h"
#include "compression/zlib.h"
#include "config.h"
#include "defender/defender.h"
#include "err/err.h"
#include "inspector/inspector.h"
#include "version/version.h"

static inline void no_return
help(char *prog_name)
{
  fprintf(stdout,
          "Linux Defender Anti0Day\n"
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
          "in "
          "quarantine\n"
          "    --restore-from-quarantine <file>\n"
          "                                 Move a file from quarantine to "
          "another location\n"
          "    --delete-from-quarantine <hash>\n"
          "                                 Delete a file from quarantine\n\n"
          "  Telekinesis Driver :\n"
          "    --connect-telekinesis         Connect to the Telekinesis "
          "driver "
          "shell\n"
          "\n\n"
          "  General Options:\n"
          "    -v, --version                 Display the version of Linux "
          "Defender \n"
          "    -h, --help                    Display this help menu for "
          "Linux "
          "Defender\n",
          prog_name);

  exit(ERR_SUCCESS);
}

static inline void no_return
pr_version()
{
  fprintf(stdout,
          "Linux Defender Anti0Day (Moblog Security Researchers) %d.%d.%d\n",
          LINUX_DEFENDER_VERSION_MAJOR, LINUX_DEFENDER_VERSION_PATCHLEVEL,
          LINUX_DEFENDER_VERSION_SUBLEVEL);
  exit(ERR_SUCCESS);
}

static inline void
cleanup_resources(DEFENDER **defender)
{
  if (IS_NULL_PTR(*defender)) exit(EXIT_SUCCESS);

  if (!IS_NULL_PTR((*defender)->config_json))
    exit_json(&(*defender)->config_json);

  if (!IS_NULL_PTR((*defender)->logger)) exit_logger(&(*defender)->logger);

  if (!IS_NULL_PTR((*defender)->scanner))
    if (IS_ERR_FAILURE(exit_scanner(&(*defender)->scanner)))
      printf(LOG_MESSAGE_FORMAT("Error in exit scanner"));

  if (!IS_NULL_PTR((*defender)->telekinesis))
    exit_driver_telekinesis(&(*defender)->telekinesis);

  if (!IS_NULL_PTR((*defender)->inotify)) exit_inotify(&(*defender)->inotify);

  if (!IS_NULL_PTR((*defender)->inspector))
    exit_inspector(&(*defender)->inspector);

  exit_defender(defender);
  exit(EXIT_SUCCESS);
}

void
process_command_line_options(DEFENDER **defender, int argc, char **argv)
{
  // Command-line options
  struct option long_options[] = {
          {"scan", required_argument, 0, 's'},
          {"scan-inotify", required_argument, 0, 'y'},
          {"verbose", no_argument, 0, 0},
          {"quick", no_argument, 0, 'q'},
          {"max-depth", required_argument, 0, 0},
          {"connect-telekinesis", no_argument, 0, 0},
          {"version", no_argument, 0, 'v'},
          {"help", no_argument, 0, 'h'},
          {"view-quarantine", no_argument, 0, 0},
          {"move-from-quarantine", required_argument, 0, 0},
          {"delete-from-quarantine", required_argument, 0, 0},
          {0, 0, 0, 0},
  };

  int option_index = 0;
  int c;

  while ((c = getopt_long(argc, argv, "qs:y:d:vh", long_options,
                          &option_index)) != -1)
  {
    switch (c)
    {
      case 0:
        // Scan Yara
        if (!IS_NULL_PTR((*defender)->scanner))
        {
          if (strcmp(long_options[option_index].name, "max-depth") == 0)
          {
            uint32_t max_depth = (uint32_t)atoi(optarg);
            (*defender)->scanner->config.max_depth =
                    (max_depth <= 0) ? 0 : max_depth;
          }
          if (strcmp(long_options[option_index].name, "verbose") == 0)
            (*defender)->scanner->config.verbose = true;
        }

        // Driver telekinesis
        if (strcmp(long_options[option_index].name, "connect-telekinesis") == 0)
        {
          init_telekinesis_main(defender);
          connect_driver_telekinesis((*defender)->telekinesis);
          return;
        }

        if (strcmp(long_options[option_index].name, "view-quarantine") == 0)
        {
          init_cjson_main(defender);
          init_logger_main(defender);
          init_inspector_main(defender);
          if (IS_ERR_FAILURE(view_quarantine_inspector((*defender)->inspector)))
          {
          };
          return;
        }

        break;

      case 's':
        init_cjson_main(defender);
        init_logger_main(defender);
        init_inspector_main(defender);
        init_scanner_main(defender);
        (*defender)->scanner->config.filepath  = optarg;
        (*defender)->scanner->config.inspector = (*defender)->inspector;
        break;

      case 'q': (*defender)->scanner->config.scan_type |= QUICK_SCAN; break;

      case 'v': pr_version(); break;

      case 'y':
        init_cjson_main(defender);
        init_logger_main(defender);
        init_inspector_main(defender);
        init_scanner_main(defender);
        init_inotify_main(defender);
        (*defender)->inotify->config.mask =
                (IN_MODIFY | IN_CLOSE_WRITE | IN_CREATE);
        (*defender)->inotify->config.time      = atoi(optarg);
        (*defender)->scanner->config.inotify   = (*defender)->inotify;
        (*defender)->scanner->config.inspector = (*defender)->inspector;
        set_watch_paths((*defender)->inotify);
        if (IS_ERR_FAILURE(scan_listen_inotify((*defender)->scanner)))
        {
          fprintf(stderr, LOG_MESSAGE_FORMAT("Error scan inotify\n"));
        }
        break;

      case 'h':
      case ':':
      case '?':
      default: help(argv[0]); break;
    }
  }

  // Scan Yara
  if (!IS_NULL_PTR((*defender)->scanner) && IS_NULL_PTR((*defender)->inotify))
    if (IS_ERR_FAILURE(scan((*defender)->scanner)))
      fprintf(stderr, LOG_MESSAGE_FORMAT("Error in scan\n"));
}

int
main(int argc, char **argv)
{
  if (argc < 2) { help(argv[0]); }

  DEFENDER *defender;

  DEFENDER_CONFIG config = (DEFENDER_CONFIG){
#ifdef DEBUG
          .settings_json_path = "../../../config/"
                                "appsettings.development.json",
#else
          .settings_json_path = "../../../config/appsettings.json",
#endif
  };

  init_defender(&defender, config);

  process_command_line_options(&defender, argc, argv);

  cleanup_resources(&defender);

  return ERR_SUCCESS;
}