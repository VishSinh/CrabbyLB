# CMake configuration for CrabbyLB
cmake_minimum_required(VERSION 3.10)
project(CrabbyLB)

# Set C++ standard to C++17 (explicitly required for thread pool & advanced features)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories for headers
include_directories(include)

# pthread for threading support
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

# Add source files
set(SOURCES
    src/main.cpp
    src/core/server.cpp
    src/core/request.cpp
    src/core/response.cpp
    src/core/thread_pool.cpp
    src/core/load_balancer.cpp
)

# Create executable
add_executable(crabbyLB ${SOURCES})

# Set output directory for binary
set_target_properties(crabbyLB PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
