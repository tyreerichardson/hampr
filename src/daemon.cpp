#include "executor.hpp"
#include "task_request.hpp"
#include "request_type.hpp"
#include "config.hpp"
#include "ipc.hpp"

#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstdio>
#include <string>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

using namespace std;


int main() {

    queue<Task> taskQueue;
    std::mutex queueMutex;
    std::condition_variable queueCV;
	std::atomic<bool> workerBusy(false);
    bool stopping = false;

    int serverFd = socket(AF_UNIX, SOCK_STREAM, 0);
	if(serverFd == -1) {
		cout << "socket failed to initialize" << endl;
		return 1;	
	}

	sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, "/tmp/hampr.sock", sizeof(addr.sun_path) -1);
	unlink(addr.sun_path);		
	int bnd = ::bind(serverFd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un));
	if(bnd != -1) {
		cout << "Socket bound to " << addr.sun_path << endl;
	} else {
		perror("Failed to bind!");
		close(serverFd);
		return 1;
	}

	int lstn = ::listen(serverFd, 5);
	if(lstn == -1) {
		perror("listen failed");
		close(serverFd);
		unlink(addr.sun_path);
		return 1;
	}

	std::thread worker([&]() {
		while(true) {
	        std::unique_lock<std::mutex> lock(queueMutex);

        	queueCV.wait(lock, [&]() {
            	return !taskQueue.empty() || stopping;
        	});
			
			if (stopping && taskQueue.empty()) {
            	break;
        	}

			Task task = taskQueue.front();
			taskQueue.pop();
			
			lock.unlock();
			workerBusy.store(true);
			executeTask(task);
			workerBusy.store(false);
		}
	});
	
	cout << "Server listening at " << addr.sun_path << endl;

	while(true) {
		cout << "Wating for client..." << endl;
		
	    int clientFd = accept(serverFd, nullptr, nullptr);
		
		if(clientFd == -1) {
			perror("accept failed");
			continue;
		}

	    int noSigPipe = 1;

        if(setsockopt(
            	clientFd,
            	SOL_SOCKET,
            	SO_NOSIGPIPE,
           		&noSigPipe,
           		sizeof(noSigPipe)
        	) == -1) {
			perror("setsockopt");
			close(clientFd);
			continue;
		}		

		cout << "Client connected!" << endl;
		
		string message;

		if(!receiveMessage(clientFd, message)) {
			perror("Failed to receive client message");
			close(clientFd);
			return 1;
		}

	
		TaskRequest request = TaskRequest::deserialize(message);
		
		if(request.name.empty() || request.projectPath.empty()) {
			if(!sendMessage(clientFd, "Task name or path is empty")) {
				perror("Task name or path is empty");
				close(clientFd);
				continue;
			}
		}
		
	    std::cout << "Name: " << request.name << '\n';
   		std::cout << "Path: " << request.projectPath << '\n';

		
		string response;

		if(request.type == RequestType::DaemonControl) {
			if(request.name == "stop") {
				sendMessage(clientFd, "Daemon shutting down");
			    close(clientFd);
				break;
    		} else if(request.name == "status") {
				if(workerBusy.load() && taskQueue.empty())
					sendMessage(clientFd, "Worker is executing the last task");
				else if(workerBusy.load() && !taskQueue.empty())
					sendMessage(clientFd, "Worker is executing, with " + to_string(taskQueue.size()) + " tasks waiting");
				else if(!workerBusy.load() && !taskQueue.empty())
					sendMessage(clientFd, "Worker is between tasks or about to begin execution");
				else if(!workerBusy.load() && taskQueue.empty()) sendMessage(clientFd, "Worker is idle");
				close(clientFd);
				continue;
			} else {
				cout << "Error: unsupported daemon command" << endl;
				close(clientFd);
				continue;
			}
		} else {
        	std::optional<Task> task = findTask(request.projectPath, request.name);
		
 	       if(!sendMessage(clientFd, "Task Received")) {
    	        perror("Failed to send message to client");
        	    close(clientFd);
           	 	return 1;
        	}

			if(task.has_value()) {
	        	task->projectPath = request.projectPath;
				{
					std::lock_guard<std::mutex> guard(queueMutex);
					taskQueue.push(*task);
				}
				queueCV.notify_one();
			}
		}

		close(clientFd);
	}

	//Shutdown logic
    {
       	std::lock_guard<std::mutex> guard(queueMutex);
    	stopping = true;
    }

   	queueCV.notify_one();
	worker.join();	
	
	cout << "Socket created successfully!" << endl;
	cout << "File descriptor: " << serverFd << endl;

	close(serverFd);
	unlink(addr.sun_path);
    return 0;
}
