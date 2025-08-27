# Parser测试配置
add_executable(test_parser_comprehensive test_parser_comprehensive.cpp)
target_include_directories(test_parser_comprehensive PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(test_parser_comprehensive 
    parser
    lexer
    gtest
    gtest_main
    pthread
)
add_test(NAME test_parser_comprehensive COMMAND test_parser_comprehensive)

# 其他parser相关测试
add_executable(test_parser_basic test_parser.cpp)
target_include_directories(test_parser_basic PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(test_parser_basic 
    parser
    lexer
    gtest
    gtest_main
    pthread
)
add_test(NAME test_parser_basic COMMAND test_parser_basic)

add_executable(test_parser_constant test_parser_constant.cpp)
target_include_directories(test_parser_constant PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(test_parser_constant 
    parser
    lexer
    gtest
    gtest_main
    pthread
)
add_test(NAME test_parser_constant COMMAND test_parser_constant)

add_executable(test_parser_helpers test_parser_helpers.cpp)
target_include_directories(test_parser_helpers PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(test_parser_helpers 
    parser
    lexer
    gtest
    gtest_main
    pthread
)
add_test(NAME test_parser_helpers COMMAND test_parser_helpers)

add_executable(test_parser_simple test_parser_simple.cpp)
target_include_directories(test_parser_simple PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(test_parser_simple 
    parser
    lexer
    gtest
    gtest_main
    pthread
)
add_test(NAME test_parser_simple COMMAND test_parser_simple)

add_executable(test_constant_expression test_constant_expression.cpp)
target_include_directories(test_constant_expression PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(test_constant_expression 
    parser
    lexer
    gtest
    gtest_main
    pthread
)
add_test(NAME test_constant_expression COMMAND test_constant_expression)

add_executable(test_expression_parsing test_expression_parsing.cpp)
target_include_directories(test_expression_parsing PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(test_expression_parsing 
    parser
    lexer
    gtest
    gtest_main
    pthread
)
add_test(NAME test_expression_parsing COMMAND test_expression_parsing)

add_executable(test_leaf_expression test_leaf_expression.cpp)
target_include_directories(test_leaf_expression PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(test_leaf_expression 
    parser
    lexer
    gtest
    gtest_main
    pthread
)
add_test(NAME test_leaf_expression COMMAND test_leaf_expression)

add_executable(test_simplified_expressions test_simplified_expressions.cpp)
target_include_directories(test_simplified_expressions PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(test_simplified_expressions 
    parser
    lexer
    gtest
    gtest_main
    pthread
)
add_test(NAME test_simplified_expressions COMMAND test_simplified_expressions)

add_executable(test_simplified_binary test_simplified_binary.cpp)
target_include_directories(test_simplified_binary PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(test_simplified_binary 
    parser
    lexer
    gtest
    gtest_main
    pthread
)
add_test(NAME test_simplified_binary COMMAND test_simplified_binary)

add_executable(test_cast_expression test_cast_expression.cpp)
target_include_directories(test_cast_expression PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(test_cast_expression 
    parser
    lexer
    gtest
    gtest_main
    pthread
)
add_test(NAME test_cast_expression COMMAND test_cast_expression)

add_executable(test_assignment_binary test_assignment_binary.cpp)
target_include_directories(test_assignment_binary PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(test_assignment_binary 
    parser
    lexer
    gtest
    gtest_main
    pthread
)
add_test(NAME test_assignment_binary COMMAND test_assignment_binary)

add_executable(test_ast_only test_ast_only.cpp)
target_include_directories(test_ast_only PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(test_ast_only 
    parser
    lexer
    gtest
    gtest_main
    pthread
)
add_test(NAME test_ast_only COMMAND test_ast_only)
