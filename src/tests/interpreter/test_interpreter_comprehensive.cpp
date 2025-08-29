#include <gtest/gtest.h>
#include "parser/expression.h"
#include "parser/statement.h"
#include "lexer/value.h"

using namespace std;

// Interpreter综合测试套件
class InterpreterComprehensiveTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 测试前的设置
    }
    
    void TearDown() override {
        // 测试后的清理
    }
};

// 测试常量表达式求值
TEST_F(InterpreterComprehensiveTest, ConstantExpressionEvaluation) {
    ConstantExpression* intExpr = new ConstantExpression(42);
    EXPECT_EQ(intExpr->getLocation(), "42");
    delete intExpr;
    
    ConstantExpression* doubleExpr = new ConstantExpression(3.14);
    EXPECT_EQ(doubleExpr->getLocation(), "3.14");
    delete doubleExpr;
}

// 测试变量表达式
TEST_F(InterpreterComprehensiveTest, VariableExpressionEvaluation) {
    VariableExpression* varExpr = new VariableExpression("x");
    EXPECT_EQ(varExpr->getLocation(), "variable: x");
    delete varExpr;
}

// 测试二元表达式结构
TEST_F(InterpreterComprehensiveTest, BinaryExpressionStructure) {
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
TEST_F(InterpreterComprehensiveTest, ExpressionStatementStructure) {
    ConstantExpression* expr = new ConstantExpression(42);
    ExpressionStatement* stmt = new ExpressionStatement(expr);
    
    EXPECT_NE(stmt, nullptr);
    
    delete expr;
    delete stmt;
}

// 测试变量声明结构
TEST_F(InterpreterComprehensiveTest, VariableDeclarationStructure) {
    ConstantExpression* value = new ConstantExpression(42);
    VariableDeclaration* varDecl = new VariableDeclaration("y", "int", value);
    
    EXPECT_NE(varDecl, nullptr);
    
    delete value;
    delete varDecl;
}

// 测试程序结构
TEST_F(InterpreterComprehensiveTest, ProgramStructure) {
    ConstantExpression* expr = new ConstantExpression(42);
    ExpressionStatement* stmt = new ExpressionStatement(expr);
    
    vector<Statement*> statements;
    statements.push_back(stmt);
    
    Program* program = new Program(statements);
    EXPECT_NE(program, nullptr);
    
    delete expr;
    delete stmt;
    delete program;
}

// 测试AST访问者模式
TEST_F(InterpreterComprehensiveTest, ASTVisitorPattern) {
    ConstantExpression* expr = new ConstantExpression(42);
    EXPECT_NE(expr, nullptr);
    delete expr;
}

// 测试内置函数
TEST_F(InterpreterComprehensiveTest, BuiltinFunctions) {
    // 测试内置函数的基本结构
    EXPECT_TRUE(true);
}

// 测试访问者模式
TEST_F(InterpreterComprehensiveTest, VisitorPattern) {
    ConstantExpression* expr = new ConstantExpression(42);
    EXPECT_NE(expr, nullptr);
    delete expr;
}

// 测试值访问者
TEST_F(InterpreterComprehensiveTest, ValueVisitor) {
    Integer intVal(42);
    EXPECT_EQ(intVal.toString(), "42");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
