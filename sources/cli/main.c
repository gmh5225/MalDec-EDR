#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <stdint.h>

#include "version/version.h"
#include "err/err.h"
#include "compiler/compiler_attribute.h"
#include "config.h"

#define CONFIG_JSON_PATH "../../../config/appsettings.json"

inline void init_logger_main();
inline void init_scanner_main();
void process_command_line_options(int argc, char **argv);
inline void cleanup_resources();
inline void help(char *prog_name) no_return;
inline void pr_version() no_return;

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		help(argv[0]);
	}

	int retval = SUCCESS;
	DEFENDER.logger = NULL;
	DEFENDER.scanner = NULL;
	DEFENDER.config_json = NULL;

	atexit(cleanup_resources);

	if (IS_ERR(init_json(&DEFENDER.config_json, CONFIG_JSON_PATH)))
	{
		fprintf(stderr, "Main : Error in parser json config '%s'\n", CONFIG_JSON_PATH);
		retval = ERROR;
		goto ret;
	}
	else
	{
		init_logger_main();
		init_scanner_main(DEFENDER.scanner, DEFENDER.config_json);
		process_command_line_options(argc, argv);
	}

ret:
	return retval;
}

void init_logger_main()
{
	struct json_object *logger_obj,
		*filename_obj,
		*max_file_size_obj,
		*max_backup_files_obj,
		*level_obj,
		*console_obj;

	if (json_object_object_get_ex(DEFENDER.config_json, "logger", &logger_obj))
	{
		if (!json_object_object_get_ex(logger_obj, "filename", &filename_obj) ||
			!json_object_object_get_ex(logger_obj, "max_file_size", &max_file_size_obj) ||
			!json_object_object_get_ex(logger_obj, "max_backup_files", &max_backup_files_obj) ||
			!json_object_object_get_ex(logger_obj, "level", &level_obj) ||
			!json_object_object_get_ex(logger_obj, "console", &console_obj))
		{
			fprintf(stderr, "Init_logger_main : Unable to retrieve logger configuration from JSON\n");
			exit(ERROR);
		}
	}
	else
	{
		fprintf(stderr, "Init_logger_main : Unable to retrieve 'logger' object from JSON\n");
		exit(ERROR);
	}

	LOGGER_CONFIG logger_config = (LOGGER_CONFIG){
		.filename = json_object_get_string(filename_obj),
		.level = json_object_get_int(level_obj),
		.max_backup_files = json_object_get_int(max_backup_files_obj),
		.max_file_size = json_object_get_int(max_file_size_obj),
		.console = json_object_get_boolean(console_obj)};

	if (IS_ERR(init_logger(&DEFENDER.logger, logger_config)))
	{
		fprintf(stderr, "Init_logger_main : Error init logger\n");
		exit(ERROR);
	}
}

void init_scanner_main()
{
	struct json_object *scan_obj, *rules_obj, *skip_dir_objs;
	if (json_object_object_get_ex(DEFENDER.config_json, "scan", &scan_obj))
	{
		if (!json_object_object_get_ex(scan_obj, "rules", &rules_obj) ||
			!json_object_object_get_ex(scan_obj, "skip_dirs", &skip_dir_objs))
		{
			fprintf(stderr, "Init_scanner_main : Unable to retrieve scan configuration from JSON\n");
			exit(ERROR);
		}
	}

	struct skip_dirs *skip = NULL;
	add_skip_dirs(&skip, (const char **)json_object_get_array(skip_dir_objs)->array, json_object_get_array(skip_dir_objs)->size);

	SCANNER_CONFIG config = (SCANNER_CONFIG){
		.file_path = NULL,
		.max_depth = -1,
		.scan_type = 0,
		.verbose = false,
		.rules = json_object_get_string(rules_obj)};

	if (IS_ERR(init_scanner(&DEFENDER.scanner, config)))
	{
		fprintf(stderr, "Init_scanner_main : Error init scanner\n");
		exit(ERROR);
	}

	DEFENDER.scanner->config.skip = skip;
}

void process_command_line_options(int argc, char **argv)
{
	// Command-line options
	struct option long_options[] = {
		/* arguments */
		{"help", no_argument, 0, 'h'},
		{"scan", required_argument, 0, 's'},
		{"quick", no_argument, 0, 'q'},
		{"max-depth", required_argument, 0, 0},
		{"version", no_argument, 0, 'v'},
		{"verbose", no_argument, 0, 0},

		/* null byte */
		{0, 0, 0, 0},
	};

	int option_index;
	while (1)
	{
		option_index = 0;
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
					DEFENDER.scanner->config.max_depth = 0;
				DEFENDER.scanner->config.max_depth = max_depth;
			}
			if (!strcmp(long_options[option_index].name, "verbose"))
				DEFENDER.scanner->config.verbose = true;

			break;

		case 'h':
			help(argv[0]);
			break;

		case 's':
			DEFENDER.scanner->config.file_path = optarg;
			break;

		case 'q':
			DEFENDER.scanner->config.scan_type |= QUICK_SCAN;
			break;

		case 'v':
			pr_version();
			break;

		default:
			break;
		}
	}

	if (!IS_NULL_PTR(DEFENDER.scanner))
	{
		if (IS_ERR(scan(DEFENDER.scanner)))
			;
	}
}

void cleanup_resources()
{
	if (!IS_NULL_PTR(DEFENDER.config_json))
		exit_json(&DEFENDER.config_json);

	if (!IS_NULL_PTR(DEFENDER.logger))
		exit_logger(&DEFENDER.logger);

	if (!IS_NULL_PTR(DEFENDER.scanner))
	{
		if (IS_ERR(exit_scanner(&DEFENDER.scanner)))
			;
	}
}

void help(char *prog_name)
{
	puts("LinuxDefender : Anti0Day");
	printf("Usage: %s [OPTIONS]\n", prog_name);
	printf("\n\
 -h, --help                     This help menu\n\
 -s, --scan <file>|<folder>     Scans either a file or a folder (default max-depth X)\n\
 -q, --quick                    Enable quick scan\n\
 --max-depth <depth>            Sets max-depth on folder scan\n\
 --verbose			Verbose for scan\n\
 -v, --version                  Version the Linux Defender\n\
");
	exit(SUCCESS);
}

void pr_version()
{
	printf("LinuxDefender ( Moblog Security Researchers ) %d.%d.%d\n", LINUX_DEFENDER_VERSION_MAJOR, LINUX_DEFENDER_VERSION_PATCHLEVEL, LINUX_DEFENDER_VERSION_SUBLEVEL);
	exit(SUCCESS);
}