#include "client.hpp"
#include "task_request.hpp"
#include "request_type.hpp"
#include "ipc.hpp"

#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstdio>
#include <cerrno>

using namespace std;

bool sendCommand(const std::string& command, RequestType type) {
	TaskRequest request = {
		type,
		command,
		std::filesystem::current_path()
	};

	int sockFd = socket(AF_UNIX, SOCK_STREAM, 0);
	if(sockFd == -1) {
		cout << "socket failed to initialize" << endl;
		return 1;
	}

	int noSigPipe = 1;

    if(setsockopt(
    		sockFd,
        	SOL_SOCKET,
       		SO_NOSIGPIPE,
        	&noSigPipe,
        	sizeof(noSigPipe)
    	) == -1) {
		perror("setsockopt");
		close(sockFd);
		return 1;
	}	
	
	sockaddr_un addr{};
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, "/tmp/hampr.sock", sizeof(addr.sun_path) -1);
	
	int conn = ::connect(sockFd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un));
	if(conn == -1) {
		int error = errno;
		close(sockFd);
		
		if(type == RequestType::DaemonControl &&
			command == "status" && 
			(error == ENOENT || error == ECONNREFUSED)) {
			std::cout << "Daemon is not running" << endl;
			return true;
		}
		
		errno = error;
		perror("connect failed");
		return 1;
	}

	cout << "Connected to Hampr server!" << endl;

	string message = request.serialize();

	if(!sendMessage(sockFd, message)) {
		perror("Failed to send task request");
		close(sockFd);
		return 1;
	}

	string response;

	if(!receiveMessage(sockFd, response)) {
		perror("Failed to receive daemon response");
		close(sockFd);
		return 1;
	}

	cout << response << endl;

	close(sockFd);
	
	return 0;
}

