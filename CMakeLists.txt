cmake_minimum_required(VERSION 3.10)
project(Parser VERSION 2.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)
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

# 添加子目录
add_subdirectory(src/lexer)
add_subdirectory(src/parser)
add_subdirectory(src/interpreter)
add_subdirectory(src/optimizer)
add_subdirectory(src/main)

# 设置全局包含目录
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/Parser)
include_directories(${LOG4CPP_INCLUDE_DIRS})


