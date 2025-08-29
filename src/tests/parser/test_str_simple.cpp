#include <gtest/gtest.h>
#include <string>

using namespace std;

class StrSimpleTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 测试前的设置
    }
    
    void TearDown() override {
        // 测试后的清理
    }
};

TEST_F(StrSimpleTest, StringLiteral) {
    string testStr = "Hello World";
    EXPECT_EQ(testStr, "Hello World");
}

TEST_F(StrSimpleTest, IntegerLiteral) {
    int testInt = 42;
    EXPECT_EQ(to_string(testInt), "42");
}

TEST_F(StrSimpleTest, BooleanLiteral) {
    bool testBool = true;
    EXPECT_EQ(testBool ? "true" : "false", "true");
    
    bool testBoolFalse = false;
    EXPECT_EQ(testBoolFalse ? "true" : "false", "false");
}
