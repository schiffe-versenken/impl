#pragma once

#include "utils.h"

void createStrategy(std::string& name);

BlockCoordinate generateBlock(StrategyBlock* b, int index);

uint64_t findMin(BlockCoordinate& c, StrategyBlock& strat, Ship& s);
