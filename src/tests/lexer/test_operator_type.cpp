#include <gtest/gtest.h>
#include "lexer/lexer.h"
#include "lexer/value.h"
#include "parser/expression.h"
#include "interpreter/interpreter.h"

using namespace std;

class OperatorTypeTest : public ::testing::Test {
protected:
    void SetUp() override {
        interpreter = new Interpreter();
    }
    
    void TearDown() override {
        delete interpreter;
    }
    
    Interpreter* interpreter;
};

// 测试Operator类型和基本属性
TEST_F(OperatorTypeTest, OperatorTypes) {
    Operator* plusOp = new Operator('+', "+", 4, true);
    Operator* eqOp = new Operator(EQ_EQ, "==", 2, true);
    Operator* andOp = new Operator(AND_AND, "&&", 1, true);
    
    EXPECT_EQ(plusOp->getSymbol(), "+");
    EXPECT_EQ(plusOp->getPrecedence(), 4);
    EXPECT_EQ(eqOp->getSymbol(), "==");
    EXPECT_EQ(eqOp->getPrecedence(), 2);
    EXPECT_EQ(andOp->getSymbol(), "&&");
    EXPECT_EQ(andOp->getPrecedence(), 1);
    
    delete plusOp;
    delete eqOp;
    delete andOp;
}

// 测试Value类型和操作符
TEST_F(OperatorTypeTest, ValueTypesAndOperators) {
    Integer* intVal = new Integer(42);
    Double* doubleVal = new Double(3.14);
    Bool* boolVal = new Bool(true);
    String* strVal = new String("hello");
    Char* charVal = new Char('A');
    
    EXPECT_EQ(intVal->toString(), "42");
    EXPECT_EQ(intVal->getTypeName(), "int");
    EXPECT_EQ(doubleVal->toString(), "3.14");
    EXPECT_EQ(doubleVal->getTypeName(), "double");
    EXPECT_EQ(boolVal->toString(), "true");
    EXPECT_EQ(boolVal->getTypeName(), "bool");
    EXPECT_EQ(strVal->toString(), "\"hello\"");
    EXPECT_EQ(strVal->getTypeName(), "string");
    EXPECT_EQ(charVal->toString(), "A");
    EXPECT_EQ(charVal->getTypeName(), "char");
    
    delete intVal;
    delete doubleVal;
    delete boolVal;
    delete strVal;
    delete charVal;
}

// 测试BinaryExpression处理
TEST_F(OperatorTypeTest, BinaryExpressionHandling) {
    ConstantExpression* leftExpr = new ConstantExpression(10);
    ConstantExpression* rightExpr = new ConstantExpression(3.5);
    Operator* plusOp = new Operator('+', "+", 4, true);
    BinaryExpression* binaryExpr = new BinaryExpression(leftExpr, rightExpr, plusOp);
    
    Value* result = interpreter->visit(binaryExpr);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->toString(), "13.5");
    
    delete leftExpr;
    delete rightExpr;
    delete plusOp;
    delete binaryExpr;
    delete result;
}

// 测试比较运算（预期会抛出异常）
TEST_F(OperatorTypeTest, ComparisonOperations) {
    ConstantExpression* boolExpr1 = new ConstantExpression(true);
    ConstantExpression* boolExpr2 = new ConstantExpression(false);
    Operator* lessOp = new Operator('<', "<", 3, true);
    BinaryExpression* compareExpr = new BinaryExpression(boolExpr1, boolExpr2, lessOp);
    
    // 这个操作应该抛出异常，因为不支持Bool类型的比较
    EXPECT_THROW({
        interpreter->visit(compareExpr);
    }, std::runtime_error);
    
    delete boolExpr1;
    delete boolExpr2;
    delete lessOp;
    delete compareExpr;
}

// 测试Value类型的转换
TEST_F(OperatorTypeTest, ValueTypeConversion) {
    Integer* intVal = new Integer(42);
    Double* doubleVal = new Double(3.14);
    
    // 测试类型转换
    EXPECT_EQ(intVal->getTypeName(), "int");
    EXPECT_EQ(doubleVal->getTypeName(), "double");
    
    // 测试布尔转换
    EXPECT_TRUE(intVal->toBool());
    EXPECT_TRUE(doubleVal->toBool());
    
    delete intVal;
    delete doubleVal;
}

