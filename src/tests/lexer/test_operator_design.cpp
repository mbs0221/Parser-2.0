#include <gtest/gtest.h>
#include <string>
#include "lexer/lexer.h"
#include "lexer/value.h"

using namespace std;

class OperatorDesignTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 在每个测试用例前设置
    }
    
    void TearDown() override {
        // 在每个测试用例后清理
    }
};

// 测试Operator的创建和基本属性
TEST_F(OperatorDesignTest, OperatorCreation) {
    Operator* addOp = new Operator('+', "+", 4, true);
    Operator* mulOp = new Operator('*', "*", 5, true);
    Operator* assignOp = new Operator('=', "=", 2, false);
    Operator* eqOp = new Operator('=', "==", 7, true);
    
    // 测试加法运算符
    EXPECT_EQ(addOp->getSymbol(), "+");
    EXPECT_EQ(addOp->getPrecedence(), 4);
    EXPECT_TRUE(addOp->isLeftAssoc());
    
    // 测试乘法运算符
    EXPECT_EQ(mulOp->getSymbol(), "*");
    EXPECT_EQ(mulOp->getPrecedence(), 5);
    EXPECT_TRUE(mulOp->isLeftAssoc());
    
    // 测试赋值运算符
    EXPECT_EQ(assignOp->getSymbol(), "=");
    EXPECT_EQ(assignOp->getPrecedence(), 2);
    EXPECT_FALSE(assignOp->isLeftAssoc());
    
    // 测试相等运算符
    EXPECT_EQ(eqOp->getSymbol(), "==");
    EXPECT_EQ(eqOp->getPrecedence(), 7);
    EXPECT_TRUE(eqOp->isLeftAssoc());
    
    // 清理内存
    delete addOp;
    delete mulOp;
    delete assignOp;
    delete eqOp;
}

// 测试Value类型的基本功能
TEST_F(OperatorDesignTest, ValueTypes) {
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

// 测试Value类型的边界情况
TEST_F(OperatorDesignTest, ValueEdgeCases) {
    // 测试零值
    Integer* zeroInt = new Integer(0);
    Double* zeroDouble = new Double(0.0);
    Bool* falseBool = new Bool(false);
    
    EXPECT_FALSE(zeroInt->toBool());
    EXPECT_FALSE(zeroDouble->toBool());
    EXPECT_FALSE(falseBool->toBool());
    
    // 测试空字符
    Char* nullChar = new Char('\0');
    EXPECT_FALSE(nullChar->toBool());
    
    // 清理内存
    delete zeroInt;
    delete zeroDouble;
    delete falseBool;
    delete nullChar;
}

// 测试Operator的优先级比较
TEST_F(OperatorDesignTest, OperatorPrecedence) {
    Operator* addOp = new Operator('+', "+", 4, true);
    Operator* mulOp = new Operator('*', "*", 5, true);
    Operator* assignOp = new Operator('=', "=", 2, false);
    
    // 测试优先级关系
    EXPECT_GT(mulOp->getPrecedence(), addOp->getPrecedence());
    EXPECT_GT(addOp->getPrecedence(), assignOp->getPrecedence());
    EXPECT_EQ(mulOp->getPrecedence(), 5);
    
    // 清理内存
    delete addOp;
    delete mulOp;
    delete assignOp;
}

