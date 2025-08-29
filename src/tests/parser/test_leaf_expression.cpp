#include "lexer/value.h"
#include "parser/expression.h"
#include <iostream>

using namespace std;

int test_leaf_expression() {
    cout << "测试LeafExpression直接封装Value..." << endl;
    
    // 测试ConstantExpression
    ConstantExpression* intExpr = new ConstantExpression(42);
    cout << "ConstantExpression(42).toString() = " << intExpr->toString() << endl;
    cout << "ConstantExpression(42).getIntValue() = " << intExpr->getValue()->toString() << endl;
    
    // 测试Value访问
    Value* intValue = intExpr->getValue();
    cout << "ConstantExpression.getValue()->toString() = " << intValue->toString() << endl;
    
    // 测试ConstantExpression
    ConstantExpression* doubleExpr = new ConstantExpression(3.14);
    cout << "ConstantExpression(3.14).toString() = " << doubleExpr->toString() << endl;
    cout << "ConstantExpression(3.14).getDoubleValue() = " << doubleExpr->getValue()->toString() << endl;
    
    Value* doubleValue = doubleExpr->getValue();
    cout << "ConstantExpression.getValue()->toString() = " << doubleValue->toString() << endl;
    
    // 测试运算符重载
    ConstantExpression* left = new ConstantExpression(10);
    ConstantExpression* right = new ConstantExpression(5);
    ConstantExpression result = *left + *right;
    cout << "ConstantExpression(10) + ConstantExpression(5) = " << result.toString() << endl;
    
    cout << "LeafExpression封装Value测试完成！" << endl;
    return 0;
}
