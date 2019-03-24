#pragma once

#include "utils.h"

void createStrategy(std::string& name);

BlockCoordinate generateBlock(StrategyBlock* b, int index);

int findMin(BlockCoordinate& c, StrategyBlock& strat, Ship& s);
