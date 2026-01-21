cmake_minimum_required(VERSION 4.2.1) # latest
project(signalflow VERSION 0.1.0 LANGUAGES CXX)

# Target C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define the library 
add_library(signalflow_lib STATIC # ensures library files are self-contained
    src/buffer.cpp
)

# Set include directories
target_include_directories(signalflow_lib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# Add compiler warnings and treat warnings as errors
target_compile_options(signalflow_lib PRIVATE -Wall -Wextra -Wpedantic -Werror)

add_subdirectory(tests)