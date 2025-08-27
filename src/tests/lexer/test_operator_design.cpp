#include <iostream>
#include <string>
#include "Parser/lexer.h"
#include "Parser/value.h"

using namespace std;

int main() {
    cout << "测试新的Operator设计" << endl;
    
    // 测试Operator的创建
    Operator* addOp = new Operator('+', "+", 4, true);
    Operator* mulOp = new Operator('*', "*", 5, true);
    Operator* assignOp = new Operator('=', "=", 2, false);
    Operator* eqOp = new Operator(EQ, "==", 7, true);
    
    cout << "加法运算符: " << addOp->getSymbol() << ", 优先级: " << addOp->getPrecedence() 
         << ", 左结合: " << (addOp->isLeftAssoc() ? "是" : "否") << endl;
    
    cout << "乘法运算符: " << mulOp->getSymbol() << ", 优先级: " << mulOp->getPrecedence() 
         << ", 左结合: " << (mulOp->isLeftAssoc() ? "是" : "否") << endl;
    
    cout << "赋值运算符: " << assignOp->getSymbol() << ", 优先级: " << assignOp->getPrecedence() 
         << ", 左结合: " << (assignOp->isLeftAssoc() ? "是" : "否") << endl;
    
    cout << "相等运算符: " << eqOp->getSymbol() << ", 优先级: " << eqOp->getPrecedence() 
         << ", 左结合: " << (eqOp->isLeftAssoc() ? "是" : "否") << endl;
    
    // 测试Value类型
    Integer* intVal = new Integer(42);
    Double* doubleVal = new Double(3.14);
    Char* charVal = new Char('A');
    Bool* boolVal = new Bool(true);
    
    cout << "整数值: " << intVal->toString() << ", 布尔值: " << intVal->toBool() << endl;
    cout << "浮点数值: " << doubleVal->toString() << ", 布尔值: " << doubleVal->toBool() << endl;
    cout << "字符值: " << charVal->toString() << ", 布尔值: " << charVal->toBool() << endl;
    cout << "布尔值: " << boolVal->toString() << ", 布尔值: " << boolVal->toBool() << endl;
    
    // 清理内存
    delete addOp;
    delete mulOp;
    delete assignOp;
    delete eqOp;
    delete intVal;
    delete doubleVal;
    delete charVal;
    delete boolVal;
    
    cout << "测试完成" << endl;
    return 0;
}
