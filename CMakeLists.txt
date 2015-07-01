cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)

project(ax)

# Set compiler options.
add_definitions(
    -std=c++14
)

# Parse build type.
set(CMAKE_BUILD_TYPE Debug)
if(${CMAKE_CURRENT_BINARY_DIR} MATCHES ".*release.*")
    set(CMAKE_BUILD_TYPE Release)
elseif(${CMAKE_CURRENT_BINARY_DIR} MATCHES ".*debug.*")
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_CXX_FLAGS_DEBUG
    "${CMAKE_CXX_FLAGS_DEBUG} \
    -Wall -Wextra -Werror -pedantic -Wsign-promo \
    -Wcast-qual -Wfloat-equal -Wcast-align -g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -O3")

# Set output directories.
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/bin)

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

set(AX_INCLUDE_PATH ${CMAKE_SOURCE_DIR}/include)

add_subdirectory(${CMAKE_SOURCE_DIR}/test)
