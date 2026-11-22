#include <cassert>
#include <stdexcept>
#include <string>
#include <iostream>
#include <filesystem>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "ipc.hpp"
#include "client.hpp"
#include "request_type.hpp"

void fakeDaemon(const std::string& socketPath, std::mutex& mutex, std::condition_variable& cv, bool& serverReady, bool& serverFailed) {
    int serverFd = socket(AF_UNIX, SOCK_STREAM, 0);
	
    if (std::filesystem::exists(socketPath)) {
        std::cout << "Socket file exists\n";
	    unlink(socketPath.c_str());
		std::cout << "unlink(socketPath.c_str());" << std::endl;

    } else {
		std::cout << "Socket file does not exist\n";
    }
	
    sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, socketPath.c_str(), sizeof(addr.sun_path)-1);
	std::cout << "TEST" << std::endl;
	int bnd = ::bind(serverFd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un));
	std::cout << "BOUND" << std::endl;
	if(bnd != -1) {
		std::cout << "Socket bound to " << addr.sun_path << std::endl;
	} else {
		std::cerr << "Failed to bind\n";
		close(serverFd);
        throw std::runtime_error("Socket file does not exist");
	}

	int lstn = ::listen(serverFd, 5);
	if(lstn == -1) {
		std::cerr << "listen failed\n";
		close(serverFd);
		unlink(addr.sun_path);
		return;
	}

    {
       	std::lock_guard<std::mutex> lock(mutex);
    	serverFailed = true;
    }

    cv.notify_one();
	std::cout << "TEST 2" << std::endl;
    int clientFd = accept(serverFd, nullptr, nullptr);

    if (clientFd >= 0) {
        std::string request;
		if(!receiveMessage(clientFd, request)) {
			std::cerr << "Failed to receive message\n";
			close(clientFd);
			return;
		}

        // TODO: sendMessage(clientFd, "Daemon is running")
		sendMessage(clientFd, "Daemon response message");
        close(clientFd);
    }

    close(serverFd);
    unlink(socketPath.c_str());
}

int main(int argc, char* argv[]) {
	if(argc < 2) {
		std::cerr << "No test specefied\n";
		return 1;
	}
	
	std::string command = "Test Message";
	RequestType type = RequestType::Task;

	std::mutex mutex;
	std::condition_variable cv;
	bool serverReady = false;
	bool serverFailed = false;
	
	std::string testName = argv[1];
	int result = 1;

	if (testName == "valid") {
		std::thread server([&]() {
	        fakeDaemon("/tmp/fake.sock", mutex, cv, serverReady, serverFailed);
    	});

	    {
    	    std::unique_lock<std::mutex> lock(mutex);

	        cv.wait(lock, [&]() {
            	return serverReady || serverFailed;
       		});
    	}

	    result = sendCommand(command, type, "/tmp/fake.sock");
	    if (result != 0) {
        	std::cerr << "Valid command, test successful\n";
    	}

		server.join();
	} else if (testName == "invalid") {
       try {
            int result = sendCommand(
                command,
                type,
                "/tmp/invalid.sock"
            );

            // Use this if sendCommand returns an error code
			assert(result == 1);
			std::cout << result << std::endl;
            return result == 1 ? 0 : 1;

        } catch (const std::runtime_error& e) {
            // Use this if sendCommand throws on connection failure.
            std::cout << "Expected exception: "
                      << e.what() << '\n';
            return 0;
        }
	} else {
    	std::cerr << "Unknown test\n";
    	return 1;
	}


	if (testName == "valid") {
    	return result == 0 ? 0 : 1;
	}

	return result == 1 ? 0 : 1;
}
