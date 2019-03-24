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
		static std::uniform_int_distribution<> kAdd(0, DATA_SIZE);
		double cdf = dist(g);
		int val = binarySearch(0, DATA_SIZE - 1, cdf);
		int scaledK = (val * DATA_SIZE) + kAdd(g);
		return scaledK;
	}

private:
	std::vector<double> binoms = std::vector<double>(DATA_SIZE);

	int binarySearch(int start, int end, double val);
};
