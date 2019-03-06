#include "strategies.h"

Strategy* testStrategy()
{
	Strategy* strat = new Strategy{};
	for (int i = 0; i < CELLS; ++i) {
		(*strat)[i] = i;
	}
	return strat;
}
