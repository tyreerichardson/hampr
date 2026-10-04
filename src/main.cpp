#include <iostream>
#include <fstream>
#include <string>
#include <optional>
#include <queue>
#include <filesystem>
#include <cstdlib>
#include <sys/wait.h>

using namespace std;

const string CONFIG_FILE = "config.hampr";

struct Task {
	string name;
	string cmd;
};

bool validateCLI(int argc){
	if(argc < 2) {
		cout << "Error: no task specified" << endl;
		return false;
	}
	return true;
}

optional<Task> findTask(string command) {
	auto target_file = filesystem::current_path() / CONFIG_FILE;
	if(filesystem::exists(target_file) && filesystem::is_regular_file(target_file)) {
		ifstream file(target_file);
		if(!file.is_open()) {
			cout << "Failed to open " << CONFIG_FILE << endl;
			return nullopt;
		}
	
		cout << CONFIG_FILE << " found!" << endl;

		string line;
		while(getline(file, line)) {
			if(line.empty()) continue;
			auto pos = line.find("=");
			string action = "";
			if(pos == string::npos) {
				cout << "Error: Invalid config.hampr entry, missing '='" << endl;
				return nullopt;
			}

			action = line.substr(0,pos);
			if(action == command) {
				string cmd = line.substr(pos+1);
				Task task = {command, cmd};
				return task;
			}
		}					
	} else {
		cout << "Error: Failed to locate " << CONFIG_FILE << " in the current directory." << endl;
		return nullopt;
	}

	cout << "Error: hampr command " << command << " was not found in " << CONFIG_FILE << endl;
	return nullopt;
}

int executeTask(Task task) {
 	int result = std::system(task.cmd.c_str());
    int exitCode = WEXITSTATUS(result);
	int wifsignaled = WIFSIGNALED(result);
	int wtermsig = WTERMSIG(result);
	
	if(exitCode == 0 && wtermsig == 0) {
        cout << "Task '" << task.name << "' succeeded." << endl;
    } else if(exitCode !=0 && wtermsig == 0) {
        cout << "Task '" << task.name << "' failed with exit code " << exitCode << "." << endl;
	} else if(wtermsig != 0) {
		cout << "Task '" << task.name  << "' terminated by signal " << wtermsig << endl;
	}

	//TODO: Task failed/terminated abnormally.
    //TODO: Add WTERMSIG(result) and WIFSIGNALED(result) for terminaton error catching
	return exitCode;
}

int main(int argc, char* argv[]) {
	if(!validateCLI(argc)) return 1;
	
	queue<Task> taskQueue;

	string command = argv[1];
	auto task = findTask(command);
	if(task.has_value())
		taskQueue.push(task.value());

	while(!taskQueue.empty()) {
		Task currentTask = taskQueue.front();
		executeTask(currentTask);
		taskQueue.pop();
	}

	return 0;
}
