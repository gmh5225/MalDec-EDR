#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <stdint.h>

#include "cjson/cjson.h"
#include "inotify/inotify.h"
#include "scan/config.h"
#include "scan/scan.h"
#include "version/version.h"
#include "err/err.h"
#include "compiler/compiler_attribute.h"
#include "logger/logger.h"

#define CONFIG_JSON_PATH "../../../config/appsettings.json"

inline void init_logger_main(LOGGER **logger, struct json_object **json_obj);
inline void init_scanner_main(SCANNER **scanner, struct json_object **json_obj);
inline void process_command_line_options(int argc, char **argv, SCANNER **scanner);
inline void cleanup_resources(struct json_object **json_obj, LOGGER **logger, SCANNER **scanner);
inline void help(char *prog_name) no_return;
inline void pr_version();

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		help(argv[0]);
	}

	int retval = SUCCESS;

	LOGGER *logger = NULL;
	SCANNER *scanner = NULL;
	struct json_object *json_obj = NULL;

	if (IS_ERR(init_json(&json_obj, CONFIG_JSON_PATH)))
	{
		fprintf(stderr, "Main : Error in parser json '%s'\n", CONFIG_JSON_PATH);
		retval = ERROR;
		goto ret;
	}

	init_logger_main(&logger, &json_obj);
	init_scanner_main(&scanner, &json_obj);

	process_command_line_options(argc, argv, &scanner);

	// Perform scanning if a file path is provided
	if (!IS_NULL_PTR(scanner->config.file_path))
	{
		retval = scan(scanner);
	}

ret:
	// clean up and exit
	cleanup_resources(&json_obj, &logger, &scanner);
	return retval;
}

void init_logger_main(LOGGER **logger, struct json_object **json_obj)
{
	struct json_object *logger_obj,
		*filename_obj,
		*max_file_size_obj,
		*max_backup_files_obj,
		*level_obj,
		*console_obj;

	if (json_object_object_get_ex(*json_obj, "logger", &logger_obj))
	{
		if (!json_object_object_get_ex(logger_obj, "filename", &filename_obj) ||
			!json_object_object_get_ex(logger_obj, "max_file_size", &max_file_size_obj) ||
			!json_object_object_get_ex(logger_obj, "max_backup_files", &max_backup_files_obj) ||
			!json_object_object_get_ex(logger_obj, "level", &level_obj) ||
			!json_object_object_get_ex(logger_obj, "console", &console_obj))
		{
			fprintf(stderr, "Init_logger_main : Unable to retrieve logger configuration from JSON\n");
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		fprintf(stderr, "Init_logger_main : Unable to retrieve 'logger' object from JSON\n");
		exit(EXIT_FAILURE);
	}

	LOGGER_CONFIG logger_config = (LOGGER_CONFIG){
		.filename = json_object_get_string(filename_obj),
		.level = json_object_get_int(level_obj),
		.max_backup_files = json_object_get_int(max_backup_files_obj),
		.max_file_size = json_object_get_int(max_file_size_obj),
		.console = json_object_get_boolean(console_obj)};

	if (IS_ERR(init_logger(logger, logger_config)))
	{
		fprintf(stderr, "Init_logger_main : Error init logger\n");
		exit(EXIT_FAILURE);
	}
}

void init_scanner_main(SCANNER **scanner, struct json_object **json_obj)
{
	struct json_object *scan_obj, *rules_obj, *skip_dir_objs;
	if (json_object_object_get_ex(*json_obj, "scan", &scan_obj))
	{
		if (!json_object_object_get_ex(scan_obj, "rules", &rules_obj) ||
			!json_object_object_get_ex(scan_obj, "skip_dirs", &skip_dir_objs))
		{
			fprintf(stderr, "Init_scanner_main : Unable to retrieve scan configuration from JSON\n");
			exit(EXIT_FAILURE);
		}
	}

	struct skip_dirs *skip = NULL;
	add_skip_dirs(&skip, (const char **)json_object_get_array(skip_dir_objs)->array, json_object_get_array(skip_dir_objs)->size);

	SCANNER_CONFIG config = (SCANNER_CONFIG){
		.file_path = NULL,
		.max_depth = -1,
		.scan_type = 0,
		.rules = json_object_get_string(rules_obj)};

	if (IS_ERR(init_scanner(scanner, config)))
	{
		fprintf(stderr, "Init_scanner_main : Error init scanner\n");
		exit(EXIT_FAILURE);
	}

	(*scanner)->config.skip = skip;
}

void process_command_line_options(int argc, char **argv, SCANNER **scanner)
{
	// Command-line options
	struct option long_options[] = {
		/* arguments */
		{"help", no_argument, 0, 'h'},
		{"scan", required_argument, 0, 's'},
		{"quick", no_argument, 0, 'q'},
		{"max-depth", required_argument, 0, 0},
		{"version", no_argument, 0, 'v'},

		/* null byte */
		{0, 0, 0, 0},
	};

	while (1)
	{
		int option_index = 0;
		const int c = getopt_long(argc, argv, ":qs:d:vh", long_options, &option_index);

		if (c < 0)
			break;

		switch (c)
		{
		case 0:
			if (!strcmp(long_options[option_index].name, "max-depth"))
			{
				uint32_t max_depth = (uint32_t)atoi(optarg);
				if (max_depth < 0)
					(*scanner)->config.max_depth = 0;
				(*scanner)->config.max_depth = max_depth;
			}
			break;

		case 'h':
			help(argv[0]);
			break;

		case 's':
			(*scanner)->config.file_path = optarg;
			break;

		case 'q':
			(*scanner)->config.scan_type |= QUICK_SCAN;
			break;

		case 'v':
			pr_version();
			exit(EXIT_SUCCESS);
			break;

		default:
			break;
		}
	}
}

void cleanup_resources(struct json_object **json_obj, LOGGER **logger, SCANNER **scanner)
{
	if (!IS_NULL_PTR(*json_obj))
		exit_json(json_obj);

	if (!IS_NULL_PTR(*logger))
		exit_logger(logger);

	if (!IS_NULL_PTR(*scanner) && !IS_NULL_PTR(*logger)) // exit_scanner depends this logger for LOG_ERROR
	{
		if (IS_ERR(exit_scanner(scanner)))
		{
			fprintf(stderr, "Cleanup_resources : Error in exit_scanner");
		}
	}
}

void help(char *prog_name)
{
	pr_version();
	printf("Usage: %s [OPTIONS]\n", prog_name);
	printf("\n\
 -h, --help                     This help menu\n\
 -s, --scan <file>|<folder>     Scans either a file or a folder (default max-depth X)\n\
 -q, --quick                    Enable quick scan\n\
 --max-depth <depth>            Sets max-depth on folder scan\n\
 -v, --version                  Version the Linux Defender\n\
");
	exit(EXIT_SUCCESS);
}

void pr_version()
{
	printf("LinuxDefender ( Moblog Security Researchers ) %d.%d.%d\n", LINUX_DEFENDER_VERSION_MAJOR, LINUX_DEFENDER_VERSION_PATCHLEVEL, LINUX_DEFENDER_VERSION_SUBLEVEL);
}