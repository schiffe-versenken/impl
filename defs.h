#pragma once

#include <vector>
#include <array>
#include <random>

extern int N;
extern int D;
extern int BLOCK_SIZE;
extern int BLOCK_COUNT;
extern int BLOCK_DIMENSION;
extern int SHIPS;
extern int CELLS;
extern int DATA_SIZE;
extern double FLEETS;
extern std::mt19937 GENERATOR;

typedef std::vector<int> Coordinate;
using BlockCoordinate = std::vector<int>;

struct Ship
{
	Coordinate min;
	Coordinate max;
};

using StrategyBlock = std::vector<int>;

void initValues(int n, int d, int b, int seed);

Coordinate emptyCoord();

BlockCoordinate emptyBlockCoord();

StrategyBlock* emptyStrategyBlock();
