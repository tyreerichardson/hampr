#pragma once

#include "task_request.hpp"
#include "daemon_state.hpp"

#include <queue>
#include <atomic>
#include <mutex>

enum class ControlResult {
	Continue,
	Shutdown
};

ControlResult handleControl(const TaskRequest& request, int clientFd, DaemonState& state);
ControlResult handleDaemonControl(const TaskRequest& request, int clientFd, DaemonState& state);
ControlResult handleQueueControl(const TaskRequest& request, int clientFd, DaemonState& state);
