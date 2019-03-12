#include <iostream>
#include <getopt.h>
#include <string.h>
#include "utils.h"
#include "strategies.h"

int main(int argc, char **argv)
{
	int c;
	int n;
	int d;
	const char* stratname = nullptr;

	while (1)
	{
		option long_options[] =
		{
		{ "size",  required_argument, 0, 'n' },
		{ "dimensions",  required_argument, 0, 'd' },
		{ "strategy",    required_argument, 0, 's' },
		{ 0, 0, 0, 0 }
		};
		/* getopt_long stores the option index here. */
		int option_index = 0;

		c = getopt_long(argc, argv, "n:d:s:",
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

		case 's':
			stratname = optarg;
			break;

		case '?':
			/* getopt_long already printed an error message. */
			break;

		default:
			abort();
		}
	}

	initValues(n, d);

	Strategy* strat = nullptr;
	if (strcmp(stratname, "test") == 0)
	{
		strat = testStrategy();
	}
	if (strcmp(stratname, "random") == 0)
	{
		strat = randomStrategy();
	}

	double e = calcExpectedValue(*strat);
	std::cout << e << '\n';

	exit(0);
}
