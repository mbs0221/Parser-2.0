#include <gtest/gtest.h>
#include "lexer/lexer.h"
#include "lexer/value.h"

using namespace std;

// Lexer测试套件
class LexerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 测试前的设置
    }
    
    void TearDown() override {
        // 测试后的清理
    }
};

// 测试基本类型
TEST_F(LexerTest, BasicTypes) {
    EXPECT_EQ(Type::Int->word, "int");
    EXPECT_EQ(Type::Int->width, 4);
    
    EXPECT_EQ(Type::Double->word, "double");
    EXPECT_EQ(Type::Double->width, 8);
    
    EXPECT_EQ(Type::Bool->word, "bool");
    EXPECT_EQ(Type::Bool->width, 1);
    
    EXPECT_EQ(Type::String->word, "string");
    EXPECT_EQ(Type::String->width, 0);
}

// 测试运算符
TEST_F(LexerTest, Operators) {
    EXPECT_EQ(Operator::Add->getSymbol(), "+");
    EXPECT_EQ(Operator::Add->getPrecedence(), 4);
    EXPECT_TRUE(Operator::Add->isLeftAssoc());
    
    EXPECT_EQ(Operator::Mul->getSymbol(), "*");
    EXPECT_EQ(Operator::Mul->getPrecedence(), 5);
    EXPECT_TRUE(Operator::Mul->isLeftAssoc());
    
    EXPECT_EQ(Operator::LT->getSymbol(), "<");
    EXPECT_EQ(Operator::LT->getPrecedence(), 3);
    EXPECT_TRUE(Operator::LT->isLeftAssoc());
}

// 测试值类型
TEST_F(LexerTest, ValueTypes) {
    Integer intVal(42);
    EXPECT_EQ(intVal.toString(), "42");
    
    Double doubleVal(3.14);
    EXPECT_EQ(doubleVal.toString(), "3.14");
    
    Bool boolVal(true);
    EXPECT_EQ(boolVal.toString(), "true");
    
    String strVal("Hello, World!");
    EXPECT_EQ(strVal.toString(), "\"Hello, World!\"");
}

// 测试布尔常量
TEST_F(LexerTest, BooleanConstants) {
    EXPECT_EQ(Bool::True->toString(), "true");
    EXPECT_EQ(Bool::False->toString(), "false");
}

// 测试类型比较
TEST_F(LexerTest, TypeComparison) {
    Type* maxType = Type::max(Type::Int, Type::Double);
    EXPECT_EQ(maxType, Type::Double);
    
    maxType = Type::max(Type::Int, Type::Bool);
    EXPECT_EQ(maxType, Type::Int);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
