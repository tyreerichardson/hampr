#include "executor.hpp"

#include <iostream>
#include <chrono>

using namespace std;

int executeTask(const Task& task) {
	string command = "cd " + task.projectPath.string() + " && " + task.cmd;
	auto start = std::chrono::steady_clock::now();
	int result = std::system(command.c_str());
	auto end = std::chrono::steady_clock::now();

	auto seconds =
    std::chrono::duration_cast<std::chrono::seconds>(
        end - start
    ).count();

	std::cout << "[EXECUTOR] Duration: " << seconds << " seconds\n";
    
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

    //TODO: Add WIFSIGNALED(result) for terminaton error catching
    return exitCode;
}
