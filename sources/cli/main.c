/**
 * @file main.c
 * @brief Linux Defender Anti0Day main program.
 */

#include <getopt.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "compiler/compiler_attribute.h"
#include "config.h"
#include "err/err.h"
#include "version/version.h"

// Configuration file path
#define CONFIG_JSON_PATH "../../../config/appsettings.json"

/**
 * @brief Main function of the Linux Defender Anti0Day program.
 *
 * This function initializes the program, processes command-line options,
 * and executes the corresponding actions based on the provided options.
 *
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line argument strings.
 * @return ERR_SUCCESS on success, ERR_FAILURE on failure.
 */
int
main(int argc, char **argv);

/**
 * @brief Initializes the main logger configuration based on a JSON object.
 *
 * This function retrieves logger configuration parameters from a JSON object
 * and initializes the main logger accordingly.
 */
void
init_logger_main();

/**
 * @brief Initializes the Telekinesis driver configuration based on a JSON object.
 *
 * This function retrieves Telekinesis driver configuration parameters from a
 * JSON object and initializes the Telekinesis driver accordingly.
 */
void
init_telekinesis_main();

/**
 * @brief Initializes the main scanner configuration based on a JSON object.
 *
 * This function retrieves scanner configuration parameters from a JSON object
 * and initializes the main scanner accordingly.
 */
void
init_scanner_main();

/**
 * @brief Initializes the Inotify configuration based on a JSON object.
 *
 * This function retrieves Inotify configuration parameters from a JSON object
 * and initializes Inotify accordingly.
 */
void
init_inotify_main();

/**
 * @brief Processes command-line options and executes corresponding actions.
 *
 * This function parses command-line options using getopt_long and executes
 * actions based on the provided options.
 *
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line argument strings.
 */
void
process_command_line_options(int argc, char **argv);

/**
 * @brief Cleans up allocated resources and exits the program.
 *
 * This function is registered using atexit to perform cleanup tasks when the
 * program exits. It releases memory and resources allocated during program
 * execution.
 */
void
cleanup_resources();

/**
 * @brief Displays the help menu with usage information.
 *
 * This function prints the help menu, explaining the usage and available
 * options of the Linux Defender Anti0Day program.
 *
 * @param prog_name Name of the program (argv[0]).
 */
void
help(char *prog_name);

/**
 * @brief Displays the version information of Linux Defender Anti0Day.
 *
 * This function prints the version information of the Linux Defender Anti0Day program.
 */
void
pr_version();

int
main(int argc, char **argv)
{
  if (argc < 2) { help(argv[0]); }

  ERR retval = ERR_SUCCESS;

  DEFENDER_CONFIG.logger      = NULL;
  DEFENDER_CONFIG.scanner     = NULL;
  DEFENDER_CONFIG.config_json = NULL;
  DEFENDER_CONFIG.telekinesis = NULL;
  DEFENDER_CONFIG.inotify     = NULL;
  atexit(cleanup_resources);

  if (IS_ERR_FAILURE(init_json(&DEFENDER_CONFIG.config_json, CONFIG_JSON_PATH)))
  {
    fprintf(stderr, LOG_MESSAGE_FORMAT("Error in parser json config '%s'\n",
                                       CONFIG_JSON_PATH));
    retval = ERR_FAILURE;
    goto ret;
  }
  else
  {
    init_logger_main();
    process_command_line_options(argc, argv);
  }

ret:
  return retval;
}

void
init_logger_main()
{
  struct json_object *logger_obj, *filename_obj, *max_file_size_obj,
          *max_backup_files_obj, *level_obj, *console_obj;

  if (json_object_object_get_ex(DEFENDER_CONFIG.config_json, "logger",
                                &logger_obj))
  {
    if (!json_object_object_get_ex(logger_obj, "filename", &filename_obj) ||
        !json_object_object_get_ex(logger_obj, "max_file_size",
                                   &max_file_size_obj) ||
        !json_object_object_get_ex(logger_obj, "max_backup_files",
                                   &max_backup_files_obj) ||
        !json_object_object_get_ex(logger_obj, "level", &level_obj) ||
        !json_object_object_get_ex(logger_obj, "console", &console_obj))
    {
      fprintf(stderr, LOG_MESSAGE_FORMAT("Unable to retrieve logger "
                                         "configuration from JSON\n"));
      exit(ERR_FAILURE);
    }
  }
  else
  {
    fprintf(stderr, LOG_MESSAGE_FORMAT("Unable to retrieve 'logger' object "
                                       "from JSON\n"));
    exit(ERR_FAILURE);
  }

  LOGGER_CONFIG logger_config = (LOGGER_CONFIG){
          .filename         = json_object_get_string(filename_obj),
          .level            = json_object_get_int(level_obj),
          .max_backup_files = json_object_get_int(max_backup_files_obj),
          .max_file_size    = json_object_get_int(max_file_size_obj),
          .console          = json_object_get_boolean(console_obj)};

  if (IS_ERR_FAILURE(init_logger(&DEFENDER_CONFIG.logger, logger_config)))
  {
    fprintf(stderr, LOG_MESSAGE_FORMAT("Error init logger\n"));
    exit(ERR_FAILURE);
  }
}

void
init_telekinesis_main()
{
  struct json_object *drivers_obj, *telekinesis_obj, *driver_path_obj,
          *driver_name_obj;

  if (json_object_object_get_ex(DEFENDER_CONFIG.config_json, "drivers",
                                &drivers_obj))
  {
    if (!json_object_object_get_ex(drivers_obj, "telekinesis",
                                   &telekinesis_obj) ||
        !json_object_object_get_ex(telekinesis_obj, "driver_path",
                                   &driver_path_obj) ||
        !json_object_object_get_ex(telekinesis_obj, "driver_name",
                                   &driver_name_obj))
    {
      fprintf(stderr, LOG_MESSAGE_FORMAT("Unable to retrieve scan "
                                         "configuration from JSON\n"));
      exit(ERR_FAILURE);
    }
  }

  TELEKINESIS_CONFIG config = (TELEKINESIS_CONFIG){
          .driver_name = json_object_get_string(driver_name_obj),
          .driver_path = json_object_get_string(driver_path_obj)};

  if (IS_ERR_FAILURE(
              init_driver_telekinesis(&DEFENDER_CONFIG.telekinesis, config)))
  {
    fprintf(stderr, LOG_MESSAGE_FORMAT("Error in init driver %s\n",
                                       config.driver_name));
    exit(EXIT_FAILURE);
  }
}

void
init_scanner_main()
{
  struct json_object *scan_obj, *yara_obj, *rules_obj, *skip_dir_objs;
  if (json_object_object_get_ex(DEFENDER_CONFIG.config_json, "scan", &scan_obj))
  {
    if (!json_object_object_get_ex(scan_obj, "yara", &yara_obj) ||
        !json_object_object_get_ex(yara_obj, "rules", &rules_obj) ||
        !json_object_object_get_ex(yara_obj, "skip_dirs", &skip_dir_objs))
    {
      fprintf(stderr, LOG_MESSAGE_FORMAT("Unable to retrieve scan "
                                         "configuration from JSON\n"));
      exit(ERR_FAILURE);
    }
  }

  const int    length   = json_object_array_length(skip_dir_objs);
  const char **skip_dir = alloca(length * sizeof(const char *));
  for (int i = 0; i < length; ++i)
  {
    struct json_object *element = json_object_array_get_idx(skip_dir_objs, i);
    skip_dir[i] = (element != NULL) ? json_object_get_string(element) : NULL;
  }

  struct SKIP_DIRS *skip = NULL;
  add_skip_dirs(&skip, skip_dir, length);

  SCANNER_CONFIG config =
          (SCANNER_CONFIG){.file_path = NULL,
                           .max_depth = -1,
                           .scan_type = 0,
                           .verbose   = false,
                           .rules     = json_object_get_string(rules_obj),
                           .skip      = skip};

  if (IS_ERR_FAILURE(init_scanner(&DEFENDER_CONFIG.scanner, config)))
  {
    fprintf(stderr, LOG_MESSAGE_FORMAT("Error init scanner\n"));
    exit(ERR_FAILURE);
  }
}

void
init_inotify_main()
{
  struct json_object *inotify_obj, *paths_obj;
  if (json_object_object_get_ex(DEFENDER_CONFIG.config_json, "inotify",
                                &inotify_obj))
  {
    if (!json_object_object_get_ex(inotify_obj, "paths", &paths_obj))
    {
      fprintf(stderr, LOG_MESSAGE_FORMAT("Unable to retrieve scan "
                                         "configuration from JSON\n"));
      exit(ERR_FAILURE);
    }
  }

  const int    length = json_object_array_length(paths_obj);
  const char **path   = alloca(length * sizeof(const char *));
  for (int i = 0; i < length; ++i)
  {
    struct json_object *element = json_object_array_get_idx(paths_obj, i);
    path[i] = (element != NULL) ? json_object_get_string(element) : NULL;
  }

  struct PATHS *paths = NULL;
  add_paths(&paths, path, length);

  INOTIFY_CONFIG config =
          (INOTIFY_CONFIG){.paths = paths, .quantity_fds = length};

  if (IS_ERR_FAILURE(init_inotify(&DEFENDER_CONFIG.inotify, config)))
  {
    fprintf(stderr, LOG_MESSAGE_FORMAT("Error init inotify\n"));
    exit(ERR_FAILURE);
  }
}

void
process_command_line_options(int argc, char **argv)
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
        if (!IS_NULL_PTR(DEFENDER_CONFIG.scanner))
        {
          if (strcmp(long_options[option_index].name, "max-depth") == 0)
          {
            uint32_t max_depth = (uint32_t)atoi(optarg);
            DEFENDER_CONFIG.scanner->config.max_depth =
                    (max_depth <= 0) ? 0 : max_depth;
          }
          if (strcmp(long_options[option_index].name, "verbose") == 0)
            DEFENDER_CONFIG.scanner->config.verbose = true;
        }

        // Driver telekinesis
        if (strcmp(long_options[option_index].name, "connect-telekinesis") == 0)
        {
          init_telekinesis_main();
          connect_driver_telekinesis(DEFENDER_CONFIG.telekinesis);
          return;
        }
        break;

      case 's':
        init_scanner_main();
        DEFENDER_CONFIG.scanner->config.file_path = optarg;
        break;

      case 'q': DEFENDER_CONFIG.scanner->config.scan_type |= QUICK_SCAN; break;

      case 'v': pr_version(); break;

      case 'y':
        init_scanner_main();
        init_inotify_main();
        DEFENDER_CONFIG.inotify->config.time = atoi(optarg);
        scan_listen(DEFENDER_CONFIG.scanner, DEFENDER_CONFIG.inotify);
        break;

      case 'h':
      case ':':
      case '?':
      default: help(argv[0]); break;
    }
  }

  // Scan Yara
  if (!IS_NULL_PTR(DEFENDER_CONFIG.scanner) &&
      IS_NULL_PTR(DEFENDER_CONFIG.inotify))
    if (IS_ERR_FAILURE(scan(DEFENDER_CONFIG.scanner)))
      printf(LOG_MESSAGE_FORMAT("Error in scan"));
}

void
cleanup_resources()
{
  if (!IS_NULL_PTR(DEFENDER_CONFIG.config_json))
    exit_json(&DEFENDER_CONFIG.config_json);
  if (!IS_NULL_PTR(DEFENDER_CONFIG.logger))
    exit_logger(&DEFENDER_CONFIG.logger);
  if (!IS_NULL_PTR(DEFENDER_CONFIG.scanner) &&
      IS_ERR_FAILURE(exit_scanner(&DEFENDER_CONFIG.scanner)))
    printf(LOG_MESSAGE_FORMAT("Error in exit scanner"));
  if (!IS_NULL_PTR(DEFENDER_CONFIG.telekinesis))
    exit_driver_telekinesis(&DEFENDER_CONFIG.telekinesis);
  if (!IS_NULL_PTR(DEFENDER_CONFIG.inotify))
    exit_inotify(&DEFENDER_CONFIG.inotify);
}

void
help(char *prog_name)
{
  printf("Linux Defender Anti0Day\n"
         "Usage: %s [OPTIONS]\n\n"
         "Options:\n\n"
         "  Scan :\n"
         "    -s, --scan <file>|<folder>    Scan a file or folder (default "
         "max-depth X)\n"
         "    -y, --scan-inotify <time>     Place the file scan in a "
         "monitoring system\n"
         "                                  if a file is created or changed it "
         "will scan the "
         "file\n"
         "    -q, --quick                   Enable quick scan\n"
         "    --max-depth <depth>           Set max-depth for folder scan\n"
         "    --verbose                     Enable verbose mode for scan\n\n"
         "  Telekinesis Driver :\n"
         "    --connect-telekinesis         Connect driver Telekinesis shell\n"
         "\n\n"
         "    -v, --version                 Display the version of Linux "
         "Defender \n"
         "    -h, --help                    Display this help menu Linux "
         "Defender\n",
         prog_name);

  exit(ERR_SUCCESS);
}

void
pr_version()
{
  printf("Linux Defender Anti0Day (Moblog Security Researchers) %d.%d.%d\n",
         LINUX_DEFENDER_VERSION_MAJOR, LINUX_DEFENDER_VERSION_PATCHLEVEL,
         LINUX_DEFENDER_VERSION_SUBLEVEL);
  exit(ERR_SUCCESS);
}