#include "defs.h"
#include <cmath>

//size of game field if seen as a D dimensional cube
int N = 0;
//dimension
int D = 0;
//total number of ships
u_int64_t SHIPS = 0;
//number of cells
u_int64_t CELLS = 0;

//dimension of Blocks
//Remark: the cube gets divided in blocks.
int BLOCK_DIMENSIONS = 0;
//Difference between D and Block_Dimension.
int BLOCK_DIMENSION_CUTOFF = 0;
//Number of blocks.
int BLOCK_COUNT = 0;
//Size of blocks.
uint64_t BLOCK_SIZE = 0;

//used for output calculations.
uint64_t DATA_SIZE = 0;
uint64_t SHIPS_SIZE = 0;
uint64_t OUTPUT_SIZE;

#ifdef DEBUG
thread_local std::mt19937_64 GENERATOR;
#else
std::mt19937_64 GENERATOR;
#endif

std::vector<uint64_t> DIMENSION_POWERS;

//Used for grids. Determines N <= 2^k = CAP, the next higher power of 2.
int CAP = 0;

//For saving the coordinates of all the increment levels.
std::vector<int> GRID_COORDINATES;

//To save the current number of shots to every layer (which represents the levels) per thread for full Grids.
thread_local std::vector<u_int64_t> LEVEL_SHOTS_FULL(100, 1);

//the maximum number of shots per layer for full Grids.
std::vector<u_int64_t> MAX_LEVEL_SHOTS_FULL;

//same as before only for sparse Grids.
thread_local std::vector<u_int64_t> LEVEL_SHOTS_SPARSE(100, 1);

//same as before only for sparse Grids.
std::vector<u_int64_t> MAX_LEVEL_SHOTS_SPARSE;

//to save some prime numbers for Halton.
std::vector<int> PRIMES;

//to save the cells that were shot.
thread_local std::vector<bool> haltonHits(BLOCK_SIZE, false);

//degrees of polynoms for Sobol.
std::vector<unsigned> GRADSOBOL = {1,2,3,3,4,4,5,5,5,5,5,5,6,6};

//Coefficient of polynoms for Sobol.
std::vector<unsigned> COEFFSOBOL = { 0,1,1,2,1,4,2,4,7,11,13,14,1,13 };

//Coefficient m_i for Sobol.
std::vector<unsigned> INITVALSOBOL = { 1,1,3,1,3,1,1,1,1,1,1,3,3,1,3,5,13,1,1,5,5,17,1,1,5,5,5,1,1,7,11,19,1,1,5,1,1,1,1,1,3,11,1,3,5,5,31,1,3,3,9,7,49,1,1,1,15,21,21 };

//number of fleets.
double FLEETS = 0.0;

//Helper to calculate binomial coefficients.
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

//to initiate values
void initValues(int n, int d, int ds, int s, int seed)
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

	//used for generating random numbers.
	GENERATOR = std::mt19937_64(seed);

	DIMENSION_POWERS = std::vector<uint64_t>(D);
	for (int i = 0; i < d; i++)
	{
		DIMENSION_POWERS[i] = std::pow((double)N, i);
	}

	//Helpers for following calculations.
	int upper = std::ceil(std::log2((double)N));
	int lower = std::floor(std::log2((double)N));

	CAP = (int)(pow((double)2, upper)) - 1;

	//to account for redundancy in the last layer (level) which only occurs
	//when N < 2^k
	int lastLevelRedundancy = std::pow((double)2, lower) - (CAP - N);

	//to fill the coordinates in the respective layers/levels
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

	//Calculating maximum level shots full Grid
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
	/* Calculating maxum level shots sparse Grid for those layers that are used
	in the cube completly. There are layers of higher level from which only a few 
	increments are used, but not all. So that we have to calculate them seperatly in
	Loop A.
	*/
	for (int i = 1; i < lower; i++) {
		MAX_LEVEL_SHOTS_SPARSE[i] = pow((double)2, i) * binomialCoefficient(D - 1 + i, D - 1);
	}

	std::vector<int> counters(lower + 1, 0);
	for (int i = 1; i <= lower; i++) {
		counters[i] = i;
	}
	Coordinate worker(D, 0);
	//Loop A
	for (int i = 0; i < (int)pow((double)lower + 1, D); i++) {
		int sumLevel = 0;
		for (int k = 0; k < D; k++) {
			sumLevel += worker[k];
		}
		if (sumLevel >= lower) {
			//To account for redundancy on the last level if N is not multiple of 2^k
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

	//Initializing some Primes for Halton. They are hard coded, because it is not expected to reach
	//higher dimensions, than those numbers not already give.
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
