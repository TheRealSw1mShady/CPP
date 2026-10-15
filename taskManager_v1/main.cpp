#include <iostream>
#include <vector>
#include <thread>
#include <queue>
#include <condition_variable>
#include <barrier>
#include <mutex>
#include <chrono>

#include "functions.h"

class TaskManager
{
	enum class TYPE
	{
		ADD = 1,
		MULTYPLY = 2,
		SLEEP = 3,
		DIVIDE = 4,
	};

	struct task
	{
		int id;
		TYPE type;
		std::string data;
	};

	struct result
	{
		float value;
		bool result;
	};

	int totalTasks;
	int bufferTasks;
	bool isFinished = false;
	int processedTasks = 0;
	int totalWorkers = 2;

	std::queue<task> tasks;
	std::condition_variable cv, cvTask;
	std::mutex taskMutex, queueMutex;
	std::vector<result> results;
	std::vector<std::thread> threads;

	void initWorker(int id)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(getRandomNumber(1,6) * 200));
		Logger::get().log("thread " + std::to_string(id) + " [initialized]");
	}

	void producer(std::barrier<> &barrier)
	{
		Logger::get().log("producer [waiting]");
		barrier.arrive_and_wait();
		Logger::get().log("producer [started]");

		for (int i = 0; i < totalTasks; ++i)
		{
			task local;
			local.id = i;
			local.type = static_cast<TYPE> (getRandomNumber(1,4));
			local.data = std::to_string(getRandomNumber(0, 100));

			{
				std::unique_lock lock(queueMutex);
				cvTask.wait(lock, [this](){return tasks.size() < bufferTasks;});
				tasks.push(std::move(local));
			}
			cv.notify_one();
		}
		queueMutex.lock();
		isFinished = true;
		queueMutex.unlock();

		cv.notify_all();
		Logger::get().log("producer [finished]");
	}

	void worker(int id, std::barrier<> &barrier)
	{
		initWorker(id);
		barrier.arrive_and_wait();

		while(true)
		{
			std::unique_lock lock(queueMutex);
			cv.wait(lock, [this](){return (!tasks.empty() or isFinished);});

			if (isFinished and tasks.empty())
				break;

			task local = tasks.front();
			tasks.pop();
			lock.unlock();

			cvTask.notify_one();

			result localRes;
			switch (local.type)
			{
			case TYPE::ADD:
				localRes.value = std::stof(local.data) + 10;
				localRes.result = 1;
				break;
			case TYPE::MULTYPLY:
				localRes.value = std::stof(local.data) * 2;
				localRes.result = 1;
				break;
			case TYPE::SLEEP:
				std::this_thread::sleep_for(std::chrono::microseconds(100));
				localRes.value = std::stof(local.data);
				localRes.result = 1;
				break;
			case TYPE::DIVIDE:
			{
				localRes.result = 0;
				localRes.value = 0;
				int value = std::stoi(local.data);

				if (value%10 != 0)
				{
					localRes.value = (value/10.0)/(value%10);
					localRes.result = 1;
				}
				break;
			}
			default:
				localRes.result = 0;
				localRes.value = 0;
			}
			results[local.id] = localRes;

			std::lock_guard lk(taskMutex);
			processedTasks++;
		}
		Logger::get().log("thread " + std::to_string(id) + " [finished]");
	}

	void concurrencyCheck()
	{
		totalWorkers = std::thread::hardware_concurrency();
		if (totalWorkers > 8)
			totalWorkers = 8;
		else if (totalWorkers == 0)
			totalWorkers = 1;
	}

public:
	TaskManager(int totalTasks = 100000, int bufferTasks = 10000) :
		totalTasks(totalTasks), bufferTasks(bufferTasks)
	{
		if (totalTasks <= 0) throw std::invalid_argument("total tasks should be positive number");
		if (bufferTasks <= 0) throw std::invalid_argument("buffer tasks should be positive number");
		results.resize(totalTasks);
	}

	void start()
	{
		concurrencyCheck();

		Timer timer;
		timer.start();

		std::barrier barrier(totalWorkers + 1);

		for (int i = 0; i < totalWorkers; ++i)
			threads.emplace_back(&TaskManager::worker, this, i + 1, std::ref(barrier));

		std::thread producerTh(&TaskManager::producer, this, std::ref(barrier));

		producerTh.join();
		for (auto& th : threads)
			th.join();

		timer.stop();

		std::cout << "\nTotal tasks: " << totalTasks << std::endl;
		std::cout << "Processed: " << processedTasks << std::endl;

		int successful(0);
		for (const auto& res : results)
			if (res.result) successful++;
		std::cout << "Successful: " << successful << std::endl;
		std::cout << "Failed: " << totalTasks - successful << std::endl;
	}
};

int main()
{
	TaskManager manager;
	manager.start();

	return 0;
}
