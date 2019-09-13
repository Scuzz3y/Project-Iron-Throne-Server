cmake_minimum_required(VERSION 3.6)
project(ArsenalHttp)

set(SOURCE_FILES arsenalHttp.h arsenalHttp.cpp)

add_library(arsenalHttp STATIC ${SOURCE_FILES})
