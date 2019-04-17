#include "utils.h"
#include <climits>
#include "strategies.h"
#include "ship_generator.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <gmp.h>
#include <iostream>
#include <fstream>
#include <atomic>

void calcTurns(std::vector<Ship>& ships, std::vector<uint64_t>& turns)
{
	static StrategyBlock* block = emptyStrategyBlock();
	for (int b = 0; b < BLOCK_COUNT; b++)
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

void calcExpectedValue(int id, std::vector<std::atomic<uint64_t>>* values)
{
	std::vector<Ship> ships = std::vector<Ship>(SHIPS_SIZE, Ship {emptyCoord(), emptyCoord()});
	std::vector<uint64_t> turns = std::vector<uint64_t>(SHIPS_SIZE, CELLS);

	genShipsAndCalcTurns(ships, turns);
	for (int i = 0; i < SHIPS_SIZE; ++i)
	{
		double shiftedTurns = static_cast<double>(turns[i]) - 1.0;
		int clampedIndex = std::round(shiftedTurns / static_cast<double>(CELLS - 1) * static_cast<double>(DATA_SIZE - 1));
		AT((*values), clampedIndex)++;
	}

	std::cout << "thread " << id << " finished \n";
}

void outputData(std::vector<std::atomic<uint64_t>>& values, int n)
{
	std::ofstream resultsFile;

	char buff[100];
	snprintf(buff, sizeof(buff), "results-%u-%u.txt", N, D);
	std::string buffAsStdStr = buff;
	if (std::ifstream(buffAsStdStr).good())
	{
		std::remove(buffAsStdStr.c_str());
	}
	resultsFile.open(buffAsStdStr);
	resultsFile << n << " " << SHIPS << " " << CELLS << " ";

	//Magic

	std::string value = "1.0@-" + std::to_string(SHIPS);
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

	uint64_t sum = 0;

	double eShips = 0;
	for (int i = 0; i < DATA_SIZE; ++i)
	{
		sum += values[i];
		uint64_t newValue = std::round(values[i] * ((double)SHIPS / (double)n));
		uint64_t newValueSum = std::round(sum * (double)SHIPS / (double)n);
		uint64_t turns = (i +1) * (CELLS / DATA_SIZE);
		std::string tmpValue = "1.0@" + std::to_string(newValueSum - SHIPS);
		mpf_set_str(newW, tmpValue.c_str(), -2);
		//mpf_mul(newW, w, temp);
		mpf_sub(temp, newW, w);
		mpf_set(w, newW);
		double pShips = values[i] / static_cast<double>(n);
		eShips += pShips * turns;
		resultsFile << turns << "," << (sum / static_cast<double>(n)) << "," << (static_cast<int64_t>(newValueSum) - static_cast<int64_t>(SHIPS)) << " ";
		mpf_mul_ui(temp, temp, turns);
		mpf_add(m, m, temp);
	}

	resultsFile << eShips << " " << mpf_get_d(m);
	resultsFile.close();

	std::cout << "Output written " << mpf_get_d(m) << std::endl;
}

void calcExpectedValueMT(int threads)
{
	auto start = std::chrono::system_clock::now();
	std::thread t[threads];

	std::vector<std::atomic<uint64_t>> values = std::vector<std::atomic<uint64_t>>(DATA_SIZE);
	int n = threads * SHIPS_SIZE;
	for(int i=0; i < threads;++i)
	{
		t[i] = std::thread(calcExpectedValue, i, &values);
	}
	for (int i = 0; i < threads; ++i)
	{
		t[i].join();
	}

	outputData(values, n);
	auto time = std::chrono::duration_cast<std::chrono::seconds>((std::chrono::system_clock::now() - start)).count();
	std::cout << "Elapsed time: " << time << " seconds\n";
}
