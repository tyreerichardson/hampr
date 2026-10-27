#include "control.hpp"
#include "config.hpp"
#include "ipc.hpp"
#include "task_request.hpp"

#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstdio>

using namespace std;

ControlResult handleDaemonControl(
	const TaskRequest& request,
	int clientFd,
	DaemonState& state
) {
	if(request.name == "stop") {
        sendMessage(clientFd, "Daemon shutting down");
        close(clientFd);
    	return ControlResult::Shutdown;
    } else if(request.name == "status") {
		if(state.workerBusy.load() && state.taskQueue.empty()) sendMessage(clientFd, "Worker is executing the last task");
     	else if(state.workerBusy.load() && !state.taskQueue.empty()) sendMessage(clientFd, "Worker is executing, with " + to_string(state.taskQueue.size()) + " tasks waiting");
     	else if(!state.workerBusy.load() && !state.taskQueue.empty()) sendMessage(clientFd, "Worker is between tasks or about to begin execution");
     	else if(!state.workerBusy.load() && state.taskQueue.empty()) sendMessage(clientFd, "Worker is idle");
        close(clientFd);
        return ControlResult::Continue;
    } else {
        cout << "Error: unsupported daemon command" << endl;
        close(clientFd);
	    return ControlResult::Continue;
	}
}

ControlResult handleQueueControl(
	const TaskRequest& request,
	int clientFd,
	DaemonState& state
) {
	string message = "Queue Status\n-----------\n";
	message += (state.workerBusy.load()) ? "Busy\n" : "Idle\n";
	message += to_string(state.taskQueue.size());
	
	if(request.name == "status") {
        sendMessage(clientFd, message);
        close(clientFd);
        return ControlResult::Continue;
    } else {
        cout << "Error: unsupported daemon command" << endl;
        close(clientFd);
        return ControlResult::Shutdown;
    }
}

ControlResult handleControl(
	const TaskRequest& request,
	int clientFd,
	DaemonState& state
) {
		if(request.name.empty() || request.projectPath.empty()) {
			if(!sendMessage(clientFd, "Task name or path is empty")) {
				perror("Task name or path is empty");
				close(clientFd);
				return ControlResult::Continue;
			}
		}
		
	std::cout << "Name: " << request.name << '\n';
   	std::cout << "Path: " << request.projectPath << '\n';

		
	string response;
		
	ControlResult result = ControlResult::Continue;
		
	if(request.type == RequestType::DaemonControl) {
		result = handleDaemonControl(request, clientFd, state);
	} else if(request.type == RequestType::QueueControl) {
		result = handleQueueControl(request, clientFd, state);
	} else {
       	std::optional<Task> task = findTask(request.projectPath, request.name);
	
 		if(!sendMessage(clientFd, "Task Received")) {
    		perror("Failed to send message to client");
        	close(clientFd);
           	return ControlResult::Shutdown;
        }

		if(task.has_value()) {
	       	task->projectPath = request.projectPath;
			{
				std::lock_guard<std::mutex> guard(state.queueMutex);
				state.taskQueue.push(*task);
			}
			state.queueCV.notify_one();
		}
	}

	return result;
};
