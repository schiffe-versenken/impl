#include "ship_generator.h"
#include <iostream>
#ifdef DEBUG
#include <cassert>
#endif

ShipGenerator::ShipGenerator() : values(N, 0)
{
	int sum = 0;
	for (int i = 0; i < N; i++)
	{
		values[i] = sum;
		int left = N - i;
		sum += left;
	}
}

std::pair<int, int> ShipGenerator::getMinMax(int index)
{
	for (int j = N - 1; j >= 0; j--)
	{
		int val = values[j];
		if (val <= index)
		{
			return { j, j + (index - val) };
		}
	}
}

Ship ShipGenerator::fromIndex(uint64_t index)
{
	Coordinate min = emptyCoord();
	Coordinate max = emptyCoord();
	for (int d = D - 1; d >= 0; --d) {
		uint64_t pow = std::pow((N * (N + 1)) / 2, (double) d);
		std::pair<int, int> minMax = getMinMax(index / static_cast<uint64_t>(pow));
		min[d] = minMax.first;
		max[d] = minMax.second;
		index = index % static_cast<uint64_t>(pow);
	}
	return Ship{ min, max };
}

template< class Generator >
void ShipGenerator::operator()(std::vector<Ship>& target, Generator& gen) {
	if (target.size() == SHIPS)
	{
		for (int i = 0; i < target.size(); ++i)
		{
			target[i] = fromIndex(i);
		}
	}
	else
	{
		static std::uniform_int_distribution<int> dis(0, (N * (N + 1) / 2) - 1);
		for (int i = 0; i < target.size(); ++i) {
			for (int d = 0; d < D; ++d) {
				int rnd = dis(gen);
				std::pair<int, int> minMax = getMinMax(rnd);
				AT(target[i].min, d) = minMax.first;
				AT(target[i].max, d) = minMax.second;
			}
		}
	}
}

template void ShipGenerator::operator()(std::vector<Ship>& target, std::mt19937_64& gen);
