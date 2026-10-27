#include "task_request.hpp"
#include <sstream>
#include <map>

std::unordered_map<std::string, RequestType> stringToTypeMap = {
	{"TASK", RequestType::Task},
	{"DAEMON", RequestType::DaemonControl},
	{"QUEUE", RequestType::QueueControl}
};

std::string typeToString(RequestType type) {
    switch(type) {
        case RequestType::Task: return "TASK";
		case RequestType::DaemonControl: return "DAEMON";
		case RequestType::QueueControl: return "QUEUE";
 		default: return "TASK";
    }
}

std::string TaskRequest::serialize() const {
    return typeToString(type) + "\n"
         + name + "\n"
         + projectPath.string();	
}

TaskRequest TaskRequest::deserialize(const std::string& data) {
	std::istringstream stream(data);

	TaskRequest request;
	
	std::string requestType;
	std::getline(stream, requestType);

	request.type = stringToTypeMap[requestType];

	std::getline(stream, request.name);

	std::string path;
	std::getline(stream, path);
	
	request.projectPath = path;

	return request;
}
