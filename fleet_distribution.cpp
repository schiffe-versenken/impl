#include "fleet_distribution.h"

FleetDistribution::FleetDistribution()
{
	binoms[0] = 0;
	double value = 1;
	double sum = 0;
	double denominator = std::pow(2.0, DATA_SIZE) - 1 - 1;
	for (int i = 1; i < DATA_SIZE; ++i) {
		value = value * ((double)(DATA_SIZE - i + 1) / (double)i);
		sum = sum + value;
		binoms[i] = sum / denominator;
	}
}

uint64_t FleetDistribution::binarySearch(uint64_t start, uint64_t end, double val)
{
	if (start + 1 == end)
	{
		return end;
	}
	int mid = (start + end) / 2;
	double midVal = binoms[mid];
	if (val <= midVal)
	{
		return binarySearch(start, mid, val);
	}
	else
	{
		return binarySearch(mid, end, val);
	}
}
