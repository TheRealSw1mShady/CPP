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

Logger& logger = Logger::get();

std::vector<float> process(std::vector<float> &signal, int begin, int end)
{
	std::vector<float> part;
	part.resize(end - begin);

	for (int i = begin; i < end; ++i)
	{
		if (i != begin and i != end - 1)
			part[i - begin] = (signal[i-1] + signal[i] + signal[i+1])/3;
		else if (i == begin)
			part[i - begin] = (signal[i] + signal[i+1])/2;
		else
			part[i - begin] = (signal[i-1] + signal[i])/2;
	}
	return part;
}

std::atomic<bool> stopGnt = false;

class Generator
{
	std::vector<float> &rawSignal;
	std::mutex &rawMutex;
	std::condition_variable &cv;
	std::thread m_genThread;

	void generateSignal()
	{
		stopGnt.store(false);
		for (int i = 0; i < 10; ++i)
		{
			std::unique_lock lock(rawMutex);
			for (int i = 0; i < rawSignal.size(); i++)
				rawSignal[i] = (sin(i/15.0 + getRandomNumber(-20, 20)/100.0));
			lock.unlock();

			cv.notify_one();
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
		logger.write("Stopped generating");
		stopGnt.store(true);
	}

	void printRawSignal()
	{
		std::lock_guard lock(rawMutex);

		for (int i = 0; i < rawSignal.size(); i++)
			std::cout << std::fixed << std::setprecision(2) << rawSignal[i] << "  ";
		std::cout << "\n";
	}

public:
	explicit Generator(std::vector<float> &rawSignal, std::mutex &mutex, std::condition_variable &cv) :
						rawSignal(rawSignal), rawMutex(mutex), cv(cv) {}

	Generator(const Generator &) = delete;
	Generator& operator=(const Generator &) = delete;
	~Generator()
	{
		cv.notify_one();
		logger.write("Generator is finished");

		if (m_genThread.joinable())
			m_genThread.join();
	}

	void start()
	{
		m_genThread = std::thread(&Generator::generateSignal, this);
	}

};

class NoiseFilter
{
	short m_numThreads;
	std::vector<std::thread> m_threads;
	std::queue<std::function<void()>> m_tasks;
	std::mutex m_taskMutex;
	std::condition_variable m_cv;
	std::atomic<bool> m_isDone;
	std::thread m_interruptionThread;
	std::vector<std::future<std::vector<float>>> m_results;

	std::vector<float> &rawSignal;
	std::vector<float> copiedSignal;

	std::mutex &rawMutex;
	std::condition_variable &rawCV;

public:
	NoiseFilter(std::vector<float> &signal, std::mutex &mutex, std::condition_variable &cv, short numThreads = 3) :
					m_numThreads(numThreads), rawSignal(signal), rawMutex(mutex), rawCV(cv)
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
		m_results.resize(m_numThreads);
		m_isDone.store(false);

		try
		{
			for (ushort thread = 0; thread < m_numThreads; ++thread)
				m_threads.emplace_back(&NoiseFilter::worker, this);

			m_interruptionThread = std::thread(&NoiseFilter::dividingSignal, this);
		}
		catch (...)
		{
			stopWorkers();
			throw;
		}

		logger.write(std::to_string(m_numThreads) + " threads were created");
	}

	~NoiseFilter()
	{
		stopWorkers();
		logger.write("NoiseFilter is finished");
	}

	using vec = std::vector<float>;

	std::future<vec> submit(std::function<vec(vec&, int, int)> task, vec &signal, int begin, int end)
	{
		std::future<vec> future;
		{
			std::lock_guard lock(m_taskMutex);

			if (m_isDone.load())
				throw std::runtime_error("Stopped submiting tasks");

			auto shared = std::make_shared<std::packaged_task<vec(vec&, int, int)>> (std::move(task));
			future = shared->get_future();

			auto lambda = [shared, &signal, begin, end] () {(*shared)(signal, begin, end);};
			m_tasks.push(std::move(lambda));
		}
		m_cv.notify_one();
		return future;
	}

private:

	void printRawSignal()
	{
		std::cout << "\n";
		for (int i = 0; i < copiedSignal.size(); i++)
			std::cout << std::fixed << std::setprecision(2) << rawSignal[i] << "  ";
		std::cout << "\n";
	}

	void printFilteredSignal()
	{
		for (std::future<std::vector<float>> &res : m_results)
		{
			std::vector<float> v = res.get();
			for (const auto &elem : v)
				std::cout << std::fixed << std::setprecision(2) << elem << "  ";
		}
		std::cout << "\n";
	}

	void dividingSignal()
	{
		try
		{
			while (true)
			{
				std::unique_lock lock(rawMutex);
				rawCV.wait(lock);

				if (stopGnt.load())
				{
					m_isDone.store(true);
					break;
				}

				copiedSignal = rawSignal;
				lock.unlock();

				printRawSignal();

				for (int i = 0; i < m_numThreads; ++i)
					m_results[i] = submit(process, copiedSignal, i * 1000, (i * 1000) + 1000);

				printFilteredSignal();
			}
		}
		catch (const std::runtime_error &e)
		{
			std::cout << e.what() << "\n";
		}
	}


	void stopWorkers()
	{
		m_isDone.store(true);
		m_cv.notify_all();

		logger.write("Waiting for workers");

		if (m_interruptionThread.joinable())
			m_interruptionThread.join();

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

			m_cv.wait(lock, [&](){return (!m_tasks.empty() or m_isDone.load());});

			if (m_isDone.load() and m_tasks.empty())
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
	try
	{
		std::vector<float> rawSignal(4000);
		std::mutex rawMutex;
		std::condition_variable rawCV;

		Generator gen(rawSignal, rawMutex, rawCV);
		NoiseFilter filter(rawSignal, rawMutex, rawCV, 4);

		gen.start();

		std::this_thread::sleep_for(std::chrono::seconds(5));

	}
	catch(const std::exception &ex)
	{
		std::cout << ex.what() << "\n";
	}

	return 0;
}
