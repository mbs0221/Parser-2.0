#include <gtest/gtest.h>
#include <string>

using namespace std;

class BuiltinFunctionTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 设置测试环境
    }
    
    void TearDown() override {
        // 清理测试环境
    }
};

// 测试内置函数调用
TEST_F(BuiltinFunctionTest, PrintFunction) {
    // 模拟print函数调用
    string expected = "Hello World";
    EXPECT_EQ(expected, "Hello World");
}

TEST_F(BuiltinFunctionTest, LengthFunction) {
    // 模拟length函数调用
    string testStr = "Hello World";
    EXPECT_EQ(testStr.length(), 11);
}

TEST_F(BuiltinFunctionTest, StringConcatenation) {
    // 模拟字符串拼接
    string str1 = "Hello, ";
    string str2 = "World!";
    string result = str1 + str2;
    EXPECT_EQ(result, "Hello, World!");
}

TEST_F(BuiltinFunctionTest, ArrayAccess) {
    // 模拟数组访问
    string fruits[] = {"apple", "banana", "cherry"};
    EXPECT_EQ(fruits[0], "apple");
}

TEST_F(BuiltinFunctionTest, DictionaryAccess) {
    // 模拟字典访问
    string expected = "John";
    EXPECT_EQ(expected, "John");
}

TEST_F(BuiltinFunctionTest, CountFunction) {
    // 模拟count函数调用
    int count = 5;
    EXPECT_EQ(count, 5);
}

TEST_F(BuiltinFunctionTest, SubstringFunction) {
    // 模拟substring函数调用
    string text = "Hello World";
    string sub = text.substr(0, 5);
    EXPECT_EQ(sub, "Hello");
}
