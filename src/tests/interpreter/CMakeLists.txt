# Interpreter测试配置 - 依赖lexer、parser和interpreter库
set(INTERPRETER_TEST_SOURCES
    test_simple_interpreter.cpp
    test_constant_expression.cpp
)

# 创建统一的interpreter测试目标
add_executable(test_interpreter ${INTERPRETER_TEST_SOURCES})
target_include_directories(test_interpreter PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

# 链接所有必要的库
target_link_libraries(test_interpreter
    lexer
    parser
    interpreter
    optimizer
    ${LOG4CPP_LIBRARIES}
    gtest
    gtest_main
    pthread
)

# 设置输出目录
set_target_properties(test_interpreter PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# 添加测试
add_test(NAME test_interpreter COMMAND test_interpreter)
