#include <iostream>
#include <string>
#include "Parser/expression.h"
#include "Parser/lexer.h"
#include "Parser/value.h"

using namespace std;

int main() {
    cout << "测试赋值表达式作为二元运算符的设计" << endl;
    
    // 创建标识符表达式
    IdentifierExpression* varA = new IdentifierExpression("a");
    IdentifierExpression* varB = new IdentifierExpression("b");
    
    // 创建常量表达式
    Integer* intVal = new Integer(42);
    ConstantExpression* constExpr = new ConstantExpression(intVal);
    
    // 创建赋值运算符
    Operator* assignOp = new Operator('=', "=", 2, false);  // 赋值运算符，优先级2，右结合
    
    // 创建赋值表达式：a = 42
    BinaryExpression* assignment = new BinaryExpression(varA, constExpr, assignOp);
    
    cout << "创建赋值表达式: a = 42" << endl;
    cout << "左操作数类型: " << typeid(*assignment->left).name() << endl;
    cout << "右操作数类型: " << typeid(*assignment->right).name() << endl;
    cout << "操作符: " << assignment->operator_->getSymbol() << endl;
    cout << "操作符优先级: " << assignment->operator_->getPrecedence() << endl;
    cout << "操作符结合性: " << (assignment->operator_->isLeftAssoc() ? "左结合" : "右结合") << endl;
    
    // 测试连续赋值：a = b = 42
    BinaryExpression* nestedAssignment = new BinaryExpression(varA, assignment, assignOp);
    cout << "\n创建连续赋值表达式: a = b = 42" << endl;
    cout << "嵌套赋值表达式类型: " << typeid(*nestedAssignment).name() << endl;
    
    // 清理内存
    delete varA;
    delete varB;
    delete intVal;
    delete constExpr;
    delete assignOp;
    delete assignment;
    delete nestedAssignment;
    
    cout << "\n测试完成" << endl;
    return 0;
}
