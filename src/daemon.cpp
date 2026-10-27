#include "executor.hpp"
#include "task_request.hpp"
#include "request_type.hpp"
#include "config.hpp"
#include "ipc.hpp"
#include "daemon_state.hpp"
#include "control.hpp"

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
	
	DaemonState state;

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
	        std::unique_lock<std::mutex> lock(state.queueMutex);

        	state.queueCV.wait(lock, [&]() {
            	return !state.taskQueue.empty() || state.stopping;
        	});
			
			if (state.stopping && state.taskQueue.empty()) {
            	break;
        	}

			Task task = state.taskQueue.front();
			state.taskQueue.pop();
			
			lock.unlock();
			state.workerBusy.store(true);
			executeTask(task);
			state.workerBusy.store(false);
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
		
		if(request.type == RequestType::Task) {
			ControlResult result = handleControl(
				request,
				clientFd,
				state
			);

			close(clientFd);

			if(result == ControlResult::Shutdown) {
				break;
			}
		} else {
			ControlResult result = handleControl(
				request,
				clientFd,
				state
			);

			close(clientFd);

			if(result == ControlResult::Shutdown) {
				break;
			}
		}		

	}

	//Shutdown logic
    {
       	std::lock_guard<std::mutex> guard(state.queueMutex);
    	state.stopping = true;
    }

   	state.queueCV.notify_one();
	worker.join();	
	
	cout << "Socket created successfully!" << endl;
	cout << "File descriptor: " << serverFd << endl;

	close(serverFd);
	unlink(addr.sun_path);
    return 0;
}
