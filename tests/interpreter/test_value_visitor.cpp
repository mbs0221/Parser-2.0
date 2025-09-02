#include <gtest/gtest.h>
#include "lexer/value.h"
#include "parser/expression.h"
#include "interpreter/interpreter.h"

using namespace std;

class ValueVisitorTest : public ::testing::Test {
protected:
    Interpreter* interpreter;
    
    void SetUp() override {
        interpreter = new Interpreter(false);
    }
    
    void TearDown() override {
        delete interpreter;
    }
};

TEST_F(ValueVisitorTest, IntegerExpression) {
    ConstantExpression* intExpr = new ConstantExpression(42);
    Value* intResult = interpreter->visit(intExpr);
    
    ASSERT_NE(intResult, nullptr);
    EXPECT_EQ(intResult->toString(), "42");
    
    delete intExpr;
}

TEST_F(ValueVisitorTest, DoubleExpression) {
    ConstantExpression* doubleExpr = new ConstantExpression(3.14);
    Value* doubleResult = interpreter->visit(doubleExpr);
    
    ASSERT_NE(doubleResult, nullptr);
    EXPECT_EQ(doubleResult->toString(), "3.14");
    
    delete doubleExpr;
}

TEST_F(ValueVisitorTest, BoolExpression) {
    ConstantExpression* boolExpr = new ConstantExpression(true);
    Value* boolResult = interpreter->visit(boolExpr);
    
    ASSERT_NE(boolResult, nullptr);
    EXPECT_EQ(boolResult->toString(), "true");
    
    delete boolExpr;
}

TEST_F(ValueVisitorTest, BinaryExpression) {
    ConstantExpression* left = new ConstantExpression(10);
    ConstantExpression* right = new ConstantExpression(5);
    Operator* plusToken = new Operator('+', "+", 1, true);
    BinaryExpression* binaryExpr = new BinaryExpression(left, right, plusToken);
    
    Value* binaryResult = interpreter->visit(binaryExpr);
    
    ASSERT_NE(binaryResult, nullptr);
    Integer* intVal = dynamic_cast<Integer*>(binaryResult);
    ASSERT_NE(intVal, nullptr);
    EXPECT_EQ(intVal->getValue(), 15);
    
    delete left;
    delete right;
    delete plusToken;
    delete binaryExpr;
}
