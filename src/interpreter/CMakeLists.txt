# Interpreter库的CMakeLists.txt

# 设置源文件
set(INTERPRETER_SOURCES
    ${CMAKE_SOURCE_DIR}/src/interpreter/interpreter.cpp
    ${CMAKE_SOURCE_DIR}/src/interpreter/statement.cpp
    ${CMAKE_SOURCE_DIR}/src/interpreter/expression.cpp
    ${CMAKE_SOURCE_DIR}/src/interpreter/calculate.cpp
    ${CMAKE_SOURCE_DIR}/src/interpreter/logger.cpp

    ${CMAKE_SOURCE_DIR}/src/interpreter/scope.cpp
    ${CMAKE_SOURCE_DIR}/src/interpreter/plugin_manager.cpp
)

# 设置头文件
set(INTERPRETER_HEADERS
    ${CMAKE_SOURCE_DIR}/include/interpreter/interpreter.h
    ${CMAKE_SOURCE_DIR}/include/interpreter/logger.h

    ${CMAKE_SOURCE_DIR}/include/interpreter/builtin_plugin.h
    ${CMAKE_SOURCE_DIR}/include/interpreter/control_flow.h
    ${CMAKE_SOURCE_DIR}/include/interpreter/scope.h
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


