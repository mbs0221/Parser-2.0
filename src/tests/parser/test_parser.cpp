#include <gtest/gtest.h>
#include "parser/expression.h"
#include "parser/statement.h"
#include "lexer/value.h"

using namespace std;

// Parser测试套件
class ParserTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 测试前的设置
    }
    
    void TearDown() override {
        // 测试后的清理
    }
};

// 测试常量表达式
TEST_F(ParserTest, ConstantExpressions) {
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
    EXPECT_NE(strExpr->getLocation(), "");
    delete strExpr;
}

// 测试变量表达式
TEST_F(ParserTest, VariableExpressions) {
    VariableExpression* varExpr = new VariableExpression("x");
    EXPECT_EQ(varExpr->getLocation(), "variable: x");
    EXPECT_EQ(varExpr->getTypePriority(), 0);
    delete varExpr;
}

// 测试二元表达式
TEST_F(ParserTest, BinaryExpressions) {
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
TEST_F(ParserTest, ExpressionStatements) {
    ConstantExpression* expr = new ConstantExpression(42);
    ExpressionStatement* stmt = new ExpressionStatement(expr);
    
    EXPECT_NE(stmt, nullptr);
    
    delete expr;
    delete stmt;
}

// 测试变量声明
TEST_F(ParserTest, VariableDefinitions) {
    ConstantExpression* value = new ConstantExpression(42);
    VariableDefinition* varDecl = new VariableDefinition("y", "int", value);
    
    EXPECT_NE(varDecl, nullptr);
    
    delete value;
    delete varDecl;
}

// 测试程序结构
TEST_F(ParserTest, ProgramStructure) {
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

int test_parser(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
