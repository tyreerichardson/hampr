#include "task_request.hpp"
#include <sstream>

std::string TaskRequest::serialize() const {
	return name + "\n" + projectPath.string() + "\n";
}

TaskRequest TaskRequest::deserialize(const std::string& data) {
	std::istringstream stream(data);

	TaskRequest request;

	std::getline(stream, request.name);

	std::string path;
	std::getline(stream, path);
	
	request.projectPath = path;

	return request;
}
