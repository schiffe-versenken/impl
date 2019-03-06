#include <random>
#include <iostream>
#include "utils.h"
#include "strategies.h"

std::random_device rd;  //Will be used to obtain a seed for the random number engine
std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
std::uniform_int_distribution<> dis(0, N - 1);

Fleet genFleet(int k)
{

	Fleet fleet = std::vector<Ship>{ k };

	for (int i = 0; i < k; ++i) {
		Coordinate min;
		Coordinate max;
		for (int d = 0; d < D; ++d) {
			int v1 = dis(gen);
			int v2 = dis(gen);
			min[d] = std::min(v1, v2);
			max[d] = std::max(v1, v2);
		}
		fleet[i] = Ship{ min, max };
	}
	return fleet;

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

int calcTurns(Strategy& strat, Fleet& fleet)
{
	int turns = 0;
	for (Ship& ship : fleet) {
		int min = traverse(strat, ship);
		if (min > turns)
		{
			turns = min;
		}
	}
	return turns;

}

double calcExpectedValue(Strategy& strat, int k)
{
	double expected = 0;
	int rounds = 100;
	for (int i = 1; i <= rounds; ++i) {
		Fleet f = genFleet(k);
		int e = calcTurns(strat, f);
		std::cout << e << '\n';
		expected += (e - expected) / i;
	}
	return expected;

}


double calcExpectedValue(Strategy& strat)
{
	double expected = 0;
	for (int k = 1; k <= SHIPS; ++k) {
		double g = BINOMS[k] / (pow(2, SHIPS) - 1);
		std::cout << g << '\n';
		expected += calcExpectedValue(strat, k) * g;
	}
	return expected;
}


int main()
{
	Strategy* strat = testStrategy();
	double e = calcExpectedValue(*strat);
	std::cout << e << '\n';
    return 0;
}