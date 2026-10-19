#include "executor.hpp"
#include "task_request.hpp"
#include "config.hpp"
#include "ipc.hpp"

#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstdio>
#include <string>

using namespace std;

int main() {
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

	cout << "Server listening at " << addr.sun_path << endl;

	while(true) {
		cout << "Wating for client..." << endl;
		
	    int clientFd = accept(serverFd, nullptr, nullptr);

		if(clientFd == -1) {
			perror("accept failed");
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
		
	    std::cout << "Name: " << request.name << '\n';
   		std::cout << "Path: " << request.projectPath << '\n';

		string response;

		if(!sendMessage(clientFd, "Task Received")) {
			perror("Failed to send message to client");
			close(clientFd);
			return 1;
		}

		close(clientFd);
	}

	cout << "Socket created successfully!" << endl;
	cout << "File descriptor: " << serverFd << endl;

	close(serverFd);
	unlink(addr.sun_path);
    return 0;
}
