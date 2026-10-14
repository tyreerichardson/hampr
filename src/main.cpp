#include "client.hpp"
#include "task.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <optional>
#include <queue>
#include <filesystem>
#include <cstdlib>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstdio>

using namespace std;

bool validateCLI(int argc){
	if(argc < 2) {
		cout << "Error: no task specified" << endl;
		return false;
	}
	return true;
}

int main(int argc, char* argv[]) {
	if(!validateCLI(argc)) return 1;
	
	queue<Task> taskQueue;

	string command = argv[1];
	sendCommand(command);
	return 0;
}
