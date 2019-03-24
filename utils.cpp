#include "utils.h"
#include <random>
#include "fleet_distribution.h"
#include <climits>
#include "strategies.h"
#include "ship_generator.h"
#include <iostream>


int toIndex(Coordinate& c)
{
	int index = 0;
	for (int d = D - BLOCK_DIMENSION - 1; d >= 0; --d) {
		int offset = c[(D - 1) - d] * std::pow((double)N, d);
		index += offset;
	}
	return index;
}

int calcTurns(std::vector<Ship>& ships)
{
	static StrategyBlock* block = emptyStrategyBlock();
	int turns = -1;
	for (int b = 0; b <= BLOCK_COUNT; b++)
	{
		BlockCoordinate c = generateBlock(block, b);
		for (int i = 0; i < ships.size(); ++i) {
			Ship& ship = ships[i];
			int min = findMin(c, *block, ship);
			if (min > turns)
			{
				turns = min;
			}
		}
	}
	return turns;

}

int calcTurns(int currentTurns, std::vector<Ship>& ships, int shipsLeft)
{
	int size = std::min(shipsLeft, DATA_SIZE);
	static ShipGenerator gen = ShipGenerator();
	gen(ships, size, GENERATOR);
	int turns = calcTurns(ships);
	int maxTurns = std::max(currentTurns, turns);
	if (size > DATA_SIZE)
	{
		return calcTurns(maxTurns, ships, shipsLeft - DATA_SIZE);
	} else
	{
		return maxTurns;
	}
}

double calcExpectedValue(int rounds)
{
	static std::vector<Ship> ships = std::vector<Ship>(DATA_SIZE, Ship {emptyCoord(), emptyCoord()});
	FleetDistribution dist = FleetDistribution();
	double expected = 0;
	for (int i = 1; i <= rounds; ++i) {
		int k = dist(GENERATOR);
		std::cout << k << '\n';
		int turns = calcTurns(0, ships, k);
		std::cout << turns << "t \n";
		expected += (turns - expected) / i;
	}
	return expected;
}