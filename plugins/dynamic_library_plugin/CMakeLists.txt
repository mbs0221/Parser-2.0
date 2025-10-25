# 动态库插件构建配置

# 创建动态库插件动态库
add_library(dynamic_library_plugin SHARED
    dynamic_library_plugin.cpp
    dynamic_library_loader.cpp
)

# 设置包含目录
target_include_directories(dynamic_library_plugin PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/Parser
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 链接依赖库
target_link_libraries(dynamic_library_plugin 
    parser 
    interpreter
    dl  # 动态库加载需要dl库
)

# 设置插件属性
set_target_properties(dynamic_library_plugin PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    POSITION_INDEPENDENT_CODE ON
)

# 设置编译器特定选项
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(dynamic_library_plugin PRIVATE
        -stdlib=libc++
    )
    target_link_options(dynamic_library_plugin PRIVATE
        -stdlib=libc++
    )
endif()
