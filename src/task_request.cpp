#include "task_request.hpp"
#include <sstream>

std::string TaskRequest::serialize() const {
    std::string typeString =
        type == RequestType::Task ? "TASK" : "DAEMON";

    return typeString + "\n"
         + name + "\n"
         + projectPath.string();	
}

TaskRequest TaskRequest::deserialize(const std::string& data) {
	std::istringstream stream(data);

	TaskRequest request;
	
	std::string requestType;
	std::getline(stream, requestType);
	request.type = requestType == "TASK" ? RequestType::Task : RequestType::DaemonControl;

	std::getline(stream, request.name);

	std::string path;
	std::getline(stream, path);
	
	request.projectPath = path;

	return request;
}
