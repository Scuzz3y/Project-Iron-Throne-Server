cmake_minimum_required(VERSION 3.6)
project(LightBringer)

set(SOURCE_FILES main.cpp lightBringer.cpp)

# Put Modules Here
add_subdirectory(modules/blackIce)

add_executable(iron-throne ${SOURCE_FILES})

target_link_libraries(iron-throne blackIce)

install(TARGETS iron-throne DESTINATION ${IRON_THRONE_INSTALL_BIN_DIR})