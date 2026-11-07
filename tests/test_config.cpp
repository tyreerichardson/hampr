#include <cassert>
#include <string>
#include <iostream>
#include <filesystem>

#include "config.hpp"

int main(int argc, char* argv[]) {
	if (argc < 2) {
        std::cerr << "No test specified\n";
        return 1;
    }
    std::cout << "Running configuration tests..." << std::endl;
	//sets the directory to where the config.hampr is located
    std::filesystem::current_path("/Users/batman/Documents/github/hampr/tests/fixtures/");	
	std::string testName = argv[1];
	
	if(testName == "valid") {
		auto task_ = findTask(std::filesystem::current_path()/"valid", "build");

		assert(task_ != std::nullopt);
	} else if(testName == "invalid") {
		auto task_ = findTask(std::filesystem::current_path()/"valid", "invalid");

		assert(task_ == std::nullopt);
	} else if(testName == "missing") {
		auto task_ = findTask(std::filesystem::current_path()/"missing", "build");

		assert(task_ == std::nullopt);
	} else {
		std::cerr << "Unknown test\n";
		return 1;
	}

	return 0;
}
