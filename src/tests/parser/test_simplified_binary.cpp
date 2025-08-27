#include "Parser/value.h"
#include "Parser/expression.h"
#include "Parser/interpreter.h"
#include <iostream>

using namespace std;

int main() {
    cout << "测试简化后的BinaryExpression处理..." << endl;
    
    // 创建解释器
    Interpreter interpreter;
    
    // 测试1：算术运算的自动类型转换
    cout << "\n=== 测试算术运算 ===" << endl;
    
    ConstantExpression* intExpr = new ConstantExpression(10);
    ConstantExpression* doubleExpr = new ConstantExpression(3.5);
    Token* plusToken = new Token('+');
    BinaryExpression* arithmeticOp = new BinaryExpression(intExpr, doubleExpr, plusToken);
    
    Value* arithmeticResult = interpreter.visit(arithmeticOp);
    cout << "int(10) + double(3.5) = " << arithmeticResult->toString() << endl;
    
    // 测试2：比较运算的自动类型转换
    cout << "\n=== 测试比较运算 ===" << endl;
    
    ConstantExpression* boolExpr = new ConstantExpression(true);
    ConstantExpression* zeroExpr = new ConstantExpression(0);
    Token* lessToken = new Token('<');
    BinaryExpression* compareOp = new BinaryExpression(boolExpr, zeroExpr, lessToken);
    
    Value* compareResult = interpreter.visit(compareOp);
    cout << "bool(true) < int(0) = " << compareResult->toString() << endl;
    
    // 测试3：逻辑运算的自动类型转换
    cout << "\n=== 测试逻辑运算 ===" << endl;
    
    ConstantExpression* nonZeroExpr = new ConstantExpression(42);
    Token* andToken = new Token(AND);
    BinaryExpression* logicOp = new BinaryExpression(nonZeroExpr, boolExpr, andToken);
    
    Value* logicResult = interpreter.visit(logicOp);
    cout << "int(42) && bool(true) = " << logicResult->toString() << endl;
    
    // 测试4：字符串比较
    cout << "\n=== 测试字符串比较 ===" << endl;
    
    ConstantExpression* str1 = new ConstantExpression("hello");
    ConstantExpression* str2 = new ConstantExpression("world");
    Token* eqToken = new Token(EQ);
    BinaryExpression* strCompareOp = new BinaryExpression(str1, str2, eqToken);
    
    Value* strCompareResult = interpreter.visit(strCompareOp);
    cout << "string(\"hello\") == string(\"world\") = " << strCompareResult->toString() << endl;
    
    // 测试5：字符运算
    cout << "\n=== 测试字符运算 ===" << endl;
    
    ConstantExpression* charExpr = new ConstantExpression('A');
    BinaryExpression* charOp = new BinaryExpression(charExpr, intExpr, plusToken);
    
    Value* charResult = interpreter.visit(charOp);
    cout << "char('A') + int(10) = " << charResult->toString() << endl;
    
    cout << "\n简化后的BinaryExpression处理测试完成！" << endl;
    cout << "优势：" << endl;
    cout << "1. 清晰的步骤：计算值 -> 确定类型 -> 转换 -> 计算" << endl;
    cout << "2. 统一的类型转换：使用CastExpression" << endl;
    cout << "3. 简化的逻辑：减少了复杂的条件判断" << endl;
    cout << "4. 更好的可读性：每个步骤都有明确的注释" << endl;
    cout << "5. 自动内存管理：清理临时对象" << endl;
    
    return 0;
}
