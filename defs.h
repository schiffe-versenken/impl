#pragma once

#include <vector>
#include <array>
#include <random>

#ifdef DEBUG
#define AT(v, i) v.at(i)
#else
#define AT(v, i) v[i]
#endif

extern int N;
extern int D;
extern uint64_t BLOCK_SIZE;
extern uint64_t BLOCK_COUNT;
extern int BLOCK_DIMENSION_CUTOFF;
extern int BLOCK_DIMENSIONS;
extern u_int64_t SHIPS;
extern u_int64_t CELLS;
extern uint64_t DATA_SIZE;
extern uint64_t OUTPUT_SIZE;
extern uint64_t SHIPS_SIZE;
extern double FLEETS;

#ifdef DEBUG
extern thread_local std::mt19937_64 GENERATOR;
#else
extern std::mt19937_64 GENERATOR;
#endif

extern std::vector<uint64_t> DIMENSION_POWERS;

extern int CAP;
extern std::vector<int> GRID_COORDINATES;
extern thread_local std::vector<u_int64_t> LEVEL_SHOTS_FULL;
extern std::vector<u_int64_t> MAX_LEVEL_SHOTS_FULL;
extern thread_local std::vector<u_int64_t> LEVEL_SHOTS_SPARSE;
extern std::vector<u_int64_t> MAX_LEVEL_SHOTS_SPARSE;
extern std::vector<int> PRIMES;
extern thread_local std::vector<bool> haltonHits;

typedef std::vector<int> Coordinate;
using BlockCoordinate = std::vector<int>;

struct Ship
{
	Coordinate min;
	Coordinate max;
};

using StrategyBlock = std::vector<u_int64_t>;

void initValues(uint64_t n, uint64_t d, uint64_t ds, uint64_t s, int seed);

Coordinate emptyCoord();

BlockCoordinate emptyBlockCoord();

StrategyBlock* emptyStrategyBlock();

u_int64_t binomialCoefficient(int n, int k);
