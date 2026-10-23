#include "filter.h"

std::vector<float> algorythm(std::vector<float> &signal, int begin, int end)
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

NoiseFilter::NoiseFilter(std::vector<float> &signal, std::mutex &mutex, std::condition_variable &cv, short numThreads = 3) :
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
	m_isDone.store(false, std::memory_order_relaxed);
	m_stopWorkers.store(false, std::memory_order_relaxed);

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

	Logger::get().write(std::to_string(m_numThreads) + " threads were created");
}

NoiseFilter::~NoiseFilter()
{
	stopWorkers();
	Logger::get().write("NoiseFilter is finished");
}

using vec = std::vector<float>;
std::future<vec> NoiseFilter::submit(std::function<vec(vec&, int, int)> task, vec &signal, int begin, int end)
{
	std::future<vec> future;
	{
		std::lock_guard lock(m_taskMutex);

		auto shared = std::make_shared<std::packaged_task<vec(vec&, int, int)>> (std::move(task));
		future = shared->get_future();

		auto lambda = [shared, &signal, begin, end] () {(*shared)(signal, begin, end);};
		m_tasks.push(std::move(lambda));
	}
	m_cv.notify_one();
	return future;
}

void NoiseFilter::printFilteredSignal()
{
	for (std::future<std::vector<float>> &res : m_results)
	{
		std::vector<float> v = res.get();
		for (const auto &elem : v)
			std::cout << std::fixed << std::setprecision(2) << elem << "  ";
	}
	std::cout << "\n";
}

void NoiseFilter::dividingSignal()
{
	try
	{
		while (true)
		{
			std::unique_lock lock(rawMutex);
			rawCV.wait(lock);

			copiedSignal = rawSignal;
			lock.unlock();

			printRawSignal();

			for (int i = 0; i < m_numThreads; ++i)
				m_results[i] = submit(algorythm, copiedSignal, i * 1000, (i * 1000) + 1000);

			printFilteredSignal();

			if (m_isDone.load())
			{
				m_stopWorkers.store(true);
				m_cv.notify_all();
				break;
			}
		}
	}
	catch (const std::runtime_error &e)
	{
		std::cout << e.what() << "\n";
	}
	Logger::get().write("Stopped recieving signal");
}

void NoiseFilter::worker()
{
	while(true)
	{
		std::unique_lock lock(m_taskMutex);

		m_cv.wait(lock, [&](){return (!m_tasks.empty() or m_stopWorkers.load());});

		if (m_tasks.empty() and m_stopWorkers.load())
			break;

		auto task = std::move(m_tasks.front());
		m_tasks.pop();
		lock.unlock();

		task();
	}
}


void NoiseFilter::stopWorkers()
{
	m_isDone.store(true, std::memory_order_relaxed);

	Logger::get().write("Waiting for workers");

	for (std::thread &th : m_threads)
	{
		if (th.joinable())
			th.join();
	}

	if (m_interruptionThread.joinable())
		m_interruptionThread.join();
}

void NoiseFilter::printRawSignal()
{
	std::cout << "\n";
	for (int i = 0; i < copiedSignal.size(); i++)
		std::cout << std::fixed << std::setprecision(2) << rawSignal[i] << "  ";
	std::cout << "\n";
}

