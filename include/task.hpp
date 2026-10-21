
#pragma once

#include <string>
#include <filesystem>

struct Task {
	std::string name;
	std::string cmd;
	std::filesystem::path  projectPath;
};
