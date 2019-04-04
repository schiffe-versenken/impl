#include "defs.h"
#include <cmath>

int N = 0;
int D = 0;
u_int64_t SHIPS = 0;
u_int64_t CELLS = 0;

int BLOCK_DIMENSIONS = 0;
int BLOCK_DIMENSION_CUTOFF = 0;
int BLOCK_COUNT = 0;
uint64_t BLOCK_SIZE = 0;

uint64_t DATA_SIZE = 0;

std::mt19937 GENERATOR;

std::vector<uint64_t> DIMENSION_POWERS;

double FLEETS = 0.0;

void initValues(int n, int d, int b, int seed)
{
	N = n;
	D = d;

	SHIPS = std::pow((double) N * (N + 1), D);
	CELLS = std::pow((double) N, D);
	FLEETS = std::pow((double)2, (double)SHIPS) - 1;

	BLOCK_DIMENSION_CUTOFF = b;
	BLOCK_DIMENSIONS = D - BLOCK_DIMENSION_CUTOFF;
	BLOCK_SIZE = std::pow((double)N, D - BLOCK_DIMENSION_CUTOFF);
	BLOCK_COUNT = CELLS / BLOCK_SIZE;

	DATA_SIZE = BLOCK_SIZE;

	GENERATOR = std::mt19937(seed);

	DIMENSION_POWERS = std::vector<uint64_t>(D);
	for (int i = 0; i < d; i++)
	{
		DIMENSION_POWERS[i] = std::pow((double)N, i);
	}
}

Coordinate emptyCoord()
{
	Coordinate c = Coordinate(D, 0);
	return c;
}

BlockCoordinate emptyBlockCoord()
{
	BlockCoordinate c = BlockCoordinate(BLOCK_DIMENSION_CUTOFF, 0);
	return c;
}

StrategyBlock* emptyStrategyBlock()
{
	return new StrategyBlock(BLOCK_SIZE);
}
