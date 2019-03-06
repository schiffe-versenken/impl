#include "utils.h"


std::vector<float> createBinoms()
{
	std::vector<float> values{ SHIPS + 1 };
	values[0] = 1;
	float last = 1;

	for (int i = 1; i < SHIPS; ++i) {
		float next = last * (1 / i) * (SHIPS - i + 1);
		last = next;
		values[i] = next;
	}
	return values;
}

std::vector<float> BINOMS = createBinoms();