cmake_minimum_required(VERSION 3.6)
project(LightBringer)

set(SOURCE_FILES main.cpp lightBringer.cpp)

add_subdirectory(modules/blackIce)

add_executable(main ${SOURCE_FILES})

target_link_libraries(main blackIce)

install(TARGETS main DESTINATION ${SERVER_BIN_DIR})