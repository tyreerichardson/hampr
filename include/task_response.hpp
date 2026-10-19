#pragma once

#include <string>
#include <filesystem>

struct TaskResponse {
	int queue_id;
    std::string name;
    std::filesystem::path projectPath;

    std::string serialize() const;

    static TaskResponse deserialize(const std::string& data);
};
