cmake_minimum_required(VERSION 3.6)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -std=c++11)

set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})

set(SERVER_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(SERVER_BIN_DIR ${PROJECT_SOURCE_DIR}/bin)

include_directories(${SERVER_INCLUDE_DIR})

add_subdirectory(src)