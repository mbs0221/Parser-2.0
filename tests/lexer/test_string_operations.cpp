#include <gtest/gtest.h>
#include "lexer/value.h"
#include <iostream>

using namespace std;

class StringOperationsTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 初始化设置
    }
    
    void TearDown() override {
        // 清理资源
    }
};

// 测试字符串基本操作
TEST_F(StringOperationsTest, StringBasicOperations) {
    String* str1 = new String("Hello");
    String* str2 = new String("World");
    
    ASSERT_NE(str1, nullptr);
    ASSERT_NE(str2, nullptr);
    
    EXPECT_EQ(str1->getValue(), "Hello");
    EXPECT_EQ(str2->getValue(), "World");
    EXPECT_EQ(str1->toString(), "\"Hello\"");
    EXPECT_EQ(str2->toString(), "\"World\"");
    
    delete str1;
    delete str2;
}

// 测试字符串连接
TEST_F(StringOperationsTest, StringConcatenation) {
    String* str1 = new String("Hello");
    String* str2 = new String("World");
    
    String result = *str1 + *str2;
    EXPECT_EQ(result.getValue(), "HelloWorld");
    EXPECT_EQ(result.toString(), "\"HelloWorld\"");
    
    delete str1;
    delete str2;
}

// 测试字符串比较
TEST_F(StringOperationsTest, StringComparison) {
    String* str1 = new String("Hello");
    String* str2 = new String("Hello");
    String* str3 = new String("World");
    
    // 测试相等性
    EXPECT_EQ(str1->getValue(), str2->getValue());
    EXPECT_NE(str1->getValue(), str3->getValue());
    
    // 测试字符串长度
    EXPECT_EQ(str1->getValue().length(), 5);
    EXPECT_EQ(str3->getValue().length(), 5);
    
    delete str1;
    delete str2;
    delete str3;
}

// 测试空字符串
TEST_F(StringOperationsTest, EmptyString) {
    String* emptyStr = new String("");
    
    EXPECT_EQ(emptyStr->getValue(), "");
    EXPECT_EQ(emptyStr->toString(), "\"\"");
    EXPECT_FALSE(emptyStr->toBool());
    
    delete emptyStr;
}

int test_string_operations_main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
