#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include "parser/expression.h"
#include "lexer/value.h"

using namespace std;

class AssignmentBinaryTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 初始化设置
    }
    
    void TearDown() override {
        // 清理资源
    }
};

// 测试赋值表达式作为二元运算符的设计
TEST_F(AssignmentBinaryTest, AssignmentExpressionDesign) {
    // 创建标识符表达式
    VariableExpression* varA = new VariableExpression("a");
    VariableExpression* varB = new VariableExpression("b");
    
    // 创建常量表达式
    ConstantExpression* constExpr = new ConstantExpression(42);
    
    // 创建赋值运算符
    Operator* assignOp = new Operator('=', "=", 2, false);  // 赋值运算符，优先级2，右结合
    
    // 创建赋值表达式：a = 42
    BinaryExpression* assignment = new BinaryExpression(varA, constExpr, assignOp);
    
    EXPECT_NE(assignment, nullptr);
    EXPECT_NE(assignment->left, nullptr);
    EXPECT_NE(assignment->right, nullptr);
    EXPECT_NE(assignment->operator_, nullptr);
    EXPECT_EQ(assignment->operator_->getSymbol(), "=");
    EXPECT_EQ(assignment->operator_->getPrecedence(), 2);
    EXPECT_FALSE(assignment->operator_->isLeftAssoc());
    
    // 测试连续赋值：a = b = 42
    BinaryExpression* nestedAssignment = new BinaryExpression(varA, assignment, assignOp);
    EXPECT_NE(nestedAssignment, nullptr);
    
    // 清理内存
    delete varA;
    delete varB;
    delete constExpr;
    delete assignOp;
    delete assignment;
    delete nestedAssignment;
}

// 测试AssignExpression类
TEST_F(AssignmentBinaryTest, AssignExpressionClass) {
    VariableExpression* var = new VariableExpression("x");
    ConstantExpression* value = new ConstantExpression(100);
    
    AssignExpression* assignExpr = new AssignExpression(var, value);
    
    EXPECT_NE(assignExpr, nullptr);
    EXPECT_NE(assignExpr->left, nullptr);
    EXPECT_NE(assignExpr->right, nullptr);
    EXPECT_NE(assignExpr->operator_, nullptr);
    EXPECT_EQ(assignExpr->operator_->getSymbol(), "=");
    EXPECT_EQ(assignExpr->getTypePriority(), 1);  // 赋值操作优先级最低
    
    delete var;
    delete value;
    delete assignExpr;
}

int test_assignment_binary_main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
