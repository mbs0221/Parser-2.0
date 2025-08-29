#include <gtest/gtest.h>
#include "parser/expression.h"
#include "lexer/value.h"

using namespace std;

class VisitorTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 测试前的设置
    }
    
    void TearDown() override {
        // 测试后的清理
    }
};

TEST_F(VisitorTest, ConstantExpressionVisitor) {
    ConstantExpression* intExpr = new ConstantExpression(42);
    EXPECT_NE(intExpr, nullptr);
    EXPECT_EQ(intExpr->getLocation(), "42");
    delete intExpr;
}

TEST_F(VisitorTest, VariableExpressionVisitor) {
    VariableExpression* varExpr = new VariableExpression("x");
    EXPECT_NE(varExpr, nullptr);
    EXPECT_EQ(varExpr->getLocation(), "variable: x");
    delete varExpr;
}

TEST_F(VisitorTest, BinaryExpressionVisitor) {
    ConstantExpression* left = new ConstantExpression(10);
    ConstantExpression* right = new ConstantExpression(5);
    BinaryExpression* binaryExpr = new BinaryExpression(left, right, Operator::Add);
    
    EXPECT_NE(binaryExpr, nullptr);
    EXPECT_EQ(binaryExpr->getLocation(), "binary expression");
    
    delete left;
    delete right;
    delete binaryExpr;
}
