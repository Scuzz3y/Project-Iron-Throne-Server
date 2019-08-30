cmake_minimum_required(VERSION 3.6)
project(BlackIce)

set(SOURCE_FILES blackIce.h blackIce.cpp)

add_library(blackIce STATIC ${SOURCE_FILES})

install(FILES blackIce.h DESTINATION ${IRON_THRONE_INSTALL_INCLUDE_DIR})