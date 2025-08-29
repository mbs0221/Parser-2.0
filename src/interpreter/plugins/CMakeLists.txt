# 插件构建配置

# 基础内置函数插件
add_library(basic_builtin_plugin SHARED
    basic_builtin_plugin.cpp
)

target_include_directories(basic_builtin_plugin PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/Parser
)

target_link_libraries(basic_builtin_plugin parser)

# 数学函数插件
add_library(math_plugin SHARED
    math_plugin.cpp
)

target_include_directories(math_plugin PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/Parser
)

target_link_libraries(math_plugin parser)

# 字符串处理插件
add_library(string_plugin SHARED
    string_plugin.cpp
)

target_include_directories(string_plugin PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/Parser
)

target_link_libraries(string_plugin parser)

# 设置插件输出目录
set_target_properties(basic_builtin_plugin math_plugin string_plugin PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
)
