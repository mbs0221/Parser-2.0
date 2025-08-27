#include <gtest/gtest.h>
#include "lexer/lexer.h"
#include "lexer/value.h"

using namespace std;

// Lexer综合测试套件
class LexerComprehensiveTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 测试前的设置
    }
    
    void TearDown() override {
        // 测试后的清理
    }
};

// 测试基本类型
TEST_F(LexerComprehensiveTest, BasicTypes) {
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
TEST_F(LexerComprehensiveTest, Operators) {
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
TEST_F(LexerComprehensiveTest, ValueTypes) {
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
TEST_F(LexerComprehensiveTest, BooleanConstants) {
    EXPECT_EQ(Bool::True->toString(), "true");
    EXPECT_EQ(Bool::False->toString(), "false");
}

// 测试类型比较
TEST_F(LexerComprehensiveTest, TypeComparison) {
    Type* maxType = Type::max(Type::Int, Type::Double);
    EXPECT_EQ(maxType, Type::Double);
    
    maxType = Type::max(Type::Int, Type::Bool);
    EXPECT_EQ(maxType, Type::Int);
}

// 测试值变量
TEST_F(LexerComprehensiveTest, ValueVariables) {
    Integer a(10);
    Integer b(5);
    
    Integer result = a + b;
    EXPECT_EQ(result.toString(), "15");
    
    result = a - b;
    EXPECT_EQ(result.toString(), "5");
}

// 测试字符串操作
TEST_F(LexerComprehensiveTest, StringOperations) {
    String a("Hello");
    String b(" World");
    
    // 测试字符串的基本功能
    EXPECT_EQ(a.toString(), "\"Hello\"");
    EXPECT_EQ(b.toString(), "\" World\"");
    EXPECT_EQ(a.getValue(), "Hello");
    EXPECT_EQ(b.getValue(), " World");
    
    // 测试字符串拼接运算符
    String result1 = a + b;
    EXPECT_EQ(result1.getValue(), "Hello World");
    EXPECT_EQ(result1.toString(), "\"Hello World\"");
    
    // 测试字符串与字符拼接
    String result2 = a + '!';
    EXPECT_EQ(result2.getValue(), "Hello!");
    
    // 测试字符串与整数拼接
    String result3 = a + 123;
    EXPECT_EQ(result3.getValue(), "Hello123");
    
    // 测试字符串与浮点数拼接
    String result4 = a + 3.14;
    EXPECT_EQ(result4.getValue(), "Hello3.140000");
    
    // 测试字符串与布尔值拼接
    String result5 = a + true;
    EXPECT_EQ(result5.getValue(), "Hellotrue");
    
    String result6 = a + false;
    EXPECT_EQ(result6.getValue(), "Hellofalse");
    
    // 测试全局运算符重载（其他类型在前）
    std::string prefix = "Prefix ";
    String result7 = prefix + a;
    EXPECT_EQ(result7.getValue(), "Prefix Hello");
    
    String result8 = 'X' + a;
    EXPECT_EQ(result8.getValue(), "XHello");
    
    String result9 = 456 + a;
    EXPECT_EQ(result9.getValue(), "456Hello");
    
    String result10 = 2.718 + a;
    EXPECT_EQ(result10.getValue(), "2.718000Hello");
    
    String result11 = true + a;
    EXPECT_EQ(result11.getValue(), "trueHello");
}

// 测试运算符设计
TEST_F(LexerComprehensiveTest, OperatorDesign) {
    EXPECT_EQ(Operator::Add->getSymbol(), "+");
    EXPECT_EQ(Operator::Sub->getSymbol(), "-");
    EXPECT_EQ(Operator::Mul->getSymbol(), "*");
    EXPECT_EQ(Operator::Div->getSymbol(), "/");
    
    EXPECT_GT(Operator::Mul->getPrecedence(), Operator::Add->getPrecedence());
    EXPECT_TRUE(Operator::Add->isLeftAssoc());
    EXPECT_TRUE(Operator::Mul->isLeftAssoc());
}

// 测试运算符类型
TEST_F(LexerComprehensiveTest, OperatorType) {
    EXPECT_EQ(Operator::LT->getSymbol(), "<");
    EXPECT_EQ(Operator::GT->getSymbol(), ">");
    EXPECT_EQ(Operator::LE->getSymbol(), "<=");
    EXPECT_EQ(Operator::GE->getSymbol(), ">=");
    EXPECT_EQ(Operator::EQ->getSymbol(), "==");
    EXPECT_EQ(Operator::NE->getSymbol(), "!=");
}

// 测试合并目标类型
TEST_F(LexerComprehensiveTest, MergedTargetType) {
    Integer intVal(42);
    Double doubleVal(3.14);
    
    // 测试整数和浮点数的基本功能
    EXPECT_EQ(intVal.toString(), "42");
    EXPECT_EQ(doubleVal.toString(), "3.14");
    EXPECT_EQ(intVal.getValue(), 42);
    EXPECT_EQ(doubleVal.getValue(), 3.14);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
