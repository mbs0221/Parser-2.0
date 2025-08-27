#include "Parser/value.h"
#include "Parser/expression.h"
#include <iostream>

using namespace std;

int main() {
    cout << "测试LeafExpression直接封装Value..." << endl;
    
    // 测试IntExpression
    IntExpression* intExpr = new IntExpression(42);
    cout << "IntExpression(42).toString() = " << intExpr->toString() << endl;
    cout << "IntExpression(42).getIntValue() = " << intExpr->getIntValue() << endl;
    
    // 测试Value访问
    Value* intValue = intExpr->getValue();
    cout << "IntExpression.getValue()->toString() = " << intValue->toString() << endl;
    
    // 测试DoubleExpression
    DoubleExpression* doubleExpr = new DoubleExpression(3.14);
    cout << "DoubleExpression(3.14).toString() = " << doubleExpr->toString() << endl;
    cout << "DoubleExpression(3.14).getDoubleValue() = " << doubleExpr->getDoubleValue() << endl;
    
    Value* doubleValue = doubleExpr->getValue();
    cout << "DoubleExpression.getValue()->toString() = " << doubleValue->toString() << endl;
    
    // 测试运算符重载
    IntExpression* left = new IntExpression(10);
    IntExpression* right = new IntExpression(5);
    IntExpression result = *left + *right;
    cout << "IntExpression(10) + IntExpression(5) = " << result.toString() << endl;
    
    cout << "LeafExpression封装Value测试完成！" << endl;
    return 0;
}
