# Interpreter库的CMakeLists.txt

# 子目录用于组织文件，不需要单独的CMakeLists.txt

# 创建interpreter静态库
add_library(interpreter STATIC
    # 核心执行引擎
    core/interpreter.cpp
    core/statement.cpp
    core/expression.cpp
    
    # 类型系统
    types/type_registry.cpp
    types/builtin_types.cpp
    types/type_converter.cpp
    
    # 值系统
    values/value.cpp
    values/calculate.cpp
    
    # 作用域管理
    scope/scope.cpp
    scope/object_factory.cpp
    
    # 插件系统
    plugins/plugin_manager.cpp
    
    # 工具和日志
    utils/logger.cpp
)

# 创建interpreter静态库
add_library(interpreter STATIC ${INTERPRETER_SOURCES} ${INTERPRETER_HEADERS})

# 设置位置无关代码
set_property(TARGET interpreter PROPERTY POSITION_INDEPENDENT_CODE ON)

# 设置包含目录
target_include_directories(interpreter PUBLIC 
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/Parser
    ${LOG4CPP_INCLUDE_DIRS}
)

# 设置编译选项
target_compile_options(interpreter PRIVATE ${LOG4CPP_CFLAGS_OTHER})

# 链接依赖库
target_link_libraries(interpreter parser dl)

# 设置输出目录
set_target_properties(interpreter PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
)


