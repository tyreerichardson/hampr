#include "client.hpp"
#include "task.hpp"
#include "request_type.hpp"

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

	RequestType type = RequestType::Task;

	string command = argv[1];

	if(command == "daemon") {
		type = RequestType::DaemonControl;
		if(argc == 3) {
			command = argv[2];
		} else if(argc > 3) {
			cout << "Error: too many arguments" << endl;
			return 1;
		} else {
			cout << "Error: missing action" << endl;
			return 1;
		}
	}

	sendCommand(command, type);	
	return 0;
}
