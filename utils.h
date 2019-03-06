#ifndef UTILS
#define UTILS 1

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

extern std::vector<float> BINOMS;

typedef std::array<int, D> Coordinate;

struct Ship
{
	Coordinate min;
	Coordinate max;
};

using Fleet = std::vector<Ship>;

#endif