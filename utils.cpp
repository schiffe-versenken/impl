#include "utils.h"
#include "fleet_distribution.h"
#include <climits>
#include "strategies.h"
#include "ship_generator.h"
#include <iostream>
#include <chrono>
#include <thread>

uint64_t calcTurns(std::vector<Ship>& ships)
{
	static StrategyBlock* block = emptyStrategyBlock();
	uint64_t turns = -1;
	for (int b = 0; b <= BLOCK_COUNT; b++)
	{

		auto start = std::chrono::system_clock::now();
		BlockCoordinate c = generateBlock(block, b);

		for (uint32_t i = 0; i < ships.size(); ++i) {
			Ship& ship = ships[i];
			uint64_t min = findMin(c, *block, ship);
			if (min > turns)
			{
				turns = min;
			}
		}

		auto time = std::chrono::duration_cast<std::chrono::milliseconds>((std::chrono::system_clock::now() - start)).count();
		std::cout << "traversed block " << b << " with " << DATA_SIZE << " ships in " << time << "ms (thread" << std::this_thread::get_id() << ")\n";
	}
	return turns;

}

uint64_t calcTurns(uint64_t currentTurns, std::vector<Ship>& ships, uint64_t shipsLeft)
{
	std::cout << shipsLeft << "left \n";
	uint64_t size = std::min(shipsLeft, DATA_SIZE);


	static ShipGenerator gen = ShipGenerator();
	auto start = std::chrono::system_clock::now();
	gen(ships, size, GENERATOR);
	auto time = std::chrono::duration_cast<std::chrono::seconds>((std::chrono::system_clock::now() - start)).count();
	std::cout << "generated " << DATA_SIZE << " ships in " << time << " seconds (thread" << std::this_thread::get_id() << ")\n";

	uint64_t turns = calcTurns(ships);
	uint64_t maxTurns = std::max(currentTurns, turns);
	if (size > DATA_SIZE)
	{
		return calcTurns(maxTurns, ships, shipsLeft - DATA_SIZE);
	} else
	{
		return maxTurns;
	}
}

void calcExpectedValue(int id, int time, PointMean* mean)
{
	static std::vector<Ship> ships = std::vector<Ship>(DATA_SIZE, Ship {emptyCoord(), emptyCoord()});
	FleetDistribution dist = FleetDistribution();

	auto start = std::chrono::system_clock::now().time_since_epoch().count();
	auto end = start + time;

	mean->mean = 0;
	mean->n = 0;

	for (; std::chrono::system_clock::now().time_since_epoch().count() <= end; ++mean->n) {
		uint64_t k = dist(GENERATOR);
		std::cout << k << '\n';
		uint64_t turns = calcTurns(0, ships, DATA_SIZE);
		std::cout << turns << "t \n";
		mean->mean += (static_cast<double>(turns) - mean->mean) / static_cast<double>(mean->n);
	}

	std::cout << "thread " << id << "finished \n";
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