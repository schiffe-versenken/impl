#include "utils.h"
#include <random>

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> dis(0, N - 1);

double FLEETS = std::pow((double)2, (double) SHIPS) - 1;

int genRandomCoordinate()
{
	return dis(gen);
}

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