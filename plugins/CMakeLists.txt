# 插件构建配置 - 使用子目录结构

# 添加所有插件子目录
add_subdirectory(core_plugin)
add_subdirectory(math_plugin)
add_subdirectory(math_enhanced_plugin)
add_subdirectory(string_plugin)
add_subdirectory(array_plugin)
add_subdirectory(type_plugin)
add_subdirectory(io_plugin)
# add_subdirectory(extra_plugin)  # 与core_plugin功能重复，暂时禁用

# 创建插件目标列表（用于依赖管理）
set(PLUGIN_TARGETS
    core_plugin
    math_plugin
    math_enhanced_plugin
    string_plugin
    array_plugin
    type_plugin
    io_plugin
    # extra_plugin  # 与core_plugin功能重复，暂时禁用
)

# 设置所有插件的通用属性
foreach(PLUGIN_TARGET ${PLUGIN_TARGETS})
    if(TARGET ${PLUGIN_TARGET})
        set_target_properties(${PLUGIN_TARGET} PROPERTIES
            LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
            CXX_STANDARD 17
            CXX_STANDARD_REQUIRED ON
            POSITION_INDEPENDENT_CODE ON
        )
    endif()
endforeach()

# 创建统一的plugins目标，用于编译所有插件
add_custom_target(plugins ALL
    DEPENDS ${PLUGIN_TARGETS}
    COMMENT "Building all plugins"
)

# 为plugins目标添加描述
set_target_properties(plugins PROPERTIES
    FOLDER "Plugins"
)

# 添加安装规则（可选）
install(TARGETS ${PLUGIN_TARGETS}
    LIBRARY DESTINATION plugins
    RUNTIME DESTINATION plugins
)
