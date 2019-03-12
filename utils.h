#pragma once

#include "defs.h"

void genFleet(Fleet& fleet, int k);

int traverse(Strategy& strat, Ship& s);

int toIndex(Coordinate& c);

double calcExpectedValue(Strategy& strat);