# Parser测试配置 - 依赖lexer和parser库
set(PARSER_TEST_SOURCES
    test_simple_parser.cpp
    test_parser.cpp
    test_leaf_expression.cpp
    test_assignment_binary.cpp
    test_expression_parsing.cpp
    test_parser_comprehensive.cpp
    test_parser_helpers.cpp
    test_parser_simple.cpp
    test_str_method.cpp
    test_ast_only.cpp
    test_str_simple.cpp
    debug_let.cpp
)

# 创建统一的parser测试目标
add_executable(test_parser ${PARSER_TEST_SOURCES})
target_include_directories(test_parser PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

# 链接lexer和parser库
target_link_libraries(test_parser
    lexer
    parser
    ${LOG4CPP_LIBRARIES}
    gtest
    gtest_main
    pthread
)

# 设置输出目录
set_target_properties(test_parser PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# 添加测试
add_test(NAME test_parser COMMAND test_parser)
