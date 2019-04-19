#include "ship_generator.h"

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

template< class Generator >
void ShipGenerator::operator()(std::vector<Ship>& target, Generator& gen) {
	if (target.size() == SHIPS)
	{
		
	}
	else
	{
		static std::uniform_int_distribution<int> dis(0, (N * (N + 1) / 2) - 1);
		for (int i = 0; i < target.size(); ++i) {
			for (int d = 0; d < D; ++d) {
				int rnd = dis(gen);
				for (int j = N - 1; j >= 0; j--)
				{
					int val = values[j];
					if (val <= rnd)
					{
						AT(target[i].min, d) = j;
						AT(target[i].max, d) = j + (rnd - val);
						break;
					}
				}
			}
		}
	}
}

template void ShipGenerator::operator()(std::vector<Ship>& target, std::mt19937_64& gen);
