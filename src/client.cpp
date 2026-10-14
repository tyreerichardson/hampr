#include "client.hpp"
#include "task_request.hpp"

#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstdio>

using namespace std;

bool sendCommand(const std::string& command) {
	TaskRequest request = {
		command,
		std::filesystem::current_path()
	};

	int sockFd = socket(AF_UNIX, SOCK_STREAM, 0);
	if(sockFd == -1) {
		cout << "socket failed to initialize" << endl;
		return 1;
	}	
	sockaddr_un addr{};
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, "/tmp/hampr.sock", sizeof(addr.sun_path) -1);
//	unlink(addr.sun_path);
	
	int conn = ::connect(sockFd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un));

	if(conn == -1) {
		perror("connect failed");
		close(sockFd);
		return 1;
	}

	cout << "Connected to Hampr server!" << endl;

	string message = request.serialize();

	ssize_t bytesWritten = write(
    	sockFd,
    	message.c_str(),
    	message.size()
	);

	char response[100]{};
	ssize_t bytesRead = read(sockFd, response, sizeof(response) -1);

	if(bytesRead < 0) {

	} else if(bytesRead == 0){
	
	} else {
		response[bytesRead] = '\0';
		cout << "Server: " << response << endl;
	}

	if(bytesWritten > 0) {
		cout << "bytes written" << endl;
	}
	
	close(sockFd);
	
	return 0;
}

