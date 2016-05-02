cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)

project(ash)

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

set(CLANG_ADDRESS_SANITIZER_FLAGS
    "-fsanitize=address -fno-omit-frame-pointer")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG ${CMAKE_EXE_LINKER_FLAGS_DEBUG})
if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
    set(CMAKE_EXE_LINKER_FLAGS
        "${CMAKE_EXE_LINKER_FLAGS} \
        ${CMAKE_EXE_LINKER_FLAGS_DEBUG}")
endif()

set(CXX_DEBUG_FLAGS "-g -O0")
set(CXX_WARN_FLAGS
    "${CXX_WARN_FLAGS} \
    -Wall -Wextra -Werror -pedantic -Wsign-promo \
    -Wcast-qual -Wfloat-equal -Wcast-align")
set(CMAKE_CXX_FLAGS_DEBUG
    "${CMAKE_CXX_FLAGS_DEBUG} \
    ${CXX_DEBUG_FLAGS} ${CXX_WARN_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} \
    -march=native -Wall -O3 -flto")

# Set output directories.
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/bin)

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

set(ASH_INCLUDE_PATH ${CMAKE_SOURCE_DIR}/include)

add_subdirectory(${CMAKE_SOURCE_DIR}/test)
