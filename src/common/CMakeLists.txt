# Common library CMakeLists.txt
cmake_minimum_required(VERSION 3.10)

# 创建 common 静态库
add_library(common STATIC
    logger.cpp
    string_utils.cpp
)

# 设置包含目录
target_include_directories(common PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

# 链接 log4cpp 库
find_package(PkgConfig REQUIRED)
pkg_check_modules(LOG4CPP REQUIRED log4cpp)

target_link_libraries(common ${LOG4CPP_LIBRARIES})
target_include_directories(common PUBLIC ${LOG4CPP_INCLUDE_DIRS})
target_compile_options(common PUBLIC ${LOG4CPP_CFLAGS_OTHER})

# 设置 C++ 标准
target_compile_features(common PUBLIC cxx_std_17)

# 设置位置无关代码
set_property(TARGET common PROPERTY POSITION_INDEPENDENT_CODE ON)
