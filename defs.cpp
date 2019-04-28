#include "defs.h"
#include <cmath>

int N = 0;
int D = 0;
u_int64_t SHIPS = 0;
u_int64_t CELLS = 0;

int BLOCK_DIMENSIONS = 0;
int BLOCK_DIMENSION_CUTOFF = 0;
uint64_t BLOCK_COUNT = 0;
uint64_t BLOCK_SIZE = 0;

uint64_t DATA_SIZE = 0;
uint64_t SHIPS_SIZE = 0;
uint64_t OUTPUT_SIZE;

#ifdef DEBUG
thread_local std::mt19937_64 GENERATOR;
#else
std::mt19937_64 GENERATOR;
#endif

std::vector<uint64_t> DIMENSION_POWERS;

int CAP = 0;
std::vector<int> GRID_COORDINATES;
thread_local std::vector<u_int64_t> LEVEL_SHOTS_FULL(100, 1);
std::vector<u_int64_t> MAX_LEVEL_SHOTS_FULL;
thread_local std::vector<u_int64_t> LEVEL_SHOTS_SPARSE(100, 1);
std::vector<u_int64_t> MAX_LEVEL_SHOTS_SPARSE;
std::vector<int> PRIMES;
thread_local std::vector<bool> haltonHits(BLOCK_SIZE, false);

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

void initValues(uint64_t n, uint64_t d, uint64_t ds, uint64_t s, int seed)
{
	N = n;
	D = d;

	SHIPS = std::pow((double) (N * (N + 1)) / 2, D);
	CELLS = std::pow((double) N, D);
	FLEETS = std::pow((double)2, (double)SHIPS) - 1;

	BLOCK_DIMENSION_CUTOFF = D - ds;
	BLOCK_DIMENSIONS = ds;
	BLOCK_SIZE = std::pow((double)N, ds);
	BLOCK_COUNT = CELLS / BLOCK_SIZE;

	DATA_SIZE = BLOCK_SIZE;
	OUTPUT_SIZE = std::min(DATA_SIZE, (uint64_t) 1000000);
	SHIPS_SIZE = std::min<uint64_t>(SHIPS, s);

	GENERATOR = std::mt19937_64(seed);

	DIMENSION_POWERS = std::vector<uint64_t>(D);
	for (int i = 0; i < d; i++)
	{
		DIMENSION_POWERS[i] = std::pow((double)N, i);
	}

	int upper = std::ceil(std::log2((double)N));
	int lower = std::floor(std::log2((double)N));
	CAP = (int)(pow((double)2, upper)) - 1;
	int lastLevelRedundancy = std::pow((double)2, lower) - (CAP - N);
	GRID_COORDINATES = std::vector<int>(CAP);

	GRID_COORDINATES[0] = std::ceil((double)N / 2.0);
	for (int i = 1; i < upper; i++) {
		int power = (int)pow(2.0, i - 1);
		int offset = std::ceil((double)GRID_COORDINATES[power - 1] / 2.0);
		int next = std::pow(2.0, i) - 1;
		for (int j = 0; j < next - power + 1; j++) {
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

	LEVEL_SHOTS_FULL = std::vector<u_int64_t>(upper, 1);
	MAX_LEVEL_SHOTS_FULL = std::vector<u_int64_t>(upper, 1);
	std::vector<u_int64_t> numberGridPoints(upper, 1);
	int coordinatePoints = 1;
	for (int i = 1; i < upper; i++) {
		coordinatePoints += std::pow((double)2, i);
		numberGridPoints[i] = std::pow((double)coordinatePoints, D);
		MAX_LEVEL_SHOTS_FULL[i] = numberGridPoints[i] - numberGridPoints[i - 1];
	}

	LEVEL_SHOTS_SPARSE = std::vector<u_int64_t>(lower * D + 1, 1);
	MAX_LEVEL_SHOTS_SPARSE = std::vector<u_int64_t>(lower * D + 1, 0);
	MAX_LEVEL_SHOTS_SPARSE[0] = 1;
	for (int i = 1; i < lower; i++) {
		MAX_LEVEL_SHOTS_SPARSE[i] = pow((double)2, i) * binomialCoefficient(D - 1 + i, D - 1);
	}

	std::vector<int> counters(lower + 1, 0);
	for (int i = 1; i <= lower; i++) {
		counters[i] = i;
	}
	Coordinate worker(D, 0);
	for (int i = 0; i < (int)pow((double)lower + 1, D); i++) {
		int sumLevel = 0;
		for (int k = 0; k < D; k++) {
			sumLevel += worker[k];
		}
		if (sumLevel >= lower) {
			//To account for redundancy on the last level if N is not multiple of (16*2^k)-1
			int addition = 1;
			for (int j = 0; j < D; j++) {
				if (worker[j] == counters[lower]) {
					addition = addition * lastLevelRedundancy;
				}
				else {
					addition = addition * std::pow((double)2, worker[j]);
				}
			}
			MAX_LEVEL_SHOTS_SPARSE[sumLevel] += addition;

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

	PRIMES = {2,3,5,7,11,13,17,19,23,29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101, 
		103, 107, 109, 113, 127, 131, 137, 139, 149, 151, 157, 163, 167, 173, 179, 181, 191, 193, 197, 199 };
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
