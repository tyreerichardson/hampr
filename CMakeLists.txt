cmake_minimum_required(VERSION 3.12)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(HamprProject CXX)

add_executable(hampr 
	src/main.cpp
	src/client.cpp
	src/task_request.cpp
	src/ipc.cpp
)

target_include_directories(hampr PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(hamprd 
	src/daemon.cpp
	src/executor.cpp
	src/task_request.cpp
	src/config.cpp
	src/ipc.cpp
)

target_include_directories(hamprd PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

set_target_properties(hampr PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)
