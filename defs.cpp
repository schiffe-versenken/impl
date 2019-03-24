#include "defs.h"
#include <cmath>

int N = 0;
int D = 0;
int SHIPS = 0;
int CELLS = 0;

int BLOCK_DIMENSION = 0;
int BLOCK_COUNT = 0;
int BLOCK_SIZE = 0;

int DATA_SIZE = 0;

std::mt19937 GENERATOR;

double FLEETS = 0.0;

void initValues(int n, int d, int b, int seed)
{
	N = n;
	D = d;

	SHIPS = std::pow((double) N * (N + 1), D);
	CELLS = std::pow((double) N, D);
	FLEETS = std::pow((double)2, (double)SHIPS) - 1;

	BLOCK_DIMENSION = b;
	BLOCK_SIZE = std::pow((double)N, D - BLOCK_DIMENSION);
	BLOCK_COUNT = CELLS / BLOCK_SIZE;

	DATA_SIZE = sqrt(CELLS);

	GENERATOR = std::mt19937(seed);
}

Coordinate emptyCoord()
{
	Coordinate c = Coordinate(D, 0);
	return c;
}

BlockCoordinate emptyBlockCoord()
{
	BlockCoordinate c = BlockCoordinate(BLOCK_DIMENSION, 0);
	return c;
}

StrategyBlock* emptyStrategyBlock()
{
	return new StrategyBlock(BLOCK_SIZE);
}
