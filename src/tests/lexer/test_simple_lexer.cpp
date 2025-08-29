#include <gtest/gtest.h>
#include "lexer/lexer.h"
#include "lexer/value.h"
#include <iostream>

using namespace std;

class SimpleLexerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 初始化设置
    }
    
    void TearDown() override {
        // 清理资源
    }
};

// 测试基本类型
TEST_F(SimpleLexerTest, BasicTypes) {
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
TEST_F(SimpleLexerTest, Operators) {
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
TEST_F(SimpleLexerTest, ValueTypes) {
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
TEST_F(SimpleLexerTest, BooleanConstants) {
    EXPECT_EQ(Bool::True->toString(), "true");
    EXPECT_EQ(Bool::False->toString(), "false");
}

// 测试类型比较
TEST_F(SimpleLexerTest, TypeComparison) {
    Type* maxType = Type::max(Type::Int, Type::Double);
    EXPECT_EQ(maxType, Type::Double);
    
    maxType = Type::max(Type::Int, Type::Bool);
    EXPECT_EQ(maxType, Type::Int);
}

// 测试Value类型的基本功能
TEST_F(SimpleLexerTest, ValueBasicFunctionality) {
    Integer* intVal = new Integer(42);
    Double* doubleVal = new Double(3.14);
    Char* charVal = new Char('A');
    Bool* boolVal = new Bool(true);
    
    // 测试整数值
    EXPECT_EQ(intVal->toString(), "42");
    EXPECT_TRUE(intVal->toBool());
    EXPECT_EQ(intVal->getTypeName(), "int");
    
    // 测试浮点数值
    EXPECT_EQ(doubleVal->toString(), "3.14");
    EXPECT_FALSE(doubleVal->toBool());
    EXPECT_EQ(doubleVal->getTypeName(), "double");
    
    // 测试字符值
    EXPECT_EQ(charVal->toString(), "A");
    EXPECT_FALSE(charVal->toBool());
    EXPECT_EQ(charVal->getTypeName(), "char");
    
    // 测试布尔值
    EXPECT_EQ(boolVal->toString(), "true");
    EXPECT_TRUE(boolVal->toBool());
    EXPECT_EQ(boolVal->getTypeName(), "bool");
    
    // 清理内存
    delete intVal;
    delete doubleVal;
    delete charVal;
    delete boolVal;
}

int test_simple_lexer_main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
