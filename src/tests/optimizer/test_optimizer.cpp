#include <gtest/gtest.h>
#include "optimizer/ast_optimizer.h"
#include "parser/expression.h"
#include "lexer/value.h"
#include <iostream>

using namespace std;

class OptimizerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 初始化设置
    }
    
    void TearDown() override {
        // 清理资源
    }
};

// 测试类型转换优化器
TEST_F(OptimizerTest, TypeCastOptimization) {
    // 创建一个简单的测试表达式：int + double
    ConstantExpression* left = new ConstantExpression(5);
    ConstantExpression* right = new ConstantExpression(3.14);
    
    // 创建二元表达式
    BinaryExpression* expr = new BinaryExpression(left, right, new Operator('+', "+", 4, true));
    
    EXPECT_NE(expr, nullptr);
    EXPECT_NE(left, nullptr);
    EXPECT_NE(right, nullptr);
    
    // 创建优化器
    SimpleOptimizer optimizer;
    
    // 优化表达式
    Expression* optimized = optimizer.optimize(expr);
    
    EXPECT_NE(optimized, nullptr);
    
    // 清理内存
    delete expr;
}

// 测试常量表达式优化
TEST_F(OptimizerTest, ConstantExpressionOptimization) {
    ConstantExpression* intExpr = new ConstantExpression(42);
    ConstantExpression* doubleExpr = new ConstantExpression(3.14);
    ConstantExpression* boolExpr = new ConstantExpression(true);
    ConstantExpression* stringExpr = new ConstantExpression("test");
    
    SimpleOptimizer optimizer;
    
    Expression* optimizedInt = optimizer.optimize(intExpr);
    Expression* optimizedDouble = optimizer.optimize(doubleExpr);
    Expression* optimizedBool = optimizer.optimize(boolExpr);
    Expression* optimizedString = optimizer.optimize(stringExpr);
    
    EXPECT_NE(optimizedInt, nullptr);
    EXPECT_NE(optimizedDouble, nullptr);
    EXPECT_NE(optimizedBool, nullptr);
    EXPECT_NE(optimizedString, nullptr);
    
    delete intExpr;
    delete doubleExpr;
    delete boolExpr;
    delete stringExpr;
}

// 测试一元表达式优化
TEST_F(OptimizerTest, UnaryExpressionOptimization) {
    ConstantExpression* operand = new ConstantExpression(10);
    UnaryExpression* unaryExpr = new UnaryExpression(operand, new Operator('-', "-", 3, false));
    
    SimpleOptimizer optimizer;
    Expression* optimized = optimizer.optimize(unaryExpr);
    
    EXPECT_NE(optimized, nullptr);
    
    delete unaryExpr;
}

// 测试赋值表达式优化
TEST_F(OptimizerTest, AssignExpressionOptimization) {
    VariableExpression* var = new VariableExpression("x");
    ConstantExpression* value = new ConstantExpression(100);
    AssignExpression* assignExpr = new AssignExpression(var, value);
    
    SimpleOptimizer optimizer;
    Expression* optimized = optimizer.optimize(assignExpr);
    
    EXPECT_NE(optimized, nullptr);
    
    delete assignExpr;
}

// 测试类型转换表达式优化
TEST_F(OptimizerTest, CastExpressionOptimization) {
    ConstantExpression* operand = new ConstantExpression(3.14);
    CastExpression<Double>* castExpr = new CastExpression<Double>(operand);
    
    SimpleOptimizer optimizer;
    Expression* optimized = optimizer.optimize(castExpr);
    
    EXPECT_NE(optimized, nullptr);
    
    delete castExpr;
}

int test_optimizer_main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
