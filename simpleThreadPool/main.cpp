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

#include "functions.h"

class ThreadPool
{
	short m_numThreads;
	std::vector<std::thread> m_threads;
	std::queue<std::function<void()>> m_tasks;
	std::mutex m_taskMutex;
	std::condition_variable m_cv;
	bool m_isDone = false;
	Logger& logger = Logger::get();

public:
	ThreadPool(short numThreads = 4) : m_numThreads(numThreads)
	{
		if (numThreads <= 0)
			throw std::invalid_argument("Total threads number should be positive!");

		uint maxThreads = std::thread::hardware_concurrency();

		if (maxThreads == 0)
			m_numThreads = 1;
		else if (numThreads >= maxThreads)
			m_numThreads = maxThreads;
		else
			m_numThreads = numThreads;

		m_threads.reserve(m_numThreads);

		for (ushort thread = 0; thread < m_numThreads; ++thread)
			m_threads.emplace_back(&ThreadPool::worker, this, thread + 1);

		logger.write(std::to_string(m_numThreads) + " threads were created");
	}

	~ThreadPool()
	{
		{
			std::lock_guard lock(m_taskMutex);
			m_isDone = true;
		}
		m_cv.notify_all();

		logger.write("Waiting for workers");

		for (std::thread &th : m_threads)
		{
			if (th.joinable())
				th.join();
		}
		logger.write("ThreadPool is finished");
	}

	void worker(int id)
	{
		while(true)
		{
			std::unique_lock lock(m_taskMutex);

			m_cv.wait(lock, [&](){return (!m_tasks.empty() or m_isDone);});

			if (m_isDone and m_tasks.empty())
				break;

			std::function<void()> task = std::move(m_tasks.front());
			m_tasks.pop();
			lock.unlock();

			task();

		}
	}

	void submit(std::function<void()> task)
	{
		{
			std::lock_guard lock(m_taskMutex);

			if (m_isDone)
				throw std::runtime_error("Stopped submiting tasks");
			m_tasks.push(std::move(task));
		}
		m_cv.notify_one();
	}

};

void task()
{
	std::cout << "TASK TASK TASK\n";
}

int main()
{
	try
	{
		ThreadPool pool(5);

		for (int i = 0; i < 13; ++i)
		{
			pool.submit(task);
			//std::this_thread::sleep_for(std::chrono::milliseconds(30));
		}

	}
	catch(const std::exception &ex)
	{
		std::cout << ex.what() << std::endl;
	}

	return 0;
}
