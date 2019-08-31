cmake_minimum_required(VERSION 3.6)
project(BlackIce-TestClient)

set(SOURCE_FILES main.cpp)

add_executable(blackice-client ${SOURCE_FILES})

install(TARGETS blackice-client DESTINATION ${IRON_THRONE_INSTALL_BIN_DIR})