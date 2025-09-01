# Lexer测试配置

# 查找Google Test
find_package(GTest REQUIRED)

# 设置测试源文件
set(LEXER_TEST_SOURCES
    test_simple_lexer.cpp
    test_value_variables.cpp
    test_merged_target_type.cpp
    test_operator_design.cpp
    test_string_operations.cpp
    test_lexer_comprehensive.cpp
    test_lexer.cpp
    debug_lexer.cpp
    test_char_flyweight.cpp
    test_new_functions.cpp  # 新增的Google Test测试
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

# 设置C++标准
set_target_properties(test_lexer PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# 创建单独的Google Test测试目标
add_executable(lexer_gtest test_new_functions.cpp)
target_include_directories(lexer_gtest PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(lexer_gtest PRIVATE
    lexer
    GTest::gtest
    GTest::gtest_main
)

# 设置C++标准
set_target_properties(lexer_gtest PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# 添加到CTest
enable_testing()
add_test(NAME TestLexer COMMAND test_lexer)
add_test(NAME LexerGTest COMMAND lexer_gtest)

# 设置工作目录
set_tests_properties(TestLexer PROPERTIES
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
set_tests_properties(LexerGTest PROPERTIES
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
