#pragma once

#include "task.hpp"

#include <queue>
#include <atomic>
#include <mutex>
#include <condition_variable>

struct DaemonState {
	std::queue<Task> taskQueue;

	std::mutex queueMutex;
	std::condition_variable queueCV;

	std::atomic<bool> workerBusy{false};

	bool stopping = false;
};
