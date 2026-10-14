#pragma once

#include "task.hpp"
#include <filesystem>
#include <optional>
#include <string>

std::optional<Task> findTask(
    const std::filesystem::path& projectPath,
    const std::string& taskName
);
