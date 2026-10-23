#ifndef FILTER_H_
#define FILTER_H_

#include <iostream>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <chrono>
#include <queue>
#include <future>
#include <functional>

#include "functions.h"


class NoiseFilter
{
	short m_numThreads;
	std::vector<std::thread> m_threads;
	std::queue<std::function<void()>> m_tasks;
	std::mutex m_taskMutex;
	std::condition_variable m_cv;
	std::atomic<bool> m_isDone;
	std::atomic<bool> m_stopWorkers;
	std::thread m_interruptionThread;
	std::vector<std::future<std::vector<float>>> m_results;

	std::vector<float> &rawSignal;
	std::vector<float> copiedSignal;
	std::mutex &rawMutex;
	std::condition_variable &rawCV;

	void printRawSignal();
	void printFilteredSignal();
	void dividingSignal();
	void stopWorkers();
	void worker();

public:
	NoiseFilter(std::vector<float> &signal, std::mutex &mutex, std::condition_variable &cv, short numThreads);
	~NoiseFilter();

	using vec = std::vector<float>;
	std::future<vec> submit(std::function<vec(vec&, int, int)> task, vec &signal, int begin, int end);
};


#endif /* FILTER_H_ */
