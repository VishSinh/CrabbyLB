cmake_minimum_required(VERSION 3.10)
project(CrabbyLB)

set(CMAKE_CXX_STANDARD 17)

# Add source files
file(GLOB_RECURSE SOURCES "src/*.cpp")
include_directories(include)

# Create executable
add_executable(crabbyLB ${SOURCES})

# Output binary to bin/
set_target_properties(crabbyLB PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
