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
uint64_t SHIPS_SIZE = 0;

std::mt19937 GENERATOR;

std::vector<uint64_t> DIMENSION_POWERS;

std::vector<int> GRID_COORDINATES;
std::vector<u_int64_t> LEVEL_SHOTS_FULL;
std::vector<u_int64_t> MAX_LEVEL_SHOTS_FULL;

double FLEETS = 0.0;

void initValues(int n, int d, int ds, int s, int seed)
{
	N = n;
	D = d;

	SHIPS = std::pow((double) N * (N + 1), D);
	CELLS = std::pow((double) N, D);
	FLEETS = std::pow((double)2, (double)SHIPS) - 1;

	BLOCK_DIMENSION_CUTOFF = D - ds;
	BLOCK_DIMENSIONS = ds;
	BLOCK_SIZE = std::pow((double)N, ds);
	BLOCK_COUNT = CELLS / BLOCK_SIZE;

	DATA_SIZE = BLOCK_SIZE;
	SHIPS_SIZE = std::pow((double)N, s);

	GENERATOR = std::mt19937(seed);

	DIMENSION_POWERS = std::vector<uint64_t>(D);
	for (int i = 0; i < d; i++)
	{
		DIMENSION_POWERS[i] = std::pow((double)N, i);
	}

	GRID_COORDINATES = std::vector<int>(N);
	GRID_COORDINATES[0] = std::floor((double)N / 2.0);
	for (int i = 1; i <= std::ceil(std::log2((double)N)); i++) {
		int power = (int)pow(2.0, i-1);
		int offset = std::ceil((double)GRID_COORDINATES[power - 1] / 2.0);
		int next = std::pow(2.0, i) - 1;
		for (int j = 0; j < next - power +1; j++) {
			int k = GRID_COORDINATES[power + j - 1];
			GRID_COORDINATES[next + 2 * j] = k - offset;
			GRID_COORDINATES[next + 2 * j + 1] = k + offset;
		}
	}

	LEVEL_SHOTS_FULL = std::vector<u_int64_t>(std::ceil(std::log2((double)N)), 0);
	MAX_LEVEL_SHOTS_FULL = std::vector<u_int64_t>(std::ceil(std::log2((double)N)), 1);
	std::vector<u_int64_t> numberGridPoints(std::ceil(std::log2((double)N)), 1);
	int coordinatePoints = 1;
	for (int i = 1 ; i < std::ceil(std::log2((double)N)); i++) {
		coordinatePoints += std::pow((double)2, i);
		numberGridPoints[i] = std::pow((double)coordinatePoints, N);
		MAX_LEVEL_SHOTS_FULL[i] = numberGridPoints[i] - numberGridPoints[i - 1];
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
