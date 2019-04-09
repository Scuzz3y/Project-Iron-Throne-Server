cmake_minimum_required(VERSION 3.6)
project(LightBringer)

set(SOURCE_FILES main.cpp lightBringer.cpp)

# Put Puppet Modules Here
add_subdirectory(${IRON_THRONE_PUPPET_MODULE_DIR}/blackIce)

# Put Master Modules Here
add_subdirectory(${IRON_THRONE_MASTER_MODULE_DIR}/arsenalHttp)

add_executable(iron-throne ${SOURCE_FILES})

target_link_libraries(iron-throne blackIce arsenalHttp)

install(TARGETS iron-throne DESTINATION ${IRON_THRONE_INSTALL_BIN_DIR})