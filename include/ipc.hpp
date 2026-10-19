#include <cstdint>
#include <arpa/inet.h>
#include <string>


bool sendMessage(int fd, const std::string& message);

bool receiveMessage(int fd, std::string& message);
