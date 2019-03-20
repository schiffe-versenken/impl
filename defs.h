#pragma once

#include <vector>
#include <array>

extern int N;
extern int D;
extern int SHIPS;
extern int CELLS;
extern double FLEETS;

typedef std::vector<int> Coordinate;

struct Ship
{
	Coordinate min;
	Coordinate max;
};

struct Tuple
{
    std::vector<int> coords;
    std::vector<int> levels;
};

using Fleet = std::vector<Ship>;

using Strategy = std::vector<int>;

void initValues(int n, int d);

Coordinate emptyCoord();

Fleet emptyFleet(int k);

Strategy* emptyStrategy();
