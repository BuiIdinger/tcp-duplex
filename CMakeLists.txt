#
# Copyright 2025-2025 Shayden Seymour.
#
# CMakeLists.txt
#

cmake_minimum_required(VERSION 3.10)

# Allow targets to be modified from outside
cmake_policy(SET CMP0079 NEW)

# Set the project name
project(
        bwss
        VERSION 1.0.0
        DESCRIPTION "Buildinger WebSocket."
        LANGUAGES CXX
)

# Set C++ standards
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set build type
set(CMAKE_BUILD_TYPE Debug)

set(BWSS_SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(BWSS_PROJECT_NAME ${PROJECT_NAME})

add_subdirectory(src)

# Include headers in the include directory
target_include_directories(${BWSS_PROJECT_NAME} PRIVATE ${BWSS_SRC_DIR}/include)

# Find PkgConfig
find_package(PkgConfig REQUIRED)

# Find and link liburing
pkg_check_modules(LIBURING REQUIRED liburing-ffi)
target_link_libraries(${BWSS_PROJECT_NAME} PRIVATE ${LIBURING_LIBRARIES})
target_include_directories(${BWSS_PROJECT_NAME} PRIVATE ${LIBURING_INCLUDE_DIR})

# Find and link BoringSSL
find_package(OpenSSL REQUIRED)
target_link_libraries(${BWSS_PROJECT_NAME} PRIVATE OpenSSL::SSL OpenSSL::Crypto)
target_include_directories(${BWSS_PROJECT_NAME} PRIVATE ${OpenSSL_INCLUDE_DIR})