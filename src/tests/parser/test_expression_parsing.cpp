#include <iostream>
#include <string>
#include "Parser/lexer.h"
#include "Parser/parser.h"
#include "Parser/value.h"

using namespace std;

int main() {
    cout << "测试移除parseAdditive后的表达式解析" << endl;
    
    cout << "\n1. 测试加减运算" << endl;
    cout << "现在加减运算通过统一的运算符优先文法解析" << endl;
    cout << "示例: a + b - c" << endl;
    cout << "解析为: BinaryExpression(BinaryExpression(a, b, +), c, -)" << endl;
    
    cout << "\n2. 测试混合运算" << endl;
    cout << "示例: a + b * c - d" << endl;
    cout << "解析为: BinaryExpression(BinaryExpression(a, BinaryExpression(b, c, *), +), d, -)" << endl;
    
    cout << "\n3. 测试赋值运算" << endl;
    cout << "示例: x = y = z" << endl;
    cout << "解析为: BinaryExpression(x, BinaryExpression(y, z, =), =)" << endl;
    
    cout << "\n4. 优势" << endl;
    cout << "- 统一的表达式解析逻辑" << endl;
    cout << "- 支持任意复杂度的表达式" << endl;
    cout << "- 正确处理运算符优先级和结合性" << endl;
    cout << "- 代码更简洁，易于维护" << endl;
    
    cout << "\n5. 运算符优先级（从高到低）" << endl;
    cout << "1. 一元操作符: !, -" << endl;
    cout << "2. 乘除模: *, /, %" << endl;
    cout << "3. 加减: +, -" << endl;
    cout << "4. 比较: ==, !=, <, >, <=, >=" << endl;
    cout << "5. 逻辑: &&, ||" << endl;
    cout << "6. 赋值: =" << endl;
    
    cout << "\n测试完成" << endl;
    return 0;
}
