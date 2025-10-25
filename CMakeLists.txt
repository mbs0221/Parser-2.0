cmake_minimum_required(VERSION 3.10)
project(Parser VERSION 2.0.0 LANGUAGES CXX)

# 设置编译器
set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置编译选项
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -Wall -Wextra")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -Wextra")
endif()

# 查找log4cpp
find_package(PkgConfig REQUIRED)
pkg_check_modules(LOG4CPP REQUIRED log4cpp)

# 启用测试
enable_testing()

# 查找Google Test测试框架
find_package(GTest REQUIRED)
if(GTest_FOUND)
    message(STATUS "Google Test found")
else()
    message(FATAL_ERROR "Google Test not found, please install libgtest-dev")
endif()

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 设置全局包含目录
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/Parser)
include_directories(${LOG4CPP_INCLUDE_DIRS})

# 添加子目录
add_subdirectory(src/common)
add_subdirectory(src/lexer)
add_subdirectory(src/parser)
add_subdirectory(src/interpreter)
add_subdirectory(src/optimizer)
add_subdirectory(src/compiler)
add_subdirectory(src/emulator)

# 添加插件和测试目录
add_subdirectory(plugins)
add_subdirectory(tests)

