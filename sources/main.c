#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

int  main(int argc, char **argv)
{
	int c;

	static struct option long_options[] ={
		{"scan", required_argument, 0, 's'},
		{"max-depth", required_argument, 0, 'd'},
		{"quick", no_argument, 0, 'q'},
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
			break;
		
		case 'q':
			printf("quick scan\n");
			break;

		case 's':
			printf("--scan %s\n", optarg);
			break;
		
		case 'd':
			printf("--max-depth %d\n", atoi(optarg));
			break;

		
		default:
			break;
		}
	}

	return 0;
}
