#include "Parser/value.h"
#include "Parser/expression.h"
#include "interpreter/interpreter.h"
#include <iostream>

using namespace std;

int main() {
    cout << "测试Value类型的表达式访问方法..." << endl;
    
    // 创建解释器
    Interpreter interpreter;
    
    // 测试基本表达式
    IntExpression* intExpr = new IntExpression(42);
    Value* intResult = interpreter.visit(intExpr);
    cout << "IntExpression(42) = " << intResult->toString() << endl;
    
    DoubleExpression* doubleExpr = new DoubleExpression(3.14);
    Value* doubleResult = interpreter.visit(doubleExpr);
    cout << "DoubleExpression(3.14) = " << doubleResult->toString() << endl;
    
    BoolExpression* boolExpr = new BoolExpression(true);
    Value* boolResult = interpreter.visit(boolExpr);
    cout << "BoolExpression(true) = " << boolResult->toString() << endl;
    
    // 测试二元运算
    IntExpression* left = new IntExpression(10);
    IntExpression* right = new IntExpression(5);
    Token* plusToken = new Token('+');
    BinaryExpression* binaryExpr = new BinaryExpression(left, plusToken, right);
    
    Value* binaryResult = interpreter.visit(binaryExpr);
    cout << "BinaryExpression(10 + 5) = " << binaryResult->toString() << endl;
    
    cout << "Value类型表达式访问方法测试完成！" << endl;
    return 0;
}
