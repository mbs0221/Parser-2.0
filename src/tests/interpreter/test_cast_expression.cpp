#include <gtest/gtest.h>
#include "lexer/value.h"
#include "parser/expression.h"
#include "interpreter/interpreter.h"

using namespace std;

class CastExpressionTest : public ::testing::Test {
protected:
    Interpreter* interpreter;
    
    void SetUp() override {
        interpreter = new Interpreter(false);
    }
    
    void TearDown() override {
        delete interpreter;
    }
};

TEST_F(CastExpressionTest, ExplicitTypeConversion) {
    // 整数转浮点数
    ConstantExpression* intExpr = new ConstantExpression(42);
    CastExpression<Double>* intToDouble = new CastExpression<Double>(intExpr);
    Value* doubleResult = interpreter->visit(intToDouble);
    
    ASSERT_NE(doubleResult, nullptr);
    Double* doubleVal = dynamic_cast<Double*>(doubleResult);
    ASSERT_NE(doubleVal, nullptr);
    EXPECT_DOUBLE_EQ(doubleVal->getValue(), 42.0);
    
    delete intExpr;
    delete intToDouble;
}

TEST_F(CastExpressionTest, DoubleToIntConversion) {
    // 浮点数转整数
    ConstantExpression* doubleExpr = new ConstantExpression(3.7);
    CastExpression<Integer>* doubleToInt = new CastExpression<Integer>(doubleExpr);
    Value* intResult = interpreter->visit(doubleToInt);
    
    ASSERT_NE(intResult, nullptr);
    Integer* intVal = dynamic_cast<Integer*>(intResult);
    ASSERT_NE(intVal, nullptr);
    EXPECT_EQ(intVal->getValue(), 3);
    
    delete doubleExpr;
    delete doubleToInt;
}

TEST_F(CastExpressionTest, IntToBoolConversion) {
    // 整数转布尔
    ConstantExpression* zeroExpr = new ConstantExpression(0);
    CastExpression<Bool>* intToBool = new CastExpression<Bool>(zeroExpr);
    Value* boolResult = interpreter->visit(intToBool);
    
    ASSERT_NE(boolResult, nullptr);
    Bool* boolVal = dynamic_cast<Bool*>(boolResult);
    ASSERT_NE(boolVal, nullptr);
    EXPECT_FALSE(boolVal->getValue());
    
    delete zeroExpr;
    delete intToBool;
}

TEST_F(CastExpressionTest, AutoTypeConversion) {
    // 整数 + 浮点数
    ConstantExpression* leftInt = new ConstantExpression(10);
    ConstantExpression* rightDouble = new ConstantExpression(3.5);
    Operator* plusOp = new Operator('+', "+", 1, true);
    BinaryExpression* mixedOp = new BinaryExpression(leftInt, rightDouble, plusOp);
    Value* mixedResult = interpreter->visit(mixedOp);
    
    ASSERT_NE(mixedResult, nullptr);
    Double* doubleVal = dynamic_cast<Double*>(mixedResult);
    ASSERT_NE(doubleVal, nullptr);
    EXPECT_DOUBLE_EQ(doubleVal->getValue(), 13.5);
    
    delete leftInt;
    delete rightDouble;
    delete plusOp;
    delete mixedOp;
}
