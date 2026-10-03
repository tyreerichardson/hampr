#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <cstdlib>

using namespace std;

int main(int argc, char* argv[]) {
	if(argc < 2) {
		cout << "Error: no task specified" << endl;
		return 1;
	}
	
	const string CONFIG_FILE = "config.hampr";

	string command = argv[1];
	
	auto target_file = filesystem::current_path() / CONFIG_FILE;
	if(filesystem::exists(target_file) && filesystem::is_regular_file(target_file)) {
		ifstream file(target_file);
		if(!file.is_open()) {
			cout << "Failed to open " << CONFIG_FILE << endl;
			return 1;
		}
	
		cout << CONFIG_FILE << " found!" << endl;

		string line;
		bool hasAction = false;
		while(getline(file, line)) {
			if(line.empty()) continue;
			auto pos = line.find("=");
			string action = "";
			if(pos == string::npos) {
				cout << "Error: Invalid config.hampr entry, missing '='" << endl;
				return 1;
			}

			action = line.substr(0,pos);
			if(action == command) {
				hasAction = true;
				string cmd = line.substr(pos+1);
				cout << "Command: " << command << endl;
				cout << "Action: " << cmd << endl;
				std::system(cmd.c_str());
				break;	
			}
		}					

		if(!hasAction) {
			cout << "Error: hampr command " << command << " was not found in " << CONFIG_FILE << endl;
			return 1;
		}
	} else {
		cout << "Error: Failed to locate " << CONFIG_FILE << " in the current directory." << endl;
		return 1;
	}

	return 0;
}
