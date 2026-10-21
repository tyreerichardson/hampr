#pragma once

#include "request_type.hpp"

#include <string>
#include <filesystem>

struct TaskRequest {
	RequestType type;
    std::string name;
    std::filesystem::path projectPath;

    std::string serialize() const;

    static TaskRequest deserialize(const std::string& data);
};
