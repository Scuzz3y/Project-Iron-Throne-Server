cmake_minimum_required(VERSION 3.6)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -std=c++11)

set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})

set(IRON_THRONE_INSTALL_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(IRON_THRONE_INSTALL_BIN_DIR ${PROJECT_SOURCE_DIR}/bin)
set(IRON_THRONE_MODULE_DIR ${PROJECT_SOURCE_DIR}/src/modules)

# Individual Module Include Dirs
set(BLACKICE_HEADER_DIR ${IRON_THRONE_MODULE_DIR}/blackIce)

include_directories(${IRON_THRONE_INSTALL_INCLUDE_DIR})
include_directories(${BLACKICE_HEADER_DIR})

add_subdirectory(src)