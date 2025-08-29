# Lexer测试配置 - 只依赖lexer库
set(LEXER_TEST_SOURCES
    test_simple_lexer.cpp
    test_value_variables.cpp
    test_merged_target_type.cpp
    test_operator_design.cpp
    test_string_operations.cpp
    test_lexer_comprehensive.cpp
    test_lexer.cpp
    debug_lexer.cpp
)

# 创建统一的lexer测试目标
add_executable(test_lexer ${LEXER_TEST_SOURCES})
target_include_directories(test_lexer PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

# 只链接lexer库
target_link_libraries(test_lexer
    lexer
    ${LOG4CPP_LIBRARIES}
    gtest
    gtest_main
    pthread
)

# 设置输出目录
set_target_properties(test_lexer PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# 添加测试
add_test(NAME test_lexer COMMAND test_lexer)
