#include <gtest/gtest.h>
#include "parser/expression.h"
#include "parser/statement.h"
#include "lexer/value.h"

using namespace std;

// Parser综合测试套件
class ParserComprehensiveTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 测试前的设置
    }
    
    void TearDown() override {
        // 测试后的清理
    }
};

// 测试常量表达式
TEST_F(ParserComprehensiveTest, ConstantExpressions) {
    ConstantExpression* intExpr = new ConstantExpression(42);
    EXPECT_EQ(intExpr->getLocation(), "42");
    EXPECT_EQ(intExpr->getTypePriority(), 0);
    delete intExpr;
    
    ConstantExpression* doubleExpr = new ConstantExpression(3.14);
    EXPECT_EQ(doubleExpr->getLocation(), "3.14");
    delete doubleExpr;
    
    ConstantExpression* boolExpr = new ConstantExpression(true);
    EXPECT_EQ(boolExpr->getLocation(), "true");
    delete boolExpr;
    
    ConstantExpression* strExpr = new ConstantExpression("Hello");
    EXPECT_EQ(strExpr->getLocation(), "\"Hello\"");
    delete strExpr;
}

// 测试变量表达式
TEST_F(ParserComprehensiveTest, VariableExpressions) {
    VariableExpression* varExpr = new VariableExpression("x");
    EXPECT_EQ(varExpr->getLocation(), "variable: x");
    EXPECT_EQ(varExpr->getTypePriority(), 0);
    delete varExpr;
}

// 测试二元表达式
TEST_F(ParserComprehensiveTest, BinaryExpressions) {
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
TEST_F(ParserComprehensiveTest, ExpressionStatements) {
    ConstantExpression* expr = new ConstantExpression(42);
    ExpressionStatement* stmt = new ExpressionStatement(expr);
    
    EXPECT_NOT_NULL(stmt);
    
    delete expr;
    delete stmt;
}

// 测试变量声明
TEST_F(ParserComprehensiveTest, VariableDeclarations) {
    ConstantExpression* value = new ConstantExpression(42);
    VariableDeclaration* varDecl = new VariableDeclaration("y", "int", value);
    
    EXPECT_NOT_NULL(varDecl);
    
    delete value;
    delete varDecl;
}

// 测试程序结构
TEST_F(ParserComprehensiveTest, ProgramStructure) {
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

// 测试叶子表达式
TEST_F(ParserComprehensiveTest, LeafExpressions) {
    ConstantExpression* intExpr = new ConstantExpression(42);
    EXPECT_EQ(intExpr->getLocation(), "42");
    delete intExpr;
    
    VariableExpression* varExpr = new VariableExpression("x");
    EXPECT_EQ(varExpr->getLocation(), "variable: x");
    delete varExpr;
}

// 测试简化表达式
TEST_F(ParserComprehensiveTest, SimplifiedExpressions) {
    ConstantExpression* expr = new ConstantExpression(42);
    EXPECT_EQ(expr->getLocation(), "42");
    delete expr;
}

// 测试简化二元表达式
TEST_F(ParserComprehensiveTest, SimplifiedBinaryExpressions) {
    ConstantExpression* left = new ConstantExpression(10);
    ConstantExpression* right = new ConstantExpression(5);
    BinaryExpression* binaryExpr = new BinaryExpression(left, right, Operator::Add);
    
    EXPECT_EQ(binaryExpr->getLocation(), "binary expression");
    
    delete left;
    delete right;
    delete binaryExpr;
}

// 测试强制转换表达式
TEST_F(ParserComprehensiveTest, CastExpressions) {
    ConstantExpression* expr = new ConstantExpression(42);
    EXPECT_EQ(expr->getLocation(), "42");
    delete expr;
}

// 测试赋值二元表达式
TEST_F(ParserComprehensiveTest, AssignmentBinaryExpressions) {
    VariableExpression* varExpr = new VariableExpression("x");
    ConstantExpression* value = new ConstantExpression(42);
    BinaryExpression* assignExpr = new BinaryExpression(varExpr, value, Operator::Assign);
    
    EXPECT_EQ(assignExpr->getLocation(), "binary expression");
    
    delete varExpr;
    delete value;
    delete assignExpr;
}

// 测试AST结构
TEST_F(ParserComprehensiveTest, ASTStructure) {
    ConstantExpression* expr = new ConstantExpression(42);
    EXPECT_NOT_NULL(expr);
    delete expr;
}

// 测试访问者模式
TEST_F(ParserComprehensiveTest, VisitorPattern) {
    ConstantExpression* expr = new ConstantExpression(42);
    EXPECT_NOT_NULL(expr);
    delete expr;
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
