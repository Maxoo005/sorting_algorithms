# Which version is required.
cmake_minimum_required(VERSION 3.23)

# Set project name
project(useHelloParams)

# Set C++ parameters.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror")
set(CMAKE_CXX_COMPILER "g++")

# What files should be compiled in my executable.
add_executable(useHelloParams main.cpp)

# Add an include directory to above exec.
target_include_directories(useHelloParams PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Linking libhelloparams to above exec.
target_link_libraries(useHelloParams ${CMAKE_CURRENT_SOURCE_DIR}/lib/libhelloparams.so)
