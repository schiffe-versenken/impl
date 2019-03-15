#include "strategies.h"
#include <random>
#include <map>
#include <functional>

Strategy* testStrategy()
{
	Strategy* strat = emptyStrategy();
	for (int i = 0; i < CELLS; ++i) {
		(*strat)[i] = i + 1;
	}
	return strat;
}

Strategy* randomStrategy()
{
	Strategy* strat = emptyStrategy();
	for (int i = 0; i < CELLS; ++i) {
		(*strat)[i] = i + 1;
	}

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, CELLS - 1);

	for (int i = 0; i < CELLS; ++i) {
		std::swap((*strat)[i], (*strat)[dis(gen)]);
	}
	return strat;
}

std::map<std::string, std::function<Strategy*()>> strategyNames = {
	{ "test", &testStrategy },
	{ "random", &randomStrategy }
};

Strategy* createStrategy(std::string& name)
{
	return strategyNames[name]();
}
