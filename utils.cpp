#include "utils.h"
#include <random>
#include "fleet_distribution.h"


void genFleet(Fleet& fleet, int k)
{
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_int_distribution<> dis(0, N - 1);

	for (int i = 0; i < k; ++i) {
		for (int d = 0; d < D; ++d) {
			int v1 = dis(gen);
			int v2 = dis(gen);
			fleet[i].min[d] = std::min(v1, v2);
			fleet[i].max[d] = std::max(v1, v2);
		}
	}
}

int toIndex(Coordinate& c)
{
	int index = 0;
	for (int d = D - 1; d >= 0; --d) {
		int offset = c[(D - 1) - d] * std::pow((double)N, d);
		index += offset;
	}
	return index;
}

int traverse(Strategy& strat, Coordinate& c, Coordinate& max, int d)
{
	int length = max[d] - c[d] + 1;
	int min = CELLS;
	if (d == D - 1)
	{
		int index = toIndex(c);
		for (int i = 0; i < length; ++i) {
			int value = strat[index + i];
			if (value < min) {
				min = value;
			}
		}
	}
	else {
		Coordinate cnew = c;
		for (int i = 0; i < length; ++i) {
			int value = traverse(strat, cnew, max, d + 1);
			if (value < min) {
				min = value;
			}
			cnew[d] = cnew[d] + 1;
		}
	}
	return min;

}

int traverse(Strategy& strat, Ship& s)
{
	return traverse(strat, s.min, s.max, 0);
}

int calcTurns(Strategy& strat, Fleet& fleet, int k)
{
	int turns = 0;
	for (int i = 0; i < k;++i) {
		Ship& ship = fleet[i];
		int min = traverse(strat, ship);
		if (min > turns)
		{
			turns = min;
		}
	}
	return turns;

}

double calcExpectedValue(Strategy& strat, int rounds)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	FleetDistribution dist = FleetDistribution();
	Fleet f = emptyFleet(SHIPS);
	double expected = 0;
	for (int i = 1; i <= rounds; ++i) {
		int k = dist(gen);
		genFleet(f, k);
		//std::cout << g << '\n';
		expected += (calcTurns(strat, f, k) - expected) / i;
	}
	return expected;
}