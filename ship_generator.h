#pragma once

#include <vector>
#include "defs.h"
#include <random>

class ShipGenerator {

public:
	template< class Generator >
	void operator()(std::vector<Ship>& target, Generator& gen) {
		static std::uniform_int_distribution<> dis(0, N - 1);

		for (int i = 0; i < target.size(); ++i) {
			for (int d = 0; d < D; ++d) {
				int v1 = dis(gen);
				int v2 = dis(gen);
				target[i].min[d] = std::min(v1, v2);
				target[i].max[d] = std::max(v1, v2);
			}
		}
	}
};
