#include "config.hpp"
#include "task.hpp"

#include <fstream>
#include <iostream>

using namespace std;

const string CONFIG_FILE = "config.hampr";

std::optional<Task> findTask(
    const std::filesystem::path& projectPath,
    const std::string& taskName
) {
    std::filesystem::path configPath =
        projectPath / CONFIG_FILE;
    
	std::ifstream config(configPath);

    if (!config.is_open()) {
        std::cerr << "Failed to open: " << configPath << '\n';
        return std::nullopt;
    }

    // Move your existing configuration parsing
    // and task matching logic here.	
	string line;
	while(getline(config, line)) {
		if(line.empty()) continue;
		auto pos = line.find("=");
		string action = "";
		if(pos == string::npos) {
			cout << "Error: Invalid config.hampr entry, missing '='" << endl;
			return nullopt;
		}

		action = line.substr(0,pos);
		if(action == taskName) {
			string cmd = line.substr(pos+1);
			Task task = {taskName, cmd};
			return task;
		}
	}					

	cout << "Error: hampr command " << taskName << " was not found in " << CONFIG_FILE << endl;
	return nullopt;
}
