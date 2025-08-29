#include <gtest/gtest.h>
#include "lexer/value.h"
#include "parser/expression.h"
#include <iostream>

using namespace std;

class LeafExpressionTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 初始化设置
    }
    
    void TearDown() override {
        // 清理资源
    }
};

// 测试ConstantExpression基本功能
TEST_F(LeafExpressionTest, ConstantExpressionBasic) {
    ConstantExpression* intExpr = new ConstantExpression(42);
    EXPECT_NE(intExpr, nullptr);
    EXPECT_NE(intExpr->value, nullptr);
    EXPECT_EQ(intExpr->value->toString(), "42");
    delete intExpr;
}

// 测试ConstantExpression的Value访问
TEST_F(LeafExpressionTest, ConstantExpressionValueAccess) {
    ConstantExpression* intExpr = new ConstantExpression(42);
    Value* intValue = intExpr->value;
    EXPECT_NE(intValue, nullptr);
    EXPECT_EQ(intValue->toString(), "42");
    delete intExpr;
}

// 测试不同类型的ConstantExpression
TEST_F(LeafExpressionTest, ConstantExpressionTypes) {
    ConstantExpression* doubleExpr = new ConstantExpression(3.14);
    EXPECT_NE(doubleExpr, nullptr);
    EXPECT_NE(doubleExpr->value, nullptr);
    EXPECT_EQ(doubleExpr->value->toString(), "3.14");
    delete doubleExpr;
}

// 测试ConstantExpression的运算符重载
TEST_F(LeafExpressionTest, ConstantExpressionOperators) {
    ConstantExpression* left = new ConstantExpression(10);
    ConstantExpression* right = new ConstantExpression(5);
    
    // 注意：这里需要检查ConstantExpression是否支持运算符重载
    // 如果不支持，我们需要使用Value的运算符重载
    Integer* leftValue = dynamic_cast<Integer*>(left->value);
    Integer* rightValue = dynamic_cast<Integer*>(right->value);
    
    EXPECT_NE(leftValue, nullptr);
    EXPECT_NE(rightValue, nullptr);
    
    if (leftValue && rightValue) {
        Integer result = *leftValue + *rightValue;
        EXPECT_EQ(result.getValue(), 15);
    }
    
    delete left;
    delete right;
}

int test_leaf_expression_main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
