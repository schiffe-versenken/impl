#include "strategies.h"
#include <random>

Strategy* testStrategy()
{
	Strategy* strat = new Strategy();
	for (int i = 0; i < CELLS; ++i) {
		(*strat)[i] = i + 1;
	}
	return strat;
}

Strategy* randomStrategy()
{
	Strategy* strat = new Strategy();
	strat->reserve(CELLS);
	for (int i = 0; i < CELLS; ++i) {
		strat->push_back(i + 1);
	}

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, CELLS - 1);

	for (int i = 0; i < CELLS; ++i) {
		std::swap((*strat)[i], (*strat)[dis(gen)]);
	}
	return strat;
}

