#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <stdint.h>

#include "inotify/inotify.h"
#include "scan/config.h"
#include "scan/scan.h"
#include "version/version.h"
#include "err/err.h"
#include "compiler/compiler_attribute.h"

void help(char *prog_name) no_return;

void help(char *prog_name)
{
	printf("LinuxDefender %d.%d.%d\n", MAJOR, MINOR, PATCH);
	printf("Usage: %s [OPTIONS]\n", prog_name);
	printf("\n\
 -h, --help                     This help menu\n\
 -s, --scan <file>|<folder>     Scans either a file or a folder (default max-depth X)\n\
 --quick                        Enable quick scan\n\
 --max-depth <depth>            Sets max-depth on folder scan\n\
");
	exit(1);
}

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		help(argv[0]);
	}

	init_inotify();

	int c, retval;

	SCANNER *scanner;
	SCANNER_CONFIG config = (SCANNER_CONFIG){
		.file_path = NULL,
		.max_depth = -1,
		.scan_type = 0,
		.skip = NULL,
	};

	if ((retval = scanner_init(&scanner, config)) == ERROR)
	{
		goto ret;
	}

	struct option long_options[] = {
		{"help", no_argument, 0, 'h'},
		{"scan", required_argument, 0, 's'},
		{"quick", no_argument, 0, 'q'},
		{"max-depth", required_argument, 0, 0},
		// FIXME: Not sure about this option yet,
		// {"full", no_argument, 0, 0},'
		{0, 0, 0, 0},
	};

	while (1)
	{
		int option_index = 0;
		c = getopt_long(argc, argv, "qs:d:",
						long_options, &option_index);

		if (c < 0)
			break;

		switch (c)
		{
		case 0:
			if (!strcmp(long_options[option_index].name, "max-depth"))
			{
				uint32_t max_depth = (uint32_t)atoi(optarg);
				if (max_depth < 0)
					scanner->config.max_depth = 0;
				scanner->config.max_depth = max_depth;
			}
			break;

		case 'h':
			help(argv[0]);
			break;

		case 's':
			scanner->config.file_path = strdup(optarg);
			break;

		case 'q':
			scanner->config.scan_type |= QUICK_SCAN;
			break;

		default:
			break;
		}
	}

	retval = scan(scanner);
	retval = scanner_destroy(&scanner);

ret:
	return retval;
}