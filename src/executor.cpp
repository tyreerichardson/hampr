#include "executor.hpp"
#include <iostream>


using namespace std;

int executeTask(const Task& task) {
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

    //TODO: Add WIFSIGNALED(result) for terminaton error catching
    return exitCode;
}
