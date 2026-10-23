#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

#include <mutex>
#include <random>

class Logger
{
	std::mutex mutex;
	Logger() = default;

public:
	Logger(const Logger &) = delete;
	Logger& operator=(const Logger &) = delete;

	static Logger& get()
	{
		static Logger log;
		return log;
	}

	void write(std::string str)
	{
		std::lock_guard lk(mutex);
		std::cout << "[logged]: " << str << "\n";
	}
};

class Random
{
public:
	int getRandomNumber(int min, int max)
	{
		thread_local std::mt19937 generator(std::random_device{}());

		std::uniform_int_distribution<int> distribution(min, max);
		return distribution(generator);
	}
};

class Timer
{
	std::mutex mutex;
	std::chrono::steady_clock::time_point begin;

public:

	void start()
	{
		begin = std::chrono::steady_clock::now();
	}

	void stop()
	{
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - begin).count();

		std::lock_guard lock(mutex);
		std::cout << "\nCompleted by " << duration/1000.f << " seconds.\n";
	}
};



#endif /* FUNCTIONS_H_ */
