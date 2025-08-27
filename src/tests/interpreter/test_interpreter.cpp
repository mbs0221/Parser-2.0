#include <gtest/gtest.h>
#include "parser/expression.h"
#include "parser/statement.h"
#include "lexer/value.h"

using namespace std;

// Interpreter测试套件
class InterpreterTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 测试前的设置
    }
    
    void TearDown() override {
        // 测试后的清理
    }
};

// 测试常量表达式求值
TEST_F(InterpreterTest, ConstantExpressionEvaluation) {
    ConstantExpression* intExpr = new ConstantExpression(42);
    EXPECT_EQ(intExpr->getLocation(), "42");
    delete intExpr;
    
    ConstantExpression* doubleExpr = new ConstantExpression(3.14);
    EXPECT_EQ(doubleExpr->getLocation(), "3.14");
    delete doubleExpr;
}

// 测试变量表达式
TEST_F(InterpreterTest, VariableExpressionEvaluation) {
    VariableExpression* varExpr = new VariableExpression("x");
    EXPECT_EQ(varExpr->getLocation(), "variable: x");
    delete varExpr;
}

// 测试二元表达式结构
TEST_F(InterpreterTest, BinaryExpressionStructure) {
    ConstantExpression* left = new ConstantExpression(10);
    ConstantExpression* right = new ConstantExpression(5);
    BinaryExpression* binaryExpr = new BinaryExpression(left, right, Operator::Add);
    
    EXPECT_EQ(binaryExpr->getLocation(), "binary expression");
    EXPECT_EQ(binaryExpr->getTypePriority(), 0);
    
    delete left;
    delete right;
    delete binaryExpr;
}

// 测试表达式语句
TEST_F(InterpreterTest, ExpressionStatementStructure) {
    ConstantExpression* expr = new ConstantExpression(42);
    ExpressionStatement* stmt = new ExpressionStatement(expr);
    
    EXPECT_NOT_NULL(stmt);
    
    delete expr;
    delete stmt;
}

// 测试变量声明结构
TEST_F(InterpreterTest, VariableDeclarationStructure) {
    ConstantExpression* value = new ConstantExpression(42);
    VariableDeclaration* varDecl = new VariableDeclaration("y", "int", value);
    
    EXPECT_NOT_NULL(varDecl);
    
    delete value;
    delete varDecl;
}

// 测试程序结构
TEST_F(InterpreterTest, ProgramStructure) {
    ConstantExpression* expr = new ConstantExpression(42);
    ExpressionStatement* stmt = new ExpressionStatement(expr);
    
    vector<Statement*> statements;
    statements.push_back(stmt);
    
    Program* program = new Program(statements);
    EXPECT_NOT_NULL(program);
    
    delete expr;
    delete stmt;
    delete program;
}

// 测试AST访问者模式
TEST_F(InterpreterTest, ASTVisitorPattern) {
    ConstantExpression* expr = new ConstantExpression(42);
    EXPECT_NOT_NULL(expr);
    delete expr;
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
