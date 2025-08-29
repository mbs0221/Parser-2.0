#include <gtest/gtest.h>
#include "lexer/value.h"
#include "parser/expression.h"
#include "interpreter/interpreter.h"
#include <iostream>

using namespace std;

class ConstantExpressionTest : public ::testing::Test {
protected:
    Interpreter* interpreter;
    
    void SetUp() override {
        // 使用不加载插件的构造函数
        interpreter = new Interpreter(false);
    }
    
    void TearDown() override {
        // 清理资源
        delete interpreter;
    }
};

TEST_F(ConstantExpressionTest, IntegerConstant) {
    ConstantExpression* intConst = new ConstantExpression(42);
    Value* intValue = interpreter->visit(intConst);
    
    ASSERT_NE(intValue, nullptr);
    EXPECT_EQ(intValue->toString(), "42");
    
    Integer* intVal = dynamic_cast<Integer*>(intValue);
    ASSERT_NE(intVal, nullptr);
    EXPECT_EQ(intVal->getValue(), 42);
    
    // 清理内存
    delete intConst;
}

TEST_F(ConstantExpressionTest, DoubleConstant) {
    ConstantExpression* doubleConst = new ConstantExpression(3.14);
    Value* doubleValue = interpreter->visit(doubleConst);
    
    ASSERT_NE(doubleValue, nullptr);
    EXPECT_EQ(doubleValue->toString(), "3.14");
    
    Double* doubleVal = dynamic_cast<Double*>(doubleValue);
    ASSERT_NE(doubleVal, nullptr);
    EXPECT_DOUBLE_EQ(doubleVal->getValue(), 3.14);
    
    // 清理内存
    delete doubleConst;
}

TEST_F(ConstantExpressionTest, BoolConstant) {
    ConstantExpression* boolConst = new ConstantExpression(true);
    Value* boolValue = interpreter->visit(boolConst);
    
    ASSERT_NE(boolValue, nullptr);
    EXPECT_EQ(boolValue->toString(), "true");
    
    Bool* boolVal = dynamic_cast<Bool*>(boolValue);
    ASSERT_NE(boolVal, nullptr);
    EXPECT_TRUE(boolVal->getValue());
    
    // 清理内存
    delete boolConst;
}

TEST_F(ConstantExpressionTest, CharConstant) {
    ConstantExpression* charConst = new ConstantExpression('A');
    Value* charValue = interpreter->visit(charConst);
    
    ASSERT_NE(charValue, nullptr);
    EXPECT_EQ(charValue->toString(), "A");
    
    Char* charVal = dynamic_cast<Char*>(charValue);
    ASSERT_NE(charVal, nullptr);
    EXPECT_EQ(charVal->getValue(), 'A');
    
    // 清理内存
    delete charConst;
}

int test_constant_expression_main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
