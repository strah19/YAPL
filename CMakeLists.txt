cmake_minimum_required(VERSION 3.10)

project(YAPL VERSION 1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

configure_file(include/config.h. include/config.h)

include_directories(include)
file(GLOB SOURCES "src/*.cpp")
add_executable(YAPL ${SOURCES})

target_include_directories(YAPL PUBLIC "${PROJECT_BINARY_DIR}/include")
