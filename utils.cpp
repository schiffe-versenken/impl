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
	static StrategyBlock* block = new StrategyBlock(1);
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

void calcExpectedValue(int id, std::vector<uint64_t>* values)
{
	std::vector<Ship> ships = std::vector<Ship>(SHIPS_SIZE, Ship {emptyCoord(), emptyCoord()});
	std::vector<uint64_t> turns = std::vector<uint64_t>(SHIPS_SIZE, CELLS);

	genShipsAndCalcTurns(ships, turns);
	for (int i = 0; i < SHIPS_SIZE; ++i)
	{
		double shiftedTurns = static_cast<double>(turns[i]) - 1.0;
		int clampedIndex = std::round(shiftedTurns / static_cast<double>(CELLS - 1) * static_cast<double>(OUTPUT_SIZE - 1));
		AT((*values), clampedIndex)++;
	}

	std::cout << "thread " << id << " finished \n";
}

void outputData(std::vector<uint64_t>& values, uint64_t n)
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

	double sum = 0;
	double eShips = 0;
	for (int i = 0; i < OUTPUT_SIZE; ++i)
	{
		int last = i == 0 ? 0 : values[i - 1];
		sum = values[i];
		double newValueSum = sum * (SHIPS / (double)n);
		double turns = (i + 1) * (CELLS / OUTPUT_SIZE);
		double pShips = (values[i] - last) / static_cast<double>(n);
		eShips += pShips * turns;
		resultsFile << turns << "," << (sum / static_cast<double>(n)) << "," << newValueSum - SHIPS << " ";
	}

	resultsFile << eShips << " " << 0;
	resultsFile.close();

	std::cout << "Output written " << "none" << std::endl;
}

void calcExpectedValueMT(int threads)
{
	auto start = std::chrono::system_clock::now();
	std::thread t[threads];

	std::vector<uint64_t> values = std::vector<uint64_t>(OUTPUT_SIZE, 0);
	std::vector<std::vector<uint64_t>> newValues = std::vector<std::vector<uint64_t>>(threads, std::vector<uint64_t>(OUTPUT_SIZE, 0));
	uint64_t n = SHIPS_SIZE;
	for(int i=0; i < threads;++i)
	{
		t[i] = std::thread(calcExpectedValue, i, &newValues[i]);
	}
	for (int i = 0; i < threads; ++i)
	{
		t[i].join();
		uint64_t sum = 0;
		for (int j = 0;j < OUTPUT_SIZE; ++j)
		{
			sum += newValues[i][j];
			if (sum > values[j])
			{
				values[j] = sum;
			}
		}
	}

	outputData(values, n);
	auto time = std::chrono::duration_cast<std::chrono::seconds>((std::chrono::system_clock::now() - start)).count();
	std::cout << "Elapsed time: " << time << " seconds\n";
}
