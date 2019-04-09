#pragma once

#include "defs.h"

void createStrategy(std::string& name);

BlockCoordinate generateBlock(StrategyBlock* b, int index);

void findMins(int blockIndex, StrategyBlock* strat, std::vector<Ship>& ships, std::vector<uint64_t>& turns);
