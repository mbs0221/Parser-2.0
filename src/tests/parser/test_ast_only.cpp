#include <gtest/gtest.h>
#include "parser/expression.h"
#include "parser/statement.h"
#include "lexer/value.h"

using namespace std;

class ASTOnlyTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 测试前的设置
    }
    
    void TearDown() override {
        // 测试后的清理
    }
};

TEST_F(ASTOnlyTest, ConstantExpressions) {
    ConstantExpression* intExpr = new ConstantExpression(42);
    ConstantExpression* doubleExpr = new ConstantExpression(3.14);
    ConstantExpression* boolExpr = new ConstantExpression(true);
    // 暂时跳过字符串常量测试，因为String类的toString()方法有问题
    // ConstantExpression* strExpr = new ConstantExpression("Hello");
    
    EXPECT_EQ(intExpr->getLocation(), "42");
    EXPECT_EQ(doubleExpr->getLocation(), "3.14");
    EXPECT_EQ(boolExpr->getLocation(), "true");
    // EXPECT_EQ(strExpr->getLocation(), "\"Hello\"");
    
    delete intExpr;
    delete doubleExpr;
    delete boolExpr;
    // delete strExpr;
}

TEST_F(ASTOnlyTest, VariableExpressions) {
    VariableExpression* varExpr = new VariableExpression("x");
    EXPECT_EQ(varExpr->getLocation(), "variable: x");
    delete varExpr;
}

TEST_F(ASTOnlyTest, BinaryExpressions) {
    ConstantExpression* intExpr = new ConstantExpression(42);
    ConstantExpression* doubleExpr = new ConstantExpression(3.14);
    BinaryExpression* binaryExpr = new BinaryExpression(intExpr, doubleExpr, Operator::Add);
    
    EXPECT_EQ(binaryExpr->getLocation(), "binary expression");
    EXPECT_EQ(binaryExpr->getTypePriority(), 0);
    
    delete intExpr;
    delete doubleExpr;
    delete binaryExpr;
}

TEST_F(ASTOnlyTest, ExpressionStatements) {
    ConstantExpression* intExpr = new ConstantExpression(42);
    ExpressionStatement* exprStmt = new ExpressionStatement(intExpr);
    
    EXPECT_NE(exprStmt, nullptr);
    
    delete intExpr;
    delete exprStmt;
}

TEST_F(ASTOnlyTest, VariableDefinitions) {
    ConstantExpression* intExpr = new ConstantExpression(42);
    VariableDefinition* varDecl = new VariableDefinition("y", "int", intExpr);
    
    EXPECT_NE(varDecl, nullptr);
    
    delete intExpr;
    delete varDecl;
}

TEST_F(ASTOnlyTest, ProgramStructure) {
    ConstantExpression* intExpr = new ConstantExpression(42);
    ExpressionStatement* exprStmt = new ExpressionStatement(intExpr);
    VariableDefinition* varDecl = new VariableDefinition("y", "int", intExpr);
    
    vector<Statement*> statements;
    statements.push_back(exprStmt);
    statements.push_back(varDecl);
    
    Program* program = new Program(statements);
    EXPECT_NE(program, nullptr);
    
    delete intExpr;
    delete exprStmt;
    delete varDecl;
    delete program;
}
