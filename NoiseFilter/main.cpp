#include <iostream>
#include <vector>
#include <thread>
#include <queue>
#include <condition_variable>
#include <mutex>
#include <chrono>
#include <future>
#include <functional>
#include <stdexcept>
#include <cassert>

#include "functions.h"
#include "generator.h"
#include "filter.h"

int main()
{
	try
	{
		int numWorkers = 3;

		std::vector<float> rawSignal(numWorkers * 1000);
		std::mutex rawMutex;
		std::condition_variable rawCV;

		Generator gen(rawSignal, rawMutex, rawCV);
		NoiseFilter filter(rawSignal, rawMutex, rawCV, numWorkers);

		gen.start();

		std::this_thread::sleep_for(std::chrono::seconds(1));

	}
	catch(const std::exception &ex)
	{
		std::cout << ex.what() << "\n";
	}

	return 0;
}
