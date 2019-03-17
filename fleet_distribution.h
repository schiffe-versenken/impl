#pragma once

#include <vector>
#include "defs.h"
#include <random>

class FleetDistribution {

public:
	FleetDistribution();

	template< class Generator >
	int operator()(Generator& g) {
		static std::uniform_real_distribution<> dist(0.0, 1.0);
		double cdf = dist(g);
		return binarySearch(0, SHIPS, cdf);
	}

private:
	std::vector<double> binoms = std::vector<double>(SHIPS + 1);

	int binarySearch(int start, int end, double val);
};
