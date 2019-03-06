#include "utils.h"


std::vector<double> createBinoms()
{
	std::vector<double> values(SHIPS + 1);
	values[0] = 1;
	double last = 1;

	for (int i = 1; i <= SHIPS; ++i) {
		double next = last / (double) i * (double) (SHIPS - i + 1);
		last = next;
		values[i] = next;
	}
	return values;
}

std::vector<double> BINOMS = createBinoms();