#include "defs.h"
#include <cmath>

int N = 0;
int D = 0;
int SHIPS = 0;
int CELLS = 0;
std::vector<double> BINOMS;
double FLEETS = 0.0;

void initValues(int n, int d)
{
	N = n;
	D = d;

	SHIPS = std::pow((double) N * (N + 1), D);
	CELLS = std::pow((double) N, D);
	FLEETS = std::pow((double)2, (double)SHIPS) - 1;

	BINOMS.reserve(SHIPS + 1);
	auto it = BINOMS.begin();
	double value = 1;
	for (int i = 1; i <= SHIPS; ++i) {
		value = value * ((double)(SHIPS - i + 1) / (double)i);
		BINOMS.insert(it, value);
		++it;
	}
}

Coordinate emptyCoord()
{
	Coordinate c = Coordinate(D, 0);
	return c;
}

Fleet emptyFleet(int k)
{
	Fleet f = Fleet(k, Ship { Coordinate(D, 0) , Coordinate(D, 0) });
	return f;
}
