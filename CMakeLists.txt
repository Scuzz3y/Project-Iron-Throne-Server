cmake_minimum_required(VERSION 3.6)

set(CMAKE_CXX_STANDARD 17)
set(GCC_GENERAL_COMPILATION_FLAGS "-std=c++17 -g -pthread")
set(GCC_COVERAGE_COMPILE_FLAGS "-Wall -pedantic \
-pedantic-errors -Wextra -Wcast-align \
-Wcast-qual -Wconversion \
-Wdisabled-optimization \
-Werror -Wformat=2 \
-Wformat-nonliteral -Wformat-security \
-Wformat-y2k \
-Wimport -Winit-self -Winline -Winvalid-pch \
-Wmissing-field-initializers -Wmissing-format-attribute \
-Wmissing-include-dirs -Wmissing-noreturn \
-Wpacked -Wpointer-arith \
-Wredundant-decls \
-Wshadow -Wstack-protector \
-Wstrict-aliasing=2 -Wswitch-default \
-Wswitch-enum \
-Wunreachable-code -Wunused \
-Wunused-parameter \
-Wvariadic-macros \
-Wwrite-strings")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS} ${GCC_GENERAL_COMPILATION_FLAGS}" )

set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})

set(IRON_THRONE_INSTALL_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(IRON_THRONE_INSTALL_BIN_DIR ${PROJECT_SOURCE_DIR}/bin)
set(IRON_THRONE_LIBS_DIR ${PROJECT_SOURCE_DIR}/libs)
set(IRON_THRONE_MODULE_DIR ${PROJECT_SOURCE_DIR}/src/modules)

# Individual Module Include Dirs
set(BLACKICE_HEADER_DIR ${IRON_THRONE_MODULE_DIR}/blackIce)

include_directories(${IRON_THRONE_INSTALL_INCLUDE_DIR})
include_directories(${BLACKICE_HEADER_DIR})
include_directories(${PROJECT_SOURCE_DIR}/shared/schema-include)
include_directories(${IRON_THRONE_LIBS_DIR}/flatbuffers/include)

add_subdirectory(src)