cmake_minimum_required(VERSION 3.12)
project(HamprProject CXX)

add_executable(hampr src/main.cpp)

set_target_properties(hampr PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)
