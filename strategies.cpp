#include "strategies.h"
#include <random>
#include <map>
#include <functional>
#include <cassert>

std::function<void(StrategyBlock*, BlockCoordinate)> generator;
//Realtive Index of sobol shot sequence
thread_local unsigned SHOTSSOBOL = 0;
//Last shot done by last pack of shots
thread_local Coordinate lastPackShot = emptyCoord();
//Index of first 0 of last number from last pack
thread_local unsigned lastPackZero = 1;

//Generates an index from a coordinate
uint64_t toIndex(Coordinate& c)
{
	uint64_t index = 0;
	for (int d = D - BLOCK_DIMENSION_CUTOFF - 1; d >= 0; --d) {
		uint64_t offset = c[(D - 1) - d] * DIMENSION_POWERS[d];
		index += offset;
	}
	return index;
}

//One dimensional halton, returns the i-th halton number, when i is given
// by index.
int halton(int base, double index) {
	double coeficient = 1.0;
	double result = 0.0;
	while (index > 0) {
		coeficient = coeficient / base;
		result += coeficient * ((int)index % base);
		index = std::floor(index / base);
	}
	return (int)std::floor(result * N);
}

//Computes a pack of sobol coordinates. The pack has the size 1000.
//Uses SHOTSOBOL to get the relaitve current index. So that we can
//traverse a big field of Coordinates in steps of 1000 shots.
std::vector<Coordinate> sobolPacks(int numb) {
	//Bits of the reliatve index
	unsigned L = std::ceil(std::log2(SHOTSSOBOL + numb));
	
	//direction numbers as used in the generation of sobol numbers.
	std::vector<unsigned> directionNumbers(L + 1, 1);
	
	//describes the index of the first occuring 0 in the
	// index which is in binary
	std::vector<unsigned> firstZero(numb, 1);

	//computing index of first 0
	for (unsigned i = SHOTSSOBOL; i < SHOTSSOBOL + numb; i++) {
		if (i == 0) continue;
		firstZero[i - SHOTSSOBOL] = 1;
		unsigned value = i;
		while (value & 1) {
			value >>= 1;
			firstZero[i - SHOTSSOBOL]++;
		}
	}
	//saving last index for potential next pack.
	lastPackZero = firstZero[numb-1];

	//Initializing pack
	std::vector<Coordinate> pack(numb);

	//Computing direction numbers of first dimension
	unsigned *directionNumber = new unsigned[L + 1];
	for (unsigned i = 1; i <= L; i++) {
		directionNumber[i] = 1 << (32 - i); 
	}

	//Computing the first dimension coordinate of every point in the pack.
	unsigned *currentCoord = new unsigned[numb];
	currentCoord[0] = 0;
	pack[0] = emptyCoord();
	for (unsigned i = 0; i < numb; i++) {
		pack[i] = emptyCoord();
		if (i == 0) {
			if (SHOTSSOBOL == 0) {
				//Nothing to do for the absolute first point
				continue;
			}
			else {
				//this is the first point of a new pack, which needs computing
				currentCoord[i] = lastPackShot[0] ^ directionNumber[lastPackZero];
			}
		}
		else {
			//Every other point that is not the first of a pack or the absolute first
			currentCoord[i] = currentCoord[i - 1] ^ directionNumber[firstZero[i - 1]];
		}
		pack[i][0] = floor((double)currentCoord[i] / pow(2.0, 32) * N);
	}
	delete[] directionNumber;delete[] currentCoord;

	//Computing the remaining dimensions/coordinates of the points
	for (unsigned j = 1; j <= D - 1; j++) {
		//getting the values of s, a, and the m_i's,which are hardcoded
		unsigned s = GRADSOBOL[j - 1];
		unsigned a = COEFFSOBOL[j - 1];
		std::vector<unsigned> m(s + 1);
		unsigned temp = 0;
		for (unsigned i = 0; i < j - 1; i++) {
			temp += GRADSOBOL[i];
		}
		for (unsigned i = 1; i <= s; i++) {
			m[i] = INITVALSOBOL[i - 1 + temp];
		}

		//Computing direction numbers for dimension j
		unsigned *directionNumber = new unsigned[L + 1];
		if (L <= s) {
			for (unsigned i = 1; i <= L; i++) {
				directionNumber[i] = m[i] << (32 - i);
			}
		}
		else {
			for (unsigned i = 1; i <= s; i++){
				directionNumber[i] = m[i] << (32 - i);
			}
			for (unsigned i = s + 1; i <= L; i++) {
				directionNumber[i] = directionNumber[i - s] ^ (directionNumber[i - s] >> s);
				for (unsigned k = 1; k <= s - 1; k++) {
					directionNumber[i] ^= (((a >> (s - 1 - k)) & 1) * directionNumber[i - k]);
				}
			}
		}

		//Computing the jth dimension/coordinate of the points, analogously to before
		unsigned *currentCoord = new unsigned[numb];
		currentCoord[0] = 0;
		for (unsigned i = 1; i < numb; i++) {
			if (i == 0) {
				if (SHOTSSOBOL == 0) {
					continue;
				}
				else {
					currentCoord[i] = lastPackShot[j] ^ directionNumber[lastPackZero];
				}
			}
			else {
				currentCoord[i] = currentCoord[i - 1] ^ directionNumber[firstZero[i - 1]];
			}
			pack[i][j] = floor((double)currentCoord[i] / pow(2.0, 32) * N);
		}
		delete[] directionNumber;delete[] currentCoord;
	}

	//Increasing relative shot number
	SHOTSSOBOL += numb;
	//saving last shot of pack
	lastPackShot = pack[numb-1];

	return pack;
}

//Generates random strategy by filling the Strategyblock and then swapping the numbers (for one block)
// for multiple blocks uses generator
void randomStrategy(StrategyBlock* b, BlockCoordinate c)
{
	static std::uniform_int_distribution<u_int64_t> dis(0, CELLS - 1);
	if (BLOCK_COUNT == 1)
	{
		for (int i = 0; i < BLOCK_SIZE; ++i) {
			(*b)[i] = i + 1;
		}

		for (int i = 0; i < CELLS; ++i) {
			std::swap((*b)[i], (*b)[dis(GENERATOR)]);
		}
	}
	else
	{
		for (int i = 0; i < BLOCK_SIZE; ++i) {
			(*b)[i] = dis(GENERATOR) + 1;
		}
	}
}

//Full grid strategy. Calculates for each cell in a block to which layer it belongs.
// Then it can calculate to corresponding shot number from that layer.
void fullGridStrategy(StrategyBlock* b, BlockCoordinate c)
{
	Coordinate currentCell;
	u_int64_t shotNumber;
	for (int i = 0; i < BLOCK_SIZE; ++i) {
		currentCell = Coordinate(D, 0);
		int counter = i;
		for (int j = D-1; counter > 0; j--){
			currentCell[j] = counter % N;
			counter = counter / N;
		}
		
		for (int j = 0; j < BLOCK_DIMENSION_CUTOFF; j++){
			currentCell[j] = c[j];
		}
		
		int max = 0;
		for (int j = 0; j < D; j++) {
			for (int k = 0; k < CAP; k++) {
				if (currentCell[j]+1 == GRID_COORDINATES[k]) {
					if (std::floor(std::log2((double)k + 1)) > max) {
						max = std::log2((double)k + 1);
					}
					break;
				}
			}
		}
		shotNumber = 0;
		for (int j = 0; j < max; j++) {
			shotNumber += MAX_LEVEL_SHOTS_FULL[j];
		}
		shotNumber += LEVEL_SHOTS_FULL[max];
		LEVEL_SHOTS_FULL[max]++;

		(*b)[toIndex(currentCell)] = shotNumber;
	}
}

//Works similar to the full Grid strategy. The only difference is that the calculation
// of the maximum shots per layer whwere harded here.
void sparseGridStrategy(StrategyBlock* b, BlockCoordinate c) {

	Coordinate currentCell;
	u_int64_t shotNumber;
	for (int i = 0; i < BLOCK_SIZE; ++i) {
		currentCell = Coordinate(D, 0);
		int counter = i;
		for (int j = D - 1; counter > 0; j--) {
			currentCell[j] = counter % N;
			counter = counter / N;
		}

		for (int j = 0; j < BLOCK_DIMENSION_CUTOFF; j++) {
			currentCell[j] = c[j];
		}

		int sumLevel = 0;
		for (int j = 0; j < D; j++) {
			for (int k = 0; k < CAP; k++) {
				if (currentCell[j] + 1 == GRID_COORDINATES[k]) {
					sumLevel += std::floor(std::log2((double)k + 1)) + 1;
					break;
				}
			}
		}
		// sumLevel - D + 1 - 1, +1 from rule, -1 from array access
		sumLevel = sumLevel - D;
		shotNumber = 0;
		for (int j = 0; j < sumLevel; j++) {
			shotNumber += MAX_LEVEL_SHOTS_SPARSE[j];
		}
		shotNumber += LEVEL_SHOTS_SPARSE[sumLevel];
		LEVEL_SHOTS_SPARSE[sumLevel]++;

		(*b)[toIndex(currentCell)] = shotNumber;
	}
}

/*Calls for a halton number while keeping track of the realShotNumber.
Only when it gets a new cell (checks haltonHits) it increases the
relativeShotNumber.*/
void haltonStrategy(StrategyBlock* b, BlockCoordinate c) {
	u_int64_t relativeShotNumber = 1;
	u_int64_t realShotNumber = 1;
	Coordinate currentCell = emptyCoord();
	for (int j = 0; j < BLOCK_DIMENSION_CUTOFF; j++) {
		currentCell[j] = c[j];
	}
	while (relativeShotNumber <= BLOCK_SIZE) {
		for (int i = 0; i < D; i++) {
			int temp = halton(PRIMES[i], realShotNumber);
			if (i < BLOCK_DIMENSION_CUTOFF && temp != c[i]) {
				break;
			}
			if (i >= BLOCK_DIMENSION_CUTOFF) {
				currentCell[i] = temp;
			}
			if (i == D - 1) {
				if (!haltonHits[toIndex(currentCell)]) {
					haltonHits[toIndex(currentCell)] = true;
					(*b)[toIndex(currentCell)] = relativeShotNumber;
					relativeShotNumber++;
				}
			}
		}
		realShotNumber++;
		
	}
	
}

//Calls for packs of sobol points and checks them for hits. 
void sobolStrategy(StrategyBlock* b, BlockCoordinate c) {
	u_int64_t relativeShotNumber = 1;
	std::vector<Coordinate> shots;
	
	while (relativeShotNumber <= BLOCK_SIZE) {
		shots = sobolPacks(BLOCK_SIZE*N);
		for (int i = 0; i < BLOCK_SIZE*N; i++) {
			if (!haltonHits[toIndex(shots[i])]) {
				haltonHits[toIndex(shots[i])] = true;
				(*b)[toIndex(shots[i])] = relativeShotNumber;
				relativeShotNumber++;
			}
		}
	}
}

//All startegy commands
std::map<std::string, std::function<void (StrategyBlock*, BlockCoordinate)>> strategyNames = {
	{ "random", &randomStrategy },
	{ "fullGrid", &fullGridStrategy },
	{ "sparseGrid", &sparseGridStrategy},
	{ "halton", &haltonStrategy},
	{ "sobol", &sobolStrategy},
};

void createStrategy(std::string& name)
{
	generator = strategyNames[name];
}

BlockCoordinate generateBlock(StrategyBlock* b, int index)
{
	BlockCoordinate coord = emptyBlockCoord();
	for (int d = BLOCK_DIMENSION_CUTOFF - 1; d >= 0; --d) {
		coord[d] = index / static_cast<int>(DIMENSION_POWERS[d]);
		index = index % static_cast<int>(DIMENSION_POWERS[d]);
	}

	generator(b, coord);
	return coord;
}

//Traverses the blocks and finds for any ship the minimal shot.
uint64_t traverse(StrategyBlock& strat, std::vector<bool>& directions, Coordinate& c, Coordinate& c1, Coordinate& c2)
{
	for (int i = BLOCK_DIMENSIONS - 1; i >= 0; --i) {
		directions[i] = true;
	}
	c = Coordinate(c1);
	uint64_t min = CELLS;

	while (directions[0])
	{
		uint64_t index = toIndex(c);
		if (directions[BLOCK_DIMENSIONS - 1])
		{
			for (int i = 0; i <= c2[D - 1] - c1[D - 1]; ++i) {
				uint64_t value = AT(strat, index + i);
				if (value < min) {
					min = value;
				}
			}
			AT(c, D - 1) = AT(c2, D - 1);
		}
		else
		{
			for (int i = c2[D - 1] - c1[D - 1]; i >= 0; --i) {
				uint64_t value = AT(strat, index - i);
				if (value < min) {
					min = value;
				}
			}
			AT(c, D - 1) = AT(c1, D - 1);
		}

		for (int i = BLOCK_DIMENSIONS - 1; i >= 0; --i) {
			int cIndex = i + BLOCK_DIMENSION_CUTOFF;
			if (directions[i]) {
				if ((AT(c, cIndex) + 1) > AT(c2, cIndex))
				{
					directions[i] = !directions[i];
					continue;
				}
			}
			else
			{
				if ((AT(c, cIndex) - 1) < AT(c2, cIndex))
				{
					directions[i] = !directions[i];
					continue;
				}
			}

			if (directions[i]) {
				AT(c, cIndex)++;
			}
			else
			{
				AT(c, cIndex)--;
			}

			break;
		}
	}

	return min;

}

//To check if a ship is in a block.
bool inBlock(BlockCoordinate& c, Ship& s)
{
	for (int d = 0; d < BLOCK_DIMENSION_CUTOFF; ++d) {
		if ((s.min[d] < c[d] && s.max[d] < c[d]) || (s.min[d] > c[d] && s.max[d] > c[d]))
		{
			return false;
		}
	}
	return true;
}

//Combines the minimal hits for every block to find out when a ship was hit the first time
//scaled for the whole cube.
void findMins(int blockIndex, StrategyBlock* strat, std::vector<Ship>& ships, std::vector<uint64_t>& turns)
{
	std::vector<bool> workDirections = std::vector<bool>(BLOCK_DIMENSIONS, true);
	Coordinate workCoord = emptyCoord();
	BlockCoordinate c = generateBlock(strat, blockIndex);
	Coordinate min = emptyCoord(), max = emptyCoord();
	for (uint32_t i = 0; i < ships.size(); ++i) {
		Ship& ship = ships[i];
		if (inBlock(c, ship))
		{
			min = ship.min, max = ship.max;
			for (int d = 0; d < BLOCK_DIMENSION_CUTOFF; ++d) {
				min[d] = c[d];
				max[d] = c[d];
			}
			uint64_t minValue = traverse(*strat, workDirections, workCoord, min, max);
			if (minValue < turns[i])
			{
				turns[i] = minValue;
			}
		}
	}
}
