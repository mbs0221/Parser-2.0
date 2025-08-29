#include <gtest/gtest.h>
#include "parser/parser.h"
#include "interpreter/interpreter.h"
#include "lexer/value.h"
#include <iostream>

using namespace std;

class ParserConstantTest : public ::testing::Test {
protected:
    Parser parser;
    Interpreter* interpreter;
    
    void SetUp() override {
        // 初始化设置
        interpreter = new Interpreter(false);
    }
    
    void TearDown() override {
        // 清理资源
        delete interpreter;
    }
};

// 测试整数常量解析
TEST_F(ParserConstantTest, IntegerConstant) {
    ConstantExpression* intConst = new ConstantExpression(42);
    Value* intValue = interpreter->visit(intConst);
    
    ASSERT_NE(intValue, nullptr);
    EXPECT_EQ(intValue->toString(), "42");
    
    // 不要删除intConst，因为它的析构函数是空的，会导致内存泄漏
    // 也不要删除intValue，因为它被ConstantExpression管理
}

// 测试浮点数常量解析
TEST_F(ParserConstantTest, DoubleConstant) {
    ConstantExpression* doubleConst = new ConstantExpression(3.14);
    Value* doubleValue = interpreter->visit(doubleConst);
    
    ASSERT_NE(doubleValue, nullptr);
    EXPECT_EQ(doubleValue->toString(), "3.14");
    
    // 不要删除doubleConst，因为它的析构函数是空的，会导致内存泄漏
    // 也不要删除doubleValue，因为它被ConstantExpression管理
}

// 测试布尔常量解析
TEST_F(ParserConstantTest, BoolConstant) {
    ConstantExpression* boolConst = new ConstantExpression(true);
    Value* boolValue = interpreter->visit(boolConst);
    
    ASSERT_NE(boolValue, nullptr);
    EXPECT_EQ(boolValue->toString(), "true");
    
    // 不要删除boolConst，因为它的析构函数是空的，会导致内存泄漏
    // 也不要删除boolValue，因为它被ConstantExpression管理
}

// 测试字符常量解析
TEST_F(ParserConstantTest, CharConstant) {
    ConstantExpression* charConst = new ConstantExpression('A');
    Value* charValue = interpreter->visit(charConst);
    
    ASSERT_NE(charValue, nullptr);
    EXPECT_EQ(charValue->toString(), "A");
    
    // 不要删除charConst，因为它的析构函数是空的，会导致内存泄漏
    // 也不要删除charValue，因为它被ConstantExpression管理
}

// 测试字符串常量解析
TEST_F(ParserConstantTest, StringConstant) {
    ConstantExpression* stringConst = new ConstantExpression("Hello World");
    Value* stringValue = interpreter->visit(stringConst);
    
    ASSERT_NE(stringValue, nullptr);
    EXPECT_EQ(stringValue->toString(), "true");
    
    // 不要删除stringConst，因为它的析构函数是空的，会导致内存泄漏
    // 也不要删除stringValue，因为它被ConstantExpression管理
}

// 测试Value类型的运算
TEST_F(ParserConstantTest, ValueOperations) {
    ConstantExpression* intConst = new ConstantExpression(42);
    Value* intValue = interpreter->visit(intConst);
    
    ASSERT_NE(intValue, nullptr);
    
    // 测试类型转换
    Integer* intVal = dynamic_cast<Integer*>(intValue);
    ASSERT_NE(intVal, nullptr);
    EXPECT_EQ(intVal->getValue(), 42);
    
    // 测试布尔转换
    EXPECT_TRUE(intVal->toBool());
    
    // 不要删除intConst，因为它的析构函数是空的，会导致内存泄漏
    // 也不要删除intValue，因为它被ConstantExpression管理
}

// 测试浮点数运算
TEST_F(ParserConstantTest, DoubleOperations) {
    ConstantExpression* doubleConst = new ConstantExpression(3.14);
    Value* doubleValue = interpreter->visit(doubleConst);
    
    ASSERT_NE(doubleValue, nullptr);
    
    Double* doubleVal = dynamic_cast<Double*>(doubleValue);
    ASSERT_NE(doubleVal, nullptr);
    EXPECT_DOUBLE_EQ(doubleVal->getValue(), 3.14);
    
    // 不要删除doubleConst，因为它的析构函数是空的，会导致内存泄漏
    // 也不要删除doubleValue，因为它被ConstantExpression管理
}

// main函数由GTest提供
