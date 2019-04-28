#include <iostream>
#include <getopt.h>
#include <string.h>
#include "utils.h"
#include "strategies.h"
#include <atomic>

int main(int argc, char **argv)
{
	int c;
	int n;
	int d;
	int threads;
	int seed;
	int ds;
	uint64_t s;
	std::string stratname;

	while (1)
	{
		option long_options[] =
		{
		{ "size",  required_argument, 0, 'n' },
		{ "dimensions",  required_argument, 0, 'd' },
		{ "strategy",    required_argument, 0, 's' },
		{ "seed",    required_argument, 0, 'x' },
		{ "data-size",    required_argument, 0, 'b' },
		{ "ships-size",    required_argument, 0, 'c' },
		{ "threads",    required_argument, 0, 't' },
		{ 0, 0, 0, 0 }
		};
		/* getopt_long stores the option index here. */
		int option_index = 0;

		c = getopt_long(argc, argv, "n:d:s:x:b:c:t:",
			long_options, &option_index);

		/* Detect the end of the options. */
		if (c == -1)
			break;

		switch (c)
		{
		case 0:
			/* If this option set a flag, do nothing else now. */
			if (long_options[option_index].flag != 0)
				break;
			printf("option %s", long_options[option_index].name);
			if (optarg)
				printf(" with arg %s", optarg);
			printf("\n");
			break;

		case 'n':
			n = atoi(optarg);
			break;

		case 'd':
			d = atoi(optarg);
			break;

		case 'x':
			seed = atoi(optarg);
			break;

		case 'b':
			ds = atoi(optarg);
			break;

		case 'c':
			s = atol(optarg);
			break;

		case 's':
			stratname = optarg;
			break;

		case 't':
			threads = atoi(optarg);
			break;

		case '?':
			/* getopt_long already printed an error message. */
			break;

		default:
			abort();
		}
	}

	initValues(n, d, ds, s, seed);
	createStrategy(stratname);
	calcExpectedValueMT(threads);
	exit(0);
}
