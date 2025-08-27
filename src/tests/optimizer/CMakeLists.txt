# Optimizer测试配置
add_executable(test_optimizer_comprehensive test_optimizer_comprehensive.cpp)
target_include_directories(test_optimizer_comprehensive PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(test_optimizer_comprehensive 
    optimizer
    parser
    lexer
    gtest
    gtest_main
    pthread
)
add_test(NAME test_optimizer_comprehensive COMMAND test_optimizer_comprehensive)

# 其他optimizer相关测试
add_executable(test_optimizer_basic test_optimizer.cpp)
target_include_directories(test_optimizer_basic PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(test_optimizer_basic 
    optimizer
    parser
    lexer
    gtest
    gtest_main
    pthread
)
add_test(NAME test_optimizer_basic COMMAND test_optimizer_basic)

add_executable(test_operator_precedence test_operator_precedence.cpp)
target_include_directories(test_operator_precedence PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(test_operator_precedence 
    optimizer
    parser
    lexer
    gtest
    gtest_main
    pthread
)
add_test(NAME test_operator_precedence COMMAND test_operator_precedence)

add_executable(test_updated_parser test_updated_parser.cpp)
target_include_directories(test_updated_parser PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(test_updated_parser 
    optimizer
    parser
    lexer
    gtest
    gtest_main
    pthread
)
add_test(NAME test_updated_parser COMMAND test_updated_parser)
