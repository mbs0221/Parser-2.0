# 自动化封装工具的CMakeLists.txt

cmake_minimum_required(VERSION 3.10)
project(auto_wrapper)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找必要的包
find_package(filesystem REQUIRED)

# 包含目录
include_directories(${CMAKE_SOURCE_DIR}/include)

# 源文件
set(SOURCES
    auto_wrapper.cpp
    code_generator.cpp
    main.cpp
)

# 创建可执行文件
add_executable(auto_wrapper ${SOURCES})

# 链接库
target_link_libraries(auto_wrapper stdc++fs)

# 设置输出目录
set_target_properties(auto_wrapper PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tools
)

# 安装规则
install(TARGETS auto_wrapper
    RUNTIME DESTINATION bin
)

# 安装示例配置文件
install(DIRECTORY examples/
    DESTINATION share/auto_wrapper/examples
    FILES_MATCHING PATTERN "*.json"
)
