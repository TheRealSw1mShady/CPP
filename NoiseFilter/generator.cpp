#include "generator.h"

Generator::~Generator()
{
	m_isStop.store(true, std::memory_order_relaxed);

	if (m_genThread.joinable())
		m_genThread.join();
	logger.write("Generator is finished");
}

void Generator::start()
{
	m_isStop.store(false, std::memory_order_relaxed);
	m_genThread = std::thread(&Generator::generateSignal, this);
}

void Generator::generateSignal()
{
	Random rand;
	while (!m_isStop.load(std::memory_order_relaxed))
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(20));

		std::unique_lock lock(rawMutex);
		for (int i = 0; i < rawSignal.size(); i++)
			rawSignal[i] = (sin(i/15.0 + rand.getRandomNumber(-20, 20)/100.0));
		lock.unlock();

		cv.notify_one();
	}
	Logger::get().write("Stopped generating");
}

void Generator::printRawSignal()
{
	std::lock_guard lock(rawMutex);

	for (int i = 0; i < rawSignal.size(); i++)
		std::cout << std::fixed << std::setprecision(2) << rawSignal[i] << "  ";
	std::cout << "\n";
}
