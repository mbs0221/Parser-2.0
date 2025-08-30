# 插件构建配置

# 核心基础函数插件
add_library(core_plugin SHARED
    basic_builtin_plugin.cpp
)

target_include_directories(core_plugin PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/Parser
)

target_link_libraries(core_plugin parser interpreter)

# 增强数学函数插件
add_library(math_enhanced_plugin SHARED
    math_plugin_enhanced.cpp
)

target_include_directories(math_enhanced_plugin PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/Parser
)

target_link_libraries(math_enhanced_plugin parser interpreter)

# 字符串处理插件
add_library(string_plugin SHARED
    string_plugin.cpp
)

target_include_directories(string_plugin PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/Parser
)

target_link_libraries(string_plugin parser interpreter)

# 数组操作插件
add_library(array_plugin SHARED
    array_plugin.cpp
)

target_include_directories(array_plugin PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/Parser
)

target_link_libraries(array_plugin parser interpreter)

# 类型转换插件
add_library(type_plugin SHARED
    type_plugin.cpp
)

target_include_directories(type_plugin PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/Parser
)

target_link_libraries(type_plugin parser interpreter)

# IO插件
add_library(io_plugin SHARED
    io_plugin.cpp
)

target_include_directories(io_plugin PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/Parser
)

target_link_libraries(io_plugin parser interpreter)

# 设置插件输出目录
set_target_properties(core_plugin math_enhanced_plugin string_plugin array_plugin type_plugin io_plugin PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
)
