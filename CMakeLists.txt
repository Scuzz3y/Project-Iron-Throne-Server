cmake_minimum_required(VERSION 3.6)
project(iron-throne-server)

set(CMAKE_CXX_STANDARD 17)
set(GCC_GENERAL_COMPILATION_FLAGS "-std=c++17 -g -pthread")
set(GCC_COVERAGE_COMPILE_FLAGS "-Wall -pedantic \
-pedantic-errors -Wextra -Wcast-align \
-Wcast-qual -Wconversion \
-Wdisabled-optimization \
-Wformat=2 \
-Wformat-nonliteral -Wformat-security \
-Wformat-y2k \
-Wimport -Winit-self -Winline -Winvalid-pch \
-Wmissing-field-initializers -Wmissing-format-attribute \
-Wmissing-include-dirs -Wmissing-noreturn \
-Wpacked -Wpointer-arith \
-Wredundant-decls \
-Wshadow -Wstack-protector \
-Wstrict-aliasing=2 -Wswitch-default \
-Wunreachable-code -Wunused \
-Wunused-parameter \
-Wvariadic-macros \
-Wwrite-strings")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS} ${GCC_GENERAL_COMPILATION_FLAGS}" )

set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})

set(IRON_THRONE_INSTALL_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(IRON_THRONE_INSTALL_BIN_DIR ${PROJECT_SOURCE_DIR}/bin)
set(IRON_THRONE_LIBS_DIR ${PROJECT_SOURCE_DIR}/libs)
set(IRON_THRONE_PUPPET_MODULE_DIR ${PROJECT_SOURCE_DIR}/src/modules/puppet)
set(IRON_THRONE_MASTER_MODULE_DIR ${PROJECT_SOURCE_DIR}/src/modules/master)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package( Boost 1.65.1 COMPONENTS system thread REQUIRED )

add_definitions(-DBOOST_ERROR_CODE_HEADER_ONLY)


# Individual Puppet Module Include Dirs
set(BLACKICE_HEADER_DIR ${IRON_THRONE_PUPPET_MODULE_DIR}/blackIce)

# Individual Master Module Include Dirs
set(ARSENALHTTP_HEADER_DIR ${IRON_THRONE_MASTER_MODULE_DIR}/arsenalHttp)

include_directories(${IRON_THRONE_INSTALL_INCLUDE_DIR})

# Include Puppet Modules
include_directories(${BLACKICE_HEADER_DIR})

# Include Master Modules
include_directories(${ARSENALHTTP_HEADER_DIR})

# Include Libraries
include_directories(${PROJECT_SOURCE_DIR}/shared/schema-include)
include_directories(${IRON_THRONE_LIBS_DIR}/flatbuffers/include)

add_subdirectory(src)