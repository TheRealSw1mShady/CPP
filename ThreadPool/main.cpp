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

Logger& logger = Logger::get();

int divide(int a, int b)
{
	if (b == 0)
		throw std::runtime_error("Divide by zero");
	return a/b;
}

class ThreadPool
{
	short m_numThreads;
	std::vector<std::thread> m_threads;
	std::queue<std::function<void()>> m_tasks;
	std::mutex m_taskMutex;
	std::condition_variable m_cv;
	bool m_isDone = false;

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

		try
		{
			for (ushort thread = 0; thread < m_numThreads; ++thread)
				m_threads.emplace_back(&ThreadPool::worker, this);
		}
		catch (...)
		{
			stopWorkers();
			throw;
		}

		logger.write(std::to_string(m_numThreads) + " threads were created");
	}

	~ThreadPool()
	{
		stopWorkers();
		logger.write("ThreadPool is finished");
	}

	std::future<int> submit(std::function<int(int, int)> task, int a, int b)
	{
		std::future<int> future;
		{
			std::lock_guard lock(m_taskMutex);

			if (m_isDone)
				throw std::runtime_error("Stopped submiting tasks");

			auto shared = std::make_shared<std::packaged_task<int(int, int)>> (std::move(task));
			future = shared->get_future();

			auto lambda = [shared, a, b] () {(*shared)(a, b);};
			m_tasks.push(std::move(lambda));
		}
		m_cv.notify_one();
		return future;
	}

private:

	void stopWorkers()
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
	}

	void worker()
	{
		while(true)
		{
			std::unique_lock lock(m_taskMutex);

			m_cv.wait(lock, [&](){return (!m_tasks.empty() or m_isDone);});

			if (m_isDone and m_tasks.empty())
				break;

			auto task = std::move(m_tasks.front());
			m_tasks.pop();
			lock.unlock();

			task();

		}
	}
};


int main()
{
	int totalTasks = 15;

	std::vector<std::future<int>> results;
	results.reserve(totalTasks);

	try
	{
		ThreadPool pool(3);

		for (int i = 0; i < totalTasks; ++i)
			results.push_back(pool.submit(divide, (i+1)*(i+1), 10 - i));

		for (auto &res : results)
		{
			try
			{
				std::cout << res.get() << "\n";
			}
			catch (const std::runtime_error &e)
			{
				std::cout << e.what() << "\n";
			}
		}

	}
	catch(const std::exception &ex)
	{
		std::cout << ex.what() << "\n";
	}

	return 0;
}
