#include "fleet_distribution.h"

FleetDistribution::FleetDistribution()
{
	binoms[0] = 0;
	double value = 1;
	double sum = 0;
	for (int i = 1; i <= SHIPS; ++i) {
		value = value * ((double)(SHIPS - i + 1) / (double)i);
		sum = sum + value;
		binoms[i] = sum / FLEETS;
	}
}

int  FleetDistribution::binarySearch(int start, int end, double val)
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
