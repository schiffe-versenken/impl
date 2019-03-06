#include <random>
#include <iostream>
#include "utils.h"
#include "strategies.h"

std::random_device rd;  //Will be used to obtain a seed for the random number engine
std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
std::uniform_int_distribution<> dis(1, N);

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

int toIndex(Coordinate& c)
{
	int index = 0;
	for (int d = D - 1; d >= 0; --d) {
		int offset = c[(D - 1) - d] * (int) pow(N, d);
		index += offset;
	}
	return index;
}

int traverse(Strategy& strat, Coordinate& c, Coordinate& max, int d)
{
	int length = max[d] - c[d];
	int min = CELLS;
	if (d == D - 2)
	{
		int index = toIndex(c);
		for (int i = 0; i <= length; ++i) {
			int value = strat[index + i];
			if (value < min) {
				min = value;
			}
		}
	}
	else {
		Coordinate cnew = c;
		cnew[d] = cnew[d] + 1;
		for (int i = 0; i < length; ++i) {
			cnew[d] = cnew[d] + 1;
			int value = traverse(strat, cnew, max, d + 1);
			if (value < min) {
				min = value;
			}
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

float calcExpectedValue(Strategy& strat, int k)
{
	float expected = 0;
	int rounds = 100;
	for (int i = 1; i <= rounds; ++i) {
		Fleet f = genFleet(k);
		int e = calcTurns(strat, f);
		expected += (e - expected) / i;
	}
	return expected;

}


float calcExpectedValue(Strategy& strat)
{
	float expected = 0;
	for (int k = 1; k <= SHIPS; ++k) {
		float g = BINOMS[k] / (pow(2, SHIPS) - 1);
		expected += calcExpectedValue(strat, k) * g;
	}
	return expected;
}


int main()
{
	auto x = std::vector<Ship>{10};
	Strategy* strat = testStrategy();

	float e = calcExpectedValue(*strat);
	std::cout << e << '\n';
    printf("hello from ConsoleApplication1!\n");
    return 0;
}