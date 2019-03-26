#pragma once

#include "defs.h"


struct PointMean
{
	double mean;
	uint64_t n;
};

PointMean calcExpectedValueMT(int threads, int time);