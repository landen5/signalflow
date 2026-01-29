cmake_minimum_required(VERSION 4.2.1) # latest
project(signalflow VERSION 0.1.0 LANGUAGES C CXX)

# Target C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define the library 
add_library(signalflow_lib STATIC # ensures library files are self-contained
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

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
add_subdirectory(tests)

# Demo app target
add_executable(demo src/demo/main.cpp)
target_link_libraries(demo PRIVATE signalflow_lib)
target_include_directories(demo PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party
)