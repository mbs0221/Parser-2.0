#include <gtest/gtest.h>
#include "parser/expression.h"
#include "parser/statement.h"
#include "lexer/value.h"
#include <iostream>

using namespace std;

class SimpleParserTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 初始化设置
    }
    
    void TearDown() override {
        // 清理资源
    }
};

// 测试ConstantExpression的创建
TEST_F(SimpleParserTest, ConstantExpressionCreation) {
    ConstantExpression* intConst = new ConstantExpression(42);
    ConstantExpression* doubleConst = new ConstantExpression(3.14);
    ConstantExpression* boolConst = new ConstantExpression(true);
    ConstantExpression* charConst = new ConstantExpression('A');
    
    ASSERT_NE(intConst, nullptr);
    ASSERT_NE(doubleConst, nullptr);
    ASSERT_NE(boolConst, nullptr);
    ASSERT_NE(charConst, nullptr);
    
    // 测试value指针不为空
    EXPECT_NE(intConst->value, nullptr);
    EXPECT_NE(doubleConst->value, nullptr);
    EXPECT_NE(boolConst->value, nullptr);
    EXPECT_NE(charConst->value, nullptr);
    
    // 测试类型转换
    Integer* intVal = dynamic_cast<Integer*>(intConst->value);
    Double* doubleVal = dynamic_cast<Double*>(doubleConst->value);
    Bool* boolVal = dynamic_cast<Bool*>(boolConst->value);
    Char* charVal = dynamic_cast<Char*>(charConst->value);
    
    ASSERT_NE(intVal, nullptr);
    ASSERT_NE(doubleVal, nullptr);
    ASSERT_NE(boolVal, nullptr);
    ASSERT_NE(charVal, nullptr);
    
    EXPECT_EQ(intVal->getValue(), 42);
    EXPECT_DOUBLE_EQ(doubleVal->getValue(), 3.14);
    EXPECT_TRUE(boolVal->getValue());
    EXPECT_EQ(charVal->getValue(), 'A');
    
    // 不要删除ConstantExpression，因为它的析构函数是空的
    // 也不要删除value指针，因为它们被ConstantExpression管理
}

// 测试VariableExpression的创建
TEST_F(SimpleParserTest, VariableExpressionCreation) {
    VariableExpression* varExpr = new VariableExpression("x");
    
    ASSERT_NE(varExpr, nullptr);
    EXPECT_EQ(varExpr->name, "x");
    EXPECT_EQ(varExpr->getLocation(), "variable: x");
    EXPECT_EQ(varExpr->getTypePriority(), 0);
    
    delete varExpr;
}

// 测试BinaryExpression的创建
TEST_F(SimpleParserTest, BinaryExpressionCreation) {
    ConstantExpression* left = new ConstantExpression(10);
    ConstantExpression* right = new ConstantExpression(5);
    Operator* op = new Operator('+', "+", 4, true);
    
    BinaryExpression* binaryExpr = new BinaryExpression(left, right, op);
    
    ASSERT_NE(binaryExpr, nullptr);
    EXPECT_EQ(binaryExpr->left, left);
    EXPECT_EQ(binaryExpr->right, right);
    EXPECT_EQ(binaryExpr->operator_, op);
    EXPECT_EQ(binaryExpr->getLocation(), "binary expression");
    EXPECT_EQ(binaryExpr->getTypePriority(), 0); // 因为left和right的优先级都是0
    
    // 不要删除ConstantExpression，因为它们的析构函数是空的
    delete binaryExpr;
    delete op;
}

// 测试VariableDeclaration的创建
TEST_F(SimpleParserTest, VariableDeclarationCreation) {
    VariableDeclaration* varDecl = new VariableDeclaration("x", "int");
    
    ASSERT_NE(varDecl, nullptr);
    EXPECT_EQ(varDecl->variables.size(), 1);
    EXPECT_EQ(varDecl->variables[0].first, "x");
    EXPECT_EQ(varDecl->variables[0].second, nullptr); // 没有初始值
    
    delete varDecl;
}

// 测试VariableDeclaration带初始值
TEST_F(SimpleParserTest, VariableDeclarationWithInitialValue) {
    ConstantExpression* initValue = new ConstantExpression(42);
    VariableDeclaration* varDecl = new VariableDeclaration("x", "int");
    varDecl->variables[0].second = initValue;
    
    ASSERT_NE(varDecl, nullptr);
    EXPECT_EQ(varDecl->variables.size(), 1);
    EXPECT_EQ(varDecl->variables[0].first, "x");
    EXPECT_EQ(varDecl->variables[0].second, initValue);
    
    // 不要删除ConstantExpression，因为它的析构函数是空的
    delete varDecl;
}

int test_simple_parser_main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
