#pragma once

#include <vector>
#include "defs.h"
#include <random>

class FleetDistribution {

public:
	FleetDistribution();

	template< class Generator >
	uint64_t operator()(Generator& g) {
		static std::uniform_real_distribution<> dist(0.0, 1.0);
		static std::uniform_int_distribution<uint64_t> kAdd(0, DATA_SIZE);
		double cdf = dist(g);
		uint64_t val = binarySearch(0, DATA_SIZE - 1, cdf);
		uint64_t scaledK = (val * DATA_SIZE) + kAdd(g);
		return scaledK;
	}

private:
	std::vector<double> binoms = std::vector<double>(DATA_SIZE);

	uint64_t binarySearch(uint64_t start, uint64_t end, double val);
};
