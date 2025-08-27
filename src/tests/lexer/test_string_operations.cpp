#include "Parser/inter.h"
#include "Parser/interpreter.h"
#include <iostream>

using namespace std;

int main() {
    cout << "=== 字符串运算测试 ===" << endl;
    
    // 创建解释器
    Interpreter interpreter;
    
    // 测试1：字符串拼接
    cout << "\n1. 字符串拼接测试:" << endl;
    StringLiteral* str1 = new StringLiteral("Hello, ");
    StringLiteral* str2 = new StringLiteral("World!");
    
    Expression* result = interpreter.stringConcatenation(str1, str2);
    if (result) {
        cout << "拼接结果: " << result->toString() << endl;
    }
    
    // 测试2：字符串比较
    cout << "\n2. 字符串比较测试:" << endl;
    StringLiteral* str3 = new StringLiteral("apple");
    StringLiteral* str4 = new StringLiteral("banana");
    
    Expression* compareResult = interpreter.stringComparison(str3, str4, "<");
    if (compareResult) {
        cout << "apple < banana: " << compareResult->toString() << endl;
    }
    
    // 测试3：字符串长度
    cout << "\n3. 字符串长度测试:" << endl;
    StringNode* strNode = new StringNode("Hello World");
    Expression* lengthResult = interpreter.stringLength(strNode);
    if (lengthResult) {
        cout << "字符串长度: " << lengthResult->toString() << endl;
    }
    
    // 测试4：字符串索引访问
    cout << "\n4. 字符串索引访问测试:" << endl;
    StringLiteral* indexExpr = new StringLiteral("0");
    Expression* charResult = interpreter.stringIndexing(strNode, indexExpr);
    if (charResult) {
        cout << "第一个字符: " << charResult->toString() << endl;
    }
    
    // 测试5：内置函数测试
    cout << "\n5. 内置函数测试:" << endl;
    vector<Expression*> args;
    args.push_back(new StringLiteral("测试字符串"));
    
    Expression* lengthFuncResult = interpreter.executeStringLength(args);
    if (lengthFuncResult) {
        cout << "length()函数结果: " << lengthFuncResult->toString() << endl;
    }
    
    // 测试6：变量定义和查找
    cout << "\n6. 变量定义和查找测试:" << endl;
    IdentifierNode* varName = new IdentifierNode(new Word(ID, "message"));
    StringLiteral* varValue = new StringLiteral("Hello from variable!");
    
    interpreter.defineVariable("message", varValue);
    Expression* foundVar = interpreter.lookupVariable("message");
    if (foundVar) {
        cout << "找到变量: " << foundVar->toString() << endl;
    }
    
    cout << "\n=== 测试完成 ===" << endl;
    
    return 0;
}
