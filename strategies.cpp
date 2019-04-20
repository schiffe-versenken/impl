#include "strategies.h"
#include <random>
#include <map>
#include <functional>
#include <cassert>

std::function<void(StrategyBlock*, BlockCoordinate)> generator;

uint64_t toIndex(Coordinate& c)
{
	uint64_t index = 0;
	for (int d = D - BLOCK_DIMENSION_CUTOFF - 1; d >= 0; --d) {
		uint64_t offset = c[(D - 1) - d] * DIMENSION_POWERS[d];
		index += offset;
	}
	return index;
}

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

void randomStrategy(StrategyBlock* b, BlockCoordinate c)
{
	static std::uniform_int_distribution<u_int64_t> dis(1, CELLS);
	if (BLOCK_COUNT == 1)
	{
		for (int i = 0; i < BLOCK_SIZE; ++i) {
			(*b)[i] = i + 1;
		}

		for (int i = 0; i < CELLS; ++i) {
			std::swap((*b)[i], (*b)[dis(GENERATOR) - 1]);
		}
	}
	else
	{
		for (int i = 0; i < BLOCK_SIZE; ++i) {
			(*b)[i] = dis(GENERATOR);
		}
	}
}

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

void haltonStrategy(StrategyBlock* b, BlockCoordinate c) {
	int relativeShotNumber = 1;
	int realShotNumber = 1;
	Coordinate currentCell = emptyCoord();
	for (int j = 0; j < BLOCK_DIMENSION_CUTOFF; j++) {
		currentCell[j] = c[j];
	}
	while (relativeShotNumber < BLOCK_SIZE) {
		for (int i = 0; i < D; i++) {
			int temp = halton(PRIMES[i], realShotNumber);
			if (i < BLOCK_DIMENSION_CUTOFF && temp != c[i]) {
				break;
			}
			if (i >= BLOCK_DIMENSION_CUTOFF) {
				currentCell[i] = temp;
			}
			if (i == D - 1) {
				if ((*b)[toIndex(currentCell)] == 0) {
					(*b)[toIndex(currentCell)] = relativeShotNumber;
					relativeShotNumber++;
				}
			}
		}
		realShotNumber++;
	}
}

std::map<std::string, std::function<void (StrategyBlock*, BlockCoordinate)>> strategyNames = {
	{ "random", &randomStrategy },
	{ "fullGrid", &fullGridStrategy },
	{ "sparseGrid", &sparseGridStrategy},
	{ "halton", &haltonStrategy}
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
