#include "parser/expression.h"
#include "parser/statement.h"
#include "lexer/value.h"
#include <iostream>

using namespace std;

int main() {
    cout << "=== AST结构测试 ===" << endl;
    
    // 测试常量表达式
    cout << "1. 测试常量表达式:" << endl;
    ConstantExpression* intExpr = new ConstantExpression(42);
    ConstantExpression* doubleExpr = new ConstantExpression(3.14);
    ConstantExpression* boolExpr = new ConstantExpression(true);
    ConstantExpression* strExpr = new ConstantExpression("Hello");
    
    cout << "   整型表达式: " << intExpr->getLocation() << endl;
    cout << "   浮点表达式: " << doubleExpr->getLocation() << endl;
    cout << "   布尔表达式: " << boolExpr->getLocation() << endl;
    cout << "   字符串表达式: " << strExpr->getLocation() << endl;
    
    // 测试变量表达式
    cout << "\n2. 测试变量表达式:" << endl;
    VariableExpression* varExpr = new VariableExpression("x");
    cout << "   变量表达式: " << varExpr->getLocation() << endl;
    
    // 测试二元表达式
    cout << "\n3. 测试二元表达式:" << endl;
    BinaryExpression* binaryExpr = new BinaryExpression(intExpr, doubleExpr, Operator::Add);
    cout << "   二元表达式: " << binaryExpr->getLocation() << endl;
    cout << "   类型优先级: " << binaryExpr->getTypePriority() << endl;
    
    // 测试表达式语句
    cout << "\n4. 测试表达式语句:" << endl;
    ExpressionStatement* exprStmt = new ExpressionStatement(binaryExpr);
    cout << "   表达式语句创建成功" << endl;
    
    // 测试变量声明
    cout << "\n5. 测试变量声明:" << endl;
    VariableDeclaration* varDecl = new VariableDeclaration("y", "int", intExpr);
    cout << "   变量声明创建成功" << endl;
    
    // 测试程序结构
    cout << "\n6. 测试程序结构:" << endl;
    vector<Statement*> statements;
    statements.push_back(exprStmt);
    statements.push_back(varDecl);
    
    Program* program = new Program(statements);
    cout << "   程序创建成功，包含 " << statements.size() << " 个语句" << endl;
    
    // 清理内存
    delete intExpr;
    delete doubleExpr;
    delete boolExpr;
    delete strExpr;
    delete varExpr;
    delete binaryExpr;
    delete exprStmt;
    delete varDecl;
    delete program;
    
    cout << "\n=== AST结构测试完成 ===" << endl;
    return 0;
}
