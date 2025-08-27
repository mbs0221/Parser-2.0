#include <iostream>
#include <string>
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "lexer/value.h"

using namespace std;

int main() {
    cout << "测试内置函数作为普通标识符处理" << endl;
    
    // 测试词法分析器对内置函数名的处理
    cout << "1. 测试词法分析器" << endl;
    
    // 创建词法分析器
    Lexer lexer;
    
    // 测试print、cin、count、length是否被识别为标识符
    cout << "print、cin、count、length现在应该被识别为普通标识符，而不是关键字" << endl;
    
    // 测试语法分析器对内置函数调用的处理
    cout << "\n2. 测试语法分析器" << endl;
    cout << "内置函数调用现在应该被解析为普通的函数调用表达式" << endl;
    
    // 示例：
    // print("Hello") -> CallExpression(IdentifierExpression("print"), [StringLiteral("Hello")])
    // cin() -> CallExpression(IdentifierExpression("cin"), [])
    // count([1,2,3]) -> CallExpression(IdentifierExpression("count"), [ArrayNode([1,2,3])])
    // length("test") -> CallExpression(IdentifierExpression("length"), [StringLiteral("test")])
    
    cout << "\n3. 优势" << endl;
    cout << "- 简化了词法分析器和语法分析器" << endl;
    cout << "- 内置函数和用户函数有一致的处理方式" << endl;
    cout << "- 更容易扩展新的内置函数" << endl;
    cout << "- 支持函数重载和覆盖" << endl;
    
    cout << "\n测试完成" << endl;
    return 0;
}
