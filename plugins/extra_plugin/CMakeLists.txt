# 额外功能插件构建配置

# 创建额外功能插件动态库
add_library(extra_plugin SHARED
    extra_plugin.cpp
)

# 设置包含目录
target_include_directories(extra_plugin PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/Parser
)

# 链接依赖库
target_link_libraries(extra_plugin parser interpreter)

# 设置插件属性
set_target_properties(extra_plugin PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    POSITION_INDEPENDENT_CODE ON
)
