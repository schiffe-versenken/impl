#pragma once

#include <vector>
#include <array>
#include <random>

extern int N;
extern int D;
extern uint64_t BLOCK_SIZE;
extern int BLOCK_COUNT;
extern int BLOCK_DIMENSION_CUTOFF;
extern int BLOCK_DIMENSIONS;
extern u_int64_t SHIPS;
extern u_int64_t CELLS;
extern uint64_t DATA_SIZE;
extern double FLEETS;
extern std::mt19937 GENERATOR;
extern std::vector<uint64_t> DIMENSION_POWERS;

typedef std::vector<int> Coordinate;
using BlockCoordinate = std::vector<int>;

struct Ship
{
	Coordinate min;
	Coordinate max;
};

using StrategyBlock = std::vector<u_int64_t>;

void initValues(int n, int d, int b, int seed);

Coordinate emptyCoord();

BlockCoordinate emptyBlockCoord();

StrategyBlock* emptyStrategyBlock();
