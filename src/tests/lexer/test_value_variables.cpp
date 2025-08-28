#include "lexer/value.h"
#include "parser/expression.h"
#include "interpreter/interpreter.h"
#include <iostream>

using namespace std;

int main() {
    cout << "测试变量存储Value类型..." << endl;
    
    // 创建解释器
    Interpreter interpreter;
    
    // 测试变量声明和赋值
    IntExpression* intExpr = new IntExpression(42);
    Value* intValue = interpreter.visit(intExpr);
    cout << "IntExpression(42) = " << intValue->toString() << endl;
    
    // 声明变量
    VariableDeclaration* decl = new VariableDeclaration();
    Variable var;
    var.name = "x";
    var.initializer = intExpr;
    decl->variables.push_back(var);
    
    interpreter.visit(decl);
    cout << "声明变量 x = 42" << endl;
    
    // 查找变量
    IdentifierExpression* idExpr = new IdentifierExpression("x");
    Value* retrievedValue = interpreter.visit(idExpr);
    cout << "查找变量 x = " << retrievedValue->toString() << endl;
    
    // 测试赋值
    IntExpression* newValue = new IntExpression(100);
    Value* newValueResult = interpreter.visit(newValue);
    
    AssignmentExpression* assign = new AssignmentExpression();
    assign->variableName = "x";
    assign->value = newValue;
    
    Value* assignResult = interpreter.visit(assign);
    cout << "赋值 x = 100, 结果 = " << assignResult->toString() << endl;
    
    // 再次查找变量
    Value* finalValue = interpreter.visit(idExpr);
    cout << "最终变量 x = " << finalValue->toString() << endl;
    
    cout << "变量存储Value类型测试完成！" << endl;
    return 0;
}
