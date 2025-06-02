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
        websocket
        VERSION 1.0.0
        DESCRIPTION "Websocket"
        LANGUAGES CXX
)

# Set C++ standards
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set build type
set(CMAKE_BUILD_TYPE Debug)

set(BWSS_SRC_DIR ${CMAKE_SOURCE_DIR})
set(BWSS_PROJECT_NAME ${PROJECT_NAME})

add_subdirectory(src)

# Include headers in the include directory
target_include_directories(websocket PRIVATE ${CMAKE_SOURCE_DIR}/src/include)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBURING REQUIRED liburing-ffi)

target_include_directories(websocket PRIVATE ${LIBURING_INCLUDE_DIRS})
target_link_libraries(websocket PRIVATE ${LIBURING_LIBRARIES})

#
# Find & link OpenSSL
# find_package(OpenSSL REQUIRED)
# target_link_libraries(websocket PUBLIC OpenSSL::SSL OpenSSL::Crypto)
# target_include_directories(websocket PUBLIC ${OPENSSL_INCLUDE_DIR})
#