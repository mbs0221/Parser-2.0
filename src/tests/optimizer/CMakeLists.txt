# Optimizer测试配置 - 只依赖lexer库
set(OPTIMIZER_TEST_SOURCES
    test_simple_optimizer.cpp
    test_optimizer.cpp
    test_operator_precedence.cpp
    test_updated_parser.cpp
)

# 创建统一的optimizer测试目标
add_executable(test_optimizer ${OPTIMIZER_TEST_SOURCES})
target_include_directories(test_optimizer PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

# 链接所有必要的库
target_link_libraries(test_optimizer
    lexer
    parser
    optimizer
    ${LOG4CPP_LIBRARIES}
    gtest
    gtest_main
    pthread
)

# 设置输出目录
set_target_properties(test_optimizer PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# 添加测试
add_test(NAME test_optimizer COMMAND test_optimizer)
