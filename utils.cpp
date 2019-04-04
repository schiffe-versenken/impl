#include "utils.h"
#include "fleet_distribution.h"
#include <climits>
#include "strategies.h"
#include "ship_generator.h"
#include <iostream>
#include <chrono>
#include <thread>

void calcTurns(std::vector<Ship>& ships, std::vector<uint64_t>& turns)
{
	static StrategyBlock* block = emptyStrategyBlock();
	for (int b = 0; b <= BLOCK_COUNT; b++)
	{

		auto start = std::chrono::system_clock::now();
		BlockCoordinate c = generateBlock(block, b);

		for (uint32_t i = 0; i < ships.size(); ++i) {
			Ship& ship = ships[i];
			uint64_t min = findMin(c, *block, ship);
			if (min < turns[i])
			{
				turns[i] = min;
			}
		}

		auto time = std::chrono::duration_cast<std::chrono::milliseconds>((std::chrono::system_clock::now() - start)).count();
		std::cout << "traversed block " << b << " with " << DATA_SIZE << " ships in " << time << "ms (thread" << std::this_thread::get_id() << ")\n";
	}

}

void genShipsAndCalcTurns(std::vector<Ship>& ships, std::vector<uint64_t>& turns)
{
	static ShipGenerator gen = ShipGenerator();

	auto start = std::chrono::system_clock::now();
	gen(ships, DATA_SIZE, GENERATOR);
	auto time = std::chrono::duration_cast<std::chrono::seconds>((std::chrono::system_clock::now() - start)).count();
	std::cout << "generated " << DATA_SIZE << " ships in " << time << " seconds (thread" << std::this_thread::get_id() << ")\n";

	calcTurns(ships, turns);
}

void calcExpectedValue(int id, int time, PointMean* mean)
{
	static std::vector<Ship> ships = std::vector<Ship>(DATA_SIZE, Ship {emptyCoord(), emptyCoord()});
	std::vector<uint64_t> turns = std::vector<uint64_t>(DATA_SIZE, 0);
	static std::vector<uint64_t> values = std::vector<uint64_t>(DATA_SIZE, 0);

	auto start = std::chrono::system_clock::now().time_since_epoch().count();
	auto end = start + time;

	int n = 0;

	for (; std::chrono::system_clock::now().time_since_epoch().count() <= end; n += DATA_SIZE) {
		genShipsAndCalcTurns(ships, turns);
		for (int i = 0; i < DATA_SIZE; ++i)
		{
			int clampedIndex = std::round(static_cast<double>(turns[i]) / CELLS) * DATA_SIZE;
			values[clampedIndex]++;
		}
	}

	double w = 1.0 / (std::pow(2.0, n) - 1);
	for (int i = 0; i < DATA_SIZE; ++i)
	{
		uint64_t turns = i * (CELLS / DATA_SIZE);
		double newW = w * (std::pow(2.0, values[i]));
		double p = newW - w;
		w = newW;
		mean->mean += p * turns;
	}
	mean->n = n;

	std::cout << "thread " << id << " finished \n";
}

PointMean calcExpectedValueMT(int threads, int time)
{
	std::thread t[threads];
	PointMean m[threads];
	for(int i=0; i < threads;++i)
	{
		t[i] = std::thread(calcExpectedValue, i, time, &(m[i]));
	}
	for (int i = 0; i < threads; ++i)
	{
		t[i].join();
	}

	PointMean mean = {};
	for (int i = 0; i < threads; ++i)
	{
		mean.mean += m[i].mean;
		mean.n += m[i].n;
	}
	mean.mean /= threads;
	return mean;
}