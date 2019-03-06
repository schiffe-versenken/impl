#pragma once

#include <array>
#include <vector>

constexpr int const_pow(int i, int k)
{
	return k == 0 ? 1 : const_pow(i, k - 1) * i;
}

constexpr int N = 3;
constexpr int D = 2;

constexpr int SHIPS = const_pow(N * (N + 1), D);
constexpr int CELLS = const_pow(N, D);
extern double FLEETS;

extern std::vector<double> BINOMS;

typedef std::array<int, D> Coordinate;

struct Ship
{
	Coordinate min;
	Coordinate max;
};

using Fleet = std::vector<Ship>;

int genRandomCoordinate();

inline int toIndex(Coordinate& c)
{
	int index = 0;
	for (int d = D - 1; d >= 0; --d) {
		int offset = c[(D - 1) - d] * const_pow(N, d);
		index += offset;
	}
	return index;
}