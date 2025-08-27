#include "Parser/ast_optimizer.h"
#include "Parser/inter.h"
#include <iostream>

using namespace std;

int main() {
    cout << "测试类型转换优化器..." << endl;
    
    // 创建一个简单的测试表达式：int + double
    IntExpression* left = new IntExpression(5);
    DoubleExpression* right = new DoubleExpression(3.14);
    
    // 创建算术表达式
    ArithmeticExpression* expr = new ArithmeticExpression(left, right, new Word("+", Tag::PLUS));
    
    cout << "原始表达式: " << expr->getLocation() << endl;
    
    // 创建优化器
    TypeCastOptimizer optimizer;
    
    // 优化表达式
    Expression* optimized = optimizer.optimize(expr);
    
    cout << "优化后表达式: " << optimized->getLocation() << endl;
    
    // 清理内存
    delete expr;
    delete optimized;
    
    cout << "测试完成！" << endl;
    return 0;
}
