#include "strategies.h"
#include <random>
#include <map>
#include <functional>

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

void randomStrategy(StrategyBlock* b, BlockCoordinate c)
{
	std::uniform_int_distribution<u_int64_t> dis(1, CELLS);
	for (int i = 0; i < BLOCK_SIZE; ++i) {
		(*b)[i] = dis(GENERATOR);
	}
}

void fullGridStrategy(StrategyBlock* b, BlockCoordinate c)
{
	/*Coordinate currentCell;
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
			for (int k = 0; k < N; k++) {
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
	}*/
}

void sparseGridStrategy(StrategyBlock* b, BlockCoordinate c) {
	int L = 0;
	int countCoord = 0;
	int position = 0;
	int maxLevels = (int)ceil(log2((double)N));
	std::vector<std::vector<int> > K;
	K.reserve(maxLevels);
	for (int i = 0; i <= maxLevels; i++) {
		K.push_back(std::vector<int>((int)pow(2, i), 0));
	}

	K[0][0] = (int)floor((double)N / 2);
	int maxCoord = K[0][0];
	std::vector<Tuple> lastShot;
	lastShot.reserve(BLOCK_SIZE);
	Tuple t1;
	t1.coords = std::vector<int>(BLOCK_DIMENSIONS, maxCoord);
	t1.levels = std::vector<int>(BLOCK_DIMENSIONS, 0);
	lastShot.push_back(t1);
	(*b)[0] = toIndex(lastShot[0].coords);

	for (int i = 1; i < BLOCK_SIZE; ++i) {
		std::vector<int> vgl(BLOCK_DIMENSIONS, K[0][0]);
		vgl.back() = maxCoord;
		if (lastShot.back().coords == vgl) {
			int min = (int)ceil((double)K[L][0] / 2);
			std::vector<int> KAdd;
			K.push_back(KAdd);
			for (int k = 0; k < (int)pow(2, L + 1); k += 2) {
				K[L + 1][k] = K[L][k / 2] - min;
				K[L + 1][k + 1] = K[L][k / 2] + min;
			}
			L++;
			maxCoord = K[L].back();
			countCoord = 0;
			position = 0;
		}

		Tuple out = lastShot[0];
		if (countCoord == 0) {
			out.levels[position]++;
		}
		out.coords[position] = K[out.levels[position]][countCoord];
		countCoord++;
		if (countCoord == K[out.levels[position]].size()) {
			countCoord = 0;
			position++;
			if (position == D) {
				position = 0;
				lastShot.erase(lastShot.begin());
			}
		}
		lastShot.push_back(out);
		(*b)[i] = toIndex(out.coords);
	}
}

std::map<std::string, std::function<void (StrategyBlock*, BlockCoordinate)>> strategyNames = {
	{ "random", &randomStrategy },
	{ "fullGrid", &fullGridStrategy },
	{ "sparseGrid", &sparseGridStrategy}
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
	c = c1;
	uint64_t min = CELLS;

	while (directions[0])
	{
		uint64_t index = toIndex(c);
		if (directions[BLOCK_DIMENSIONS - 1])
		{
			for (int i = 0; i <= c2[D - 1] - c1[D - 1]; ++i) {
				uint64_t value = strat[index + i];
				if (value < min) {
					min = value;
				}
			}
			c[D - 1] = c2[D - 1];
		}
		else
		{
			for (int i = c2[D - 1] - c1[D - 1]; i >= 0; --i) {
				uint64_t value = strat[index + i];
				if (value < min) {
					min = value;
				}
			}
			c[D - 1] = c1[D - 1];
		}

		for (int i = BLOCK_DIMENSIONS - 1; i >= 0; --i) {
			int cIndex = i + BLOCK_DIMENSION_CUTOFF;
			if (directions[i]) {
				if ((c[cIndex] + 1) > c2[cIndex])
				{
					directions[i] = !directions[i];
					continue;
				}
			}
			else
			{
				if ((c[cIndex] - 1) < c2[cIndex])
				{
					directions[i] = !directions[i];
					continue;
				}
			}

			if (directions[i]) {
				c[cIndex]++;
			}
			else
			{
				c[cIndex]--;
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
	Coordinate min, max = emptyCoord();
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
