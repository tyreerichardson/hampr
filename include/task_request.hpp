#pragma once

#include <string>
#include <filesystem>

struct TaskRequest {
    std::string name;
    std::filesystem::path projectPath;

    std::string serialize() const;

    static TaskRequest deserialize(const std::string& data);
};
