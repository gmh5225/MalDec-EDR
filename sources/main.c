#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <stdint.h>

#define MAJOR 0
#define MINOR 0
#define PATCH 1

void help(char *prog_name) __attribute__((__noreturn__));
void help(char *prog_name)
{
	printf("LinuxDefender %d.%d.%d\n", MAJOR, MINOR, PATCH);
	printf("Usage: %s [OPTIONS]\n", prog_name);
	printf("\n\
 -h, --help                     This help menu\n\
 -s, --scan <file>|<folder>     Scans either a file or a folder (default max-depth X)\n\
 --quick                        Enable quick scan\n\
 --max-depth <depth>            Sets max-depth on folder scan\n\
"
	);
	exit(1);	
}

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		help(argv[0]);
	}

	int c;
	int quick;

	struct option long_options[] ={
		{"help", no_argument, 0, 'h'},
		{"scan", required_argument, 0, 's'},
		{"quick", no_argument, &quick, 1},
		{"max-depth", required_argument, 0, 0},
		// FIXME: Not sure about this option yet,
		// {"full", no_argument, 0, 0},
		{0, 0, 0, 0},
	};

	while (1)
	{
		int option_index = 0;
		c = getopt_long(argc, argv, "qs:d:",
						long_options, &option_index);
		
		if (c < 0) break;

		switch (c)
		{
		case 0:
			if (!strcmp(long_options[option_index].name, "max-depth"))
			{
				printf("--max-depth %d\n", atoi(optarg));
			}
			break;
		
		case 'h':
			help(argv[0]);
			break;

		case 's':
			printf("--scan %s\n", optarg);
			break;
		
		default:
			break;
		}
	}

	return 0;
}