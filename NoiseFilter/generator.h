#ifndef GENERATOR_H_
#define GENERATOR_H_

#include <iostream>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <chrono>

#include "functions.h"

class Generator
{
	std::vector<float> &rawSignal;
	std::mutex &rawMutex;
	std::condition_variable &cv;

	std::thread m_genThread;
	std::atomic<bool> m_isStop;

	Logger& logger = Logger::get();

	void generateSignal();
	void printRawSignal();

public:
	explicit Generator(std::vector<float> &rawSignal, std::mutex &mutex, std::condition_variable &cv) :
						rawSignal(rawSignal), rawMutex(mutex), cv(cv) {}

	Generator(const Generator &) = delete;
	Generator& operator=(const Generator &) = delete;
	~Generator();

	void start();
};


#endif /* GENERATOR_H_ */
