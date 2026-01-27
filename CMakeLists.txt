cmake_minimum_required(VERSION 4.2.1) # latest
project(signalflow VERSION 0.1.0 LANGUAGES C CXX)

# Target C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define the library 
add_library(signalflow_lib STATIC # ensures library files are self-contained
    src/buffer.cpp
    third_party/kiss_fft.c
    third_party/kiss_fftr.c
)

# Set include directories
target_include_directories(signalflow_lib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/third_party> 
)



# Add compiler warnings and treat warnings as errors
target_compile_options(signalflow_lib PRIVATE -Wall -Wextra -Wpedantic -Werror)

add_subdirectory(tests)