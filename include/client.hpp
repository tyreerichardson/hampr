#pragma once

#include "request_type.hpp"

#include <string>

bool sendCommand(const std::string& command, RequestType type);
