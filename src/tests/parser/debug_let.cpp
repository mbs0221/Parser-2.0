#include "lexer/value.h"
#include <iostream>

class DebugLetTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 设置测试环境
    }
};

TEST_F(DebugLetTest, DebugLet) {
    EXPECT_EQ(BASIC, 1);
    EXPECT_EQ(LET, 2);
    EXPECT_EQ(ID, 3);
    EXPECT_EQ(NUM, 4);
    EXPECT_EQ(REAL, 5);
    EXPECT_EQ(DOUBLE, 6);
    EXPECT_EQ(CHAR, 7);
    EXPECT_EQ(STR, 8);
    EXPECT_EQ(BOOL, 9);
    EXPECT_EQ(END_OF_FILE, 10);
    EXPECT_EQ(IF, 11);
    EXPECT_EQ(THEN, 12);
    EXPECT_EQ(ELSE, 13);
    EXPECT_EQ(DO, 14);
    EXPECT_EQ(WHILE, 15);
    EXPECT_EQ(FOR, 16);
    EXPECT_EQ(CASE, 17);
    EXPECT_EQ(DEFAULT, 18);
}

