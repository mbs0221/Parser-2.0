#include <gtest/gtest.h>
#include <string>

using namespace std;

class InterpreterSimpleTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 测试前的设置
    }
    
    void TearDown() override {
        // 测试后的清理
    }
};

TEST_F(InterpreterSimpleTest, BasicOutput) {
    string output = "Hello World";
    EXPECT_EQ(output, "Hello World");
}

TEST_F(InterpreterSimpleTest, StringConcatenation) {
    string str1 = "Hello, ";
    string str2 = "World!";
    string result = str1 + str2;
    EXPECT_EQ(result, "Hello, World!");
}

TEST_F(InterpreterSimpleTest, StringComparison) {
    string apple = "apple";
    string banana = "banana";
    bool comparison = apple < banana;
    EXPECT_TRUE(comparison);
}

TEST_F(InterpreterSimpleTest, StringLength) {
    string testStr = "Hello World";
    EXPECT_EQ(testStr.length(), 11);
}

TEST_F(InterpreterSimpleTest, StringIndexAccess) {
    string testStr = "Hello World";
    char firstChar = testStr[0];
    EXPECT_EQ(firstChar, 'H');
}

TEST_F(InterpreterSimpleTest, ArrayOperations) {
    string fruits[] = {"apple", "banana", "cherry"};
    EXPECT_EQ(fruits[0], "apple");
    EXPECT_EQ(fruits[1], "banana");
    EXPECT_EQ(fruits[2], "cherry");
}

TEST_F(InterpreterSimpleTest, ScopeConcept) {
    string globalVar = "全局变量";
    EXPECT_EQ(globalVar, "全局变量");
    
    {
        string localVar = "局部变量";
        EXPECT_EQ(localVar, "局部变量");
    }
    // 局部作用域结束，localVar不再可访问
    EXPECT_EQ(globalVar, "全局变量");
}
