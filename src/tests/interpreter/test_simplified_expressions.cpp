#include <gtest/gtest.h>
#include "parser/expression.h"
#include "interpreter/interpreter.h"
#include "lexer/value.h"
#include <iostream>

using namespace std;

class SimplifiedExpressionsTest : public ::testing::Test {
protected:
    Interpreter interpreter;
    
    void SetUp() override {
        // 初始化设置
    }
    
    void TearDown() override {
        // 清理资源
    }
};

// 测试整数常量表达式
TEST_F(SimplifiedExpressionsTest, IntegerConstant) {
    ConstantExpression* intConst = new ConstantExpression(42);
    Value* intValue = interpreter.visit(intConst);
    
    ASSERT_NE(intValue, nullptr);
    EXPECT_EQ(intValue->toString(), "42");
    
    // 不要删除intConst，因为它的析构函数是空的，会导致内存泄漏
    // 也不要删除intValue，因为它被ConstantExpression管理
}

// 测试浮点数常量表达式
TEST_F(SimplifiedExpressionsTest, DoubleConstant) {
    ConstantExpression* doubleConst = new ConstantExpression(3.14);
    Value* doubleValue = interpreter.visit(doubleConst);
    
    ASSERT_NE(doubleValue, nullptr);
    EXPECT_EQ(doubleValue->toString(), "3.140000");
    
    // 不要删除doubleConst，因为它的析构函数是空的，会导致内存泄漏
    // 也不要删除doubleValue，因为它被ConstantExpression管理
}

// 测试布尔常量表达式
TEST_F(SimplifiedExpressionsTest, BoolConstant) {
    ConstantExpression* boolConst = new ConstantExpression(true);
    Value* boolValue = interpreter.visit(boolConst);
    
    ASSERT_NE(boolValue, nullptr);
    EXPECT_EQ(boolValue->toString(), "true");
    
    // 不要删除boolConst，因为它的析构函数是空的，会导致内存泄漏
    // 也不要删除boolValue，因为它被ConstantExpression管理
}

// 测试字符常量表达式
TEST_F(SimplifiedExpressionsTest, CharConstant) {
    ConstantExpression* charConst = new ConstantExpression('A');
    Value* charValue = interpreter.visit(charConst);
    
    ASSERT_NE(charValue, nullptr);
    EXPECT_EQ(charValue->toString(), "A");
    
    // 不要删除charConst，因为它的析构函数是空的，会导致内存泄漏
    // 也不要删除charValue，因为它被ConstantExpression管理
}

// 测试字符串常量表达式
TEST_F(SimplifiedExpressionsTest, StringConstant) {
    ConstantExpression* stringConst = new ConstantExpression("Hello");
    Value* stringValue = interpreter.visit(stringConst);
    
    ASSERT_NE(stringValue, nullptr);
    EXPECT_EQ(stringValue->toString(), "\"Hello\"");
    
    // 不要删除stringConst，因为它的析构函数是空的，会导致内存泄漏
    // 也不要删除stringValue，因为它被ConstantExpression管理
}

// 测试变量声明
TEST_F(SimplifiedExpressionsTest, VariableDeclaration) {
    // 创建变量声明
    VariableDeclaration* varDecl = new VariableDeclaration("x", "int");
    interpreter.visit(varDecl);
    
    // 变量应该被正确声明
    // 这里我们只是测试声明是否成功，不测试访问
    
    delete varDecl;
}

// 测试Value类型的运算
TEST_F(SimplifiedExpressionsTest, ValueOperations) {
    ConstantExpression* intConst = new ConstantExpression(42);
    Value* intValue = interpreter.visit(intConst);
    
    ASSERT_NE(intValue, nullptr);
    
    Integer* intVal = dynamic_cast<Integer*>(intValue);
    ASSERT_NE(intVal, nullptr);
    EXPECT_EQ(intVal->getValue(), 42);
    
    // 不要删除intConst，因为它的析构函数是空的，会导致内存泄漏
    // 也不要删除intValue，因为它被ConstantExpression管理
}

// 测试浮点数运算
TEST_F(SimplifiedExpressionsTest, DoubleOperations) {
    ConstantExpression* doubleConst = new ConstantExpression(3.14);
    Value* doubleValue = interpreter.visit(doubleConst);
    
    ASSERT_NE(doubleValue, nullptr);
    
    Double* doubleVal = dynamic_cast<Double*>(doubleValue);
    ASSERT_NE(doubleVal, nullptr);
    EXPECT_DOUBLE_EQ(doubleVal->getValue(), 3.14);
    
    // 不要删除doubleConst，因为它的析构函数是空的，会导致内存泄漏
    // 也不要删除doubleValue，因为它被ConstantExpression管理
}

// 测试布尔运算
TEST_F(SimplifiedExpressionsTest, BoolOperations) {
    ConstantExpression* boolConst = new ConstantExpression(true);
    Value* boolValue = interpreter.visit(boolConst);
    
    ASSERT_NE(boolValue, nullptr);
    
    Bool* boolVal = dynamic_cast<Bool*>(boolValue);
    ASSERT_NE(boolVal, nullptr);
    EXPECT_TRUE(boolVal->getValue());
    
    // 不要删除boolConst，因为它的析构函数是空的，会导致内存泄漏
    // 也不要删除boolValue，因为它被ConstantExpression管理
}

int test_simplified_expressions_main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
