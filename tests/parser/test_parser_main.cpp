#include <gtest/gtest.h>

// 声明其他测试的main函数
int test_simple_parser_main(int argc, char **argv);

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    
    // 运行所有测试
    int result = RUN_ALL_TESTS();
    
    // 调用其他测试函数
    test_simple_parser_main(argc, argv);
    
    return result;
}
