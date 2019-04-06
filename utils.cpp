#include "utils.h"
#include "fleet_distribution.h"
#include <climits>
#include "strategies.h"
#include "ship_generator.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <gmp.h>

void calcTurns(std::vector<Ship>& ships, std::vector<uint64_t>& turns)
{
	static StrategyBlock* block = emptyStrategyBlock();
	for (int b = 0; b <= BLOCK_COUNT; b++)
	{

		auto start = std::chrono::system_clock::now();

		findMins(b, block, ships, turns);

		auto time = std::chrono::duration_cast<std::chrono::milliseconds>((std::chrono::system_clock::now() - start)).count();
		std::cout << "traversed block " << b << " with " << SHIPS_SIZE << " ships in " << time << "ms (thread" << std::this_thread::get_id() << ")\n";
	}

}

void genShipsAndCalcTurns(std::vector<Ship>& ships, std::vector<uint64_t>& turns)
{
	static ShipGenerator gen = ShipGenerator();

	auto start = std::chrono::system_clock::now();
	gen(ships, GENERATOR);
	auto time = std::chrono::duration_cast<std::chrono::seconds>((std::chrono::system_clock::now() - start)).count();
	std::cout << "generated " << SHIPS_SIZE << " ships in " << time << " seconds (thread" << std::this_thread::get_id() << ")\n";

	calcTurns(ships, turns);
}

void calcExpectedValue(int id, int time, std::vector<uint64_t>* values, int* valueCount)
{
	std::vector<Ship> ships = std::vector<Ship>(SHIPS_SIZE, Ship {emptyCoord(), emptyCoord()});
	std::vector<uint64_t> turns = std::vector<uint64_t>(SHIPS_SIZE, CELLS);

	auto start = std::chrono::system_clock::now().time_since_epoch().count();
	auto end = start + time;

	int n = 0;

	for (; std::chrono::system_clock::now().time_since_epoch().count() <= end; n += SHIPS_SIZE) {
		genShipsAndCalcTurns(ships, turns);
		for (int i = 0; i < SHIPS_SIZE; ++i)
		{
			int clampedIndex = std::round((static_cast<double>(turns[i]) / static_cast<double>(CELLS)) * static_cast<double>(DATA_SIZE));
			(*values)[clampedIndex]++;
		}
	}
	*valueCount += n;

	std::cout << "thread " << id << " finished \n";
}

void outputData(std::vector<uint64_t>& values, int n)
{
	std::string value = "1.0@-" + std::to_string(n);
	mpf_t w;
	mpf_init(w);
	mpf_set_str(w, value.c_str(), -2);

	mpf_t newW;
	mpf_init(newW);
	mpf_set_d(newW, 1.0);

	mpf_t temp;
	mpf_init(temp);

	mpf_t m;
	mpf_init(m);
	mpf_set_d(m, 0.0);

	for (int i = 0; i < DATA_SIZE; ++i)
	{
		uint64_t turns = i * (CELLS / DATA_SIZE);
		std::string tmpValue = "1.0@" + std::to_string(values[i]);
		mpf_set_str(temp, tmpValue.c_str(), -2);
		mpf_mul(newW, w, temp);
		mpf_sub(temp, newW, w);
		mpf_set(w, newW);
		mpf_mul_ui(temp, temp, turns);
		mpf_add(m, m, temp);
	}

	double e = mpf_get_d(m);

	std::cout << "expected turn count: " << e << std::endl;
	std::cout << "ship sample count: " << n << std::endl;
}

PointMean calcExpectedValueMT(int threads, int time)
{
	std::thread t[threads];
	PointMean m[threads];

	std::vector<uint64_t> values = std::vector<uint64_t>(DATA_SIZE, 0);
	int n = 0;
	for(int i=0; i < threads;++i)
	{
		t[i] = std::thread(calcExpectedValue, i, time, &values, &n);
	}
	for (int i = 0; i < threads; ++i)
	{
		t[i].join();
	}

	outputData(values, n);

	PointMean mean = {};
	for (int i = 0; i < threads; ++i)
	{
		mean.mean += m[i].mean;
		mean.n += m[i].n;
	}
	mean.mean /= threads;
	return mean;
}