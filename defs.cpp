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

int CAP = 0;
std::vector<int> GRID_COORDINATES;
thread_local std::vector<u_int64_t> LEVEL_SHOTS_FULL(100, 1);
std::vector<u_int64_t> MAX_LEVEL_SHOTS_FULL;
thread_local std::vector<u_int64_t> LEVEL_SHOTS_SPARSE(100, 1);
std::vector<u_int64_t> MAX_LEVEL_SHOTS_SPARSE;

double FLEETS = 0.0;

u_int64_t binomialCoefficient(int n, int k)
{
	u_int64_t coeffient = 1;
	if (k > n - k) {
		k = n - k;
	}
	for (int i = 0; i < k; ++i) {
		coeffient *= (n - i);
		coeffient /= (i + 1);
	}
	return coeffient;
}

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

	int upper = std::ceil(std::log2((double)N));
	int lower = std::floor(std::log2((double)N));
	CAP = (int)(pow((double)2, upper)) - 1;
	GRID_COORDINATES = std::vector<int>(CAP);

	GRID_COORDINATES[0] = std::ceil((double)N / 2.0);
	for (int i = 1; i < upper; i++) {
		int power = (int)pow(2.0, i-1);
		int offset = std::ceil((double)GRID_COORDINATES[power - 1] / 2.0);
		int next = std::pow(2.0, i) - 1;
		for (int j = 0; j < next - power +1; j++) {
			int k = GRID_COORDINATES[power + j - 1];
			if (k - offset != 0) {
				GRID_COORDINATES[next + 2 * j] = k - offset;
			}
			else {
				GRID_COORDINATES[next + 2 * j] = 1;
			}
			if (k + offset > N) {
				GRID_COORDINATES[next + 2 * j + 1] = N;
			}
			else {
				GRID_COORDINATES[next + 2 * j + 1] = k + offset;
			}
		}
	}

	LEVEL_SHOTS_FULL = std::vector<u_int64_t>(upper , 1);
	MAX_LEVEL_SHOTS_FULL = std::vector<u_int64_t>(upper , 1);
	std::vector<u_int64_t> numberGridPoints(upper , 1);
	int coordinatePoints = 1;
	for (int i = 1 ; i < upper; i++) {
		coordinatePoints += std::pow((double)2, i);
		numberGridPoints[i] = std::pow((double)coordinatePoints, D);
		MAX_LEVEL_SHOTS_FULL[i] = numberGridPoints[i] - numberGridPoints[i - 1];
	}

	LEVEL_SHOTS_SPARSE = std::vector<u_int64_t>(lower * D + 1, 1);
	MAX_LEVEL_SHOTS_SPARSE = std::vector<u_int64_t>(lower * D + 1, 0);
	MAX_LEVEL_SHOTS_SPARSE[0] = 1;
	for (int i = 1; i < upper; i++) {
		MAX_LEVEL_SHOTS_SPARSE[i] = pow((double)2, i) * binomialCoefficient(D - 1 + i, D - 1);
	}
	
	std::vector<int> counters(lower + 1, 0);
	for (int i = 1; i <= lower; i++) {
		counters[i] = i;
	}
	Coordinate worker(D, 0);
	for (int i = 0; i < (int)pow((double)lower + 1, D); i++) {
		for (int j = D - 1; j >= 0; j--) {
			if (worker[j] == lower) {
				int sumLevel = 0;
				for (int k = 0; k < D; k++) {
					sumLevel += worker[k];
				}
				if (sumLevel > lower) {
					MAX_LEVEL_SHOTS_SPARSE[sumLevel] += pow((double)2, sumLevel);
				}
				break;
			}
		}
		for (int j = D - 1; j >= 0; j--) {
			if (worker[j] == lower) {
				worker[j] = 0;
			}
			else {
				worker[j] = counters[worker[j] + 1];
				break;
			}
		}
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
