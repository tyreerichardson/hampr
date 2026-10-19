#include <unistd.h>
#include <cerrno>
#include <cstddef>
#include <string>

bool writeAll(int fd, const void* data, std::size_t size) {
    const char* bytes = static_cast<const char*>(data);
    std::size_t totalWritten = 0;

    while (totalWritten < size) {
 		ssize_t written = write(
    		fd,
    		bytes + totalWritten,
    		size - totalWritten
		);
		
		if(written == -1) {
			if(errno == EINTR) continue;
			 else return false;
		} else if(written == 0) {
			return false;
		}

		totalWritten += static_cast<std::size_t>(written);
    }

    return true;
}

bool readAll(int fd, void* data, std::size_t size) {
    char* bytes = static_cast<char*>(data);
    std::size_t totalRead = 0;

    while (totalRead < size) {
    	ssize_t end_read = read(
			fd,
			bytes + totalRead,
			size - totalRead
		);

		if(end_read == -1) {
			if(errno == EINTR) continue;
			else return false;
		} else if(end_read == 0) {
			return false;
		}
		totalRead += static_cast<std::size_t>(end_read);
	}
    return true;
}

bool receiveMessage(int fd, std::string& message) {
    constexpr std::size_t MAX_MESSAGE_SIZE = 1024 * 1024;
    uint32_t networkLength = 0;

    // Read the 4-byte length prefix.
    if (!readAll(fd, &networkLength, sizeof(networkLength))) {
        return false;
    }

    // Convert network byte order to host byte order.
    uint32_t length = ntohl(networkLength);

    // Validate the received length.
    if (length > MAX_MESSAGE_SIZE) {
        return false;
    }

    // Allocate enough space for the payload.
    message.resize(length);

    // Read the complete payload.
    return readAll(fd, message.data(), length);
}

bool sendMessage(int fd, const std::string& message) {
    constexpr std::size_t MAX_MESSAGE_SIZE = 1024 * 1024;

    if (message.size() > MAX_MESSAGE_SIZE) {
        return false;
    }

    uint32_t length = static_cast<uint32_t>(message.size());
    uint32_t networkLength = htonl(length);
	
	bool sent = writeAll(fd, &networkLength, sizeof(networkLength));
	if(!sent) return false;
	return writeAll(fd, message.data(), message.size());
}

