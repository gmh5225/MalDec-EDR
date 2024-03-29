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

#include "compiler/compiler_attribute.h"
#include "compression/zlib.h"
#include "config.h"
#include "edr/edr.h"
#include "err/err.h"
#include "inspector/inspector.h"
#include "version/version.h"

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
          "  Telekinesis Driver:\n"
          "    --status-telekinesis         Check the driver status and "
          "whether its features are active.\n\n"
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

static inline void
cleanup_resources(EDR **edr)
{
  if (IS_NULL_PTR(*edr)) exit(EXIT_SUCCESS);

  if (!IS_NULL_PTR((*edr)->cjson)) exit_json(&(*edr)->cjson);

  if (!IS_NULL_PTR((*edr)->logger)) exit_logger(&(*edr)->logger);

  if (!IS_NULL_PTR((*edr)->scanner))
    if (IS_ERR_FAILURE(exit_scanner(&(*edr)->scanner)))
      printf(LOG_MESSAGE_FORMAT("Error in exit scanner"));

  if (!IS_NULL_PTR((*edr)->telekinesis))
    exit_driver_telekinesis(&(*edr)->telekinesis);

  if (!IS_NULL_PTR((*edr)->inotify)) exit_inotify(&(*edr)->inotify);

  if (!IS_NULL_PTR((*edr)->inspector)) exit_inspector(&(*edr)->inspector);

  exit(EXIT_SUCCESS);
}

void
process_command_line_options(EDR **edr, int argc, char **argv)
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
  int c;

  while ((c = getopt_long(argc, argv, "qs:y:d:vh", long_options,
                          &option_index)) != -1)
  {
    switch (c)
    {
      case 0:
        // Scan
        if (!IS_NULL_PTR((*edr)->scanner))
        {
          if (strcmp(long_options[option_index].name, "max-depth") == 0)
          {
            int max_depth                     = atoi(optarg);
            (*edr)->scanner->config.max_depth = abs(max_depth);
          }
          if (strcmp(long_options[option_index].name, "verbose") == 0)
            (*edr)->scanner->config.verbose = true;
        }

        // Driver telekinesis
        if (strcmp(long_options[option_index].name, "status-telekinesis") == 0)
        {
          init_cjson_main(edr);
          init_logger_main(edr);
          init_telekinesis_main(edr);
        }

        if (strcmp(long_options[option_index].name, "view-quarantine") == 0)
        {
          init_cjson_main(edr);
          init_logger_main(edr);
          init_inspector_main(edr);
          if (IS_ERR_FAILURE(view_quarantine_inspector(
                      (*edr)->inspector, DEFAULT_VIEW_QUARANTINE)))
          {
            fprintf(stderr, LOG_MESSAGE_FORMAT("Error view quarantine\n"));
          };
        }

        // Quarantine for malwares
        if (strcmp(long_options[option_index].name, "sync-quarantine") == 0)
        {
          init_cjson_main(edr);
          init_logger_main(edr);
          init_inspector_main(edr);
          if (IS_ERR_FAILURE(sync_quarantine_inspector(
                      (*edr)->inspector, DEFAULT_SYNC_QUARANTINE)))
          {
            fprintf(stderr, LOG_MESSAGE_FORMAT("Error sync quarantine\n"));
          };
        }

        if (strcmp(long_options[option_index].name, "restore-"
                                                    "quarantine") == 0)
        {
          init_cjson_main(edr);
          init_logger_main(edr);
          init_inspector_main(edr);

          QUARANTINE_FILES file = (QUARANTINE_FILES){.id = atoi(optarg)};
          if (IS_ERR_FAILURE(
                      restore_quarantine_inspector((*edr)->inspector, &file)))
          {
            fprintf(stderr, LOG_MESSAGE_FORMAT("Error in restore file in "
                                               "quarantine\n"));
          };
        }

        if (strcmp(long_options[option_index].name, "delete-"
                                                    "quarantine") == 0)
        {
          init_cjson_main(edr);
          init_logger_main(edr);
          init_inspector_main(edr);

          QUARANTINE_FILES file = (QUARANTINE_FILES){.id = atoi(optarg)};
          if (IS_ERR_FAILURE(
                      del_quarantine_inspector((*edr)->inspector, &file)))
          {
            fprintf(stderr, LOG_MESSAGE_FORMAT("Error in delete file in "
                                               "quarantine\n"));
          };
        }

        // Driver CrowArmor
        if (strcmp(long_options[option_index].name, "status-crowarmor") == 0)
        {
          init_cjson_main(edr);
          init_logger_main(edr);
          init_crowarmor_main(edr);
          check_driver_crowarmor_activated((*edr)->crowarmor);
        }
        break;

      case 's':
        init_cjson_main(edr);
        init_logger_main(edr);
        init_inspector_main(edr);
        init_scanner_main(edr);
        (*edr)->scanner->config.filepath  = optarg;
        (*edr)->scanner->config.inspector = (*edr)->inspector;
        break;

      case 'q': (*edr)->scanner->config.scan_type |= QUICK_SCAN; break;

      case 'v': pr_version(); break;

      case 'y':
        init_cjson_main(edr);
        init_logger_main(edr);
        init_scanner_main(edr);
        init_inotify_main(edr);
        init_inspector_main(edr);
        (*edr)->inotify->config.mask = (IN_MODIFY | IN_CLOSE_WRITE | IN_CREATE);
        (*edr)->inotify->config.time = atoi(optarg);
        (*edr)->scanner->config.inotify   = (*edr)->inotify;
        (*edr)->scanner->config.inspector = (*edr)->inspector;
        set_watch_paths((*edr)->inotify);
        if (IS_ERR_FAILURE(scan_listen_inotify((*edr)->scanner)))
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
  if (!IS_NULL_PTR((*edr)->scanner) && IS_NULL_PTR((*edr)->inotify))
    if (IS_ERR_FAILURE(scan((*edr)->scanner)))
      fprintf(stderr, LOG_MESSAGE_FORMAT("Error in scan\n"));
}

int
main(int argc, char **argv)
{
  if (argc < 2) { help(argv[0]); }

  EDR *edr;

  DEFENDER_CONFIG config = (DEFENDER_CONFIG){
#ifdef DEBUG
          .settings_json_path = "../../../config/"
                                "appsettings.development.json",

#else
          .settings_json_path = "../../../config/appsettings.json",
#endif
  };

  init_edr(&edr, config);

  process_command_line_options(&edr, argc, argv);

  cleanup_resources(&edr);

  exit_edr(&edr);

  return ERR_SUCCESS;
}