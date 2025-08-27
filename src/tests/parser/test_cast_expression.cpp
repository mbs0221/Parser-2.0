#include "Parser/value.h"
#include "Parser/expression.h"
#include "Parser/interpreter.h"
#include <iostream>

using namespace std;

int main() {
    cout << "测试CastExpression类型转换系统..." << endl;
    
    // 创建解释器
    Interpreter interpreter;
    
    // 测试1：显式类型转换
    cout << "\n=== 测试显式类型转换 ===" << endl;
    
    // 整数转浮点数
    ConstantExpression* intExpr = new ConstantExpression(42);
    CastExpression* intToDouble = new CastExpression(intExpr, "double");
    Value* doubleResult = interpreter.visit(intToDouble);
    cout << "int(42) -> double = " << doubleResult->toString() << endl;
    
    // 浮点数转整数
    ConstantExpression* doubleExpr = new ConstantExpression(3.7);
    CastExpression* doubleToInt = new CastExpression(doubleExpr, "int");
    Value* intResult = interpreter.visit(doubleToInt);
    cout << "double(3.7) -> int = " << intResult->toString() << endl;
    
    // 整数转布尔
    ConstantExpression* zeroExpr = new ConstantExpression(0);
    CastExpression* intToBool = new CastExpression(zeroExpr, "bool");
    Value* boolResult = interpreter.visit(intToBool);
    cout << "int(0) -> bool = " << boolResult->toString() << endl;
    
    // 字符串转整数
    ConstantExpression* strExpr = new ConstantExpression("123");
    CastExpression* strToInt = new CastExpression(strExpr, "int");
    Value* strIntResult = interpreter.visit(strToInt);
    cout << "string(\"123\") -> int = " << strIntResult->toString() << endl;
    
    // 测试2：自动类型转换（通过BinaryExpression）
    cout << "\n=== 测试自动类型转换 ===" << endl;
    
    // 整数 + 浮点数
    ConstantExpression* leftInt = new ConstantExpression(10);
    ConstantExpression* rightDouble = new ConstantExpression(3.5);
    Token* plusToken = new Token('+');
    BinaryExpression* mixedOp = new BinaryExpression(leftInt, rightDouble, plusToken);
    Value* mixedResult = interpreter.visit(mixedOp);
    cout << "int(10) + double(3.5) = " << mixedResult->toString() << endl;
    
    // 布尔 + 整数
    ConstantExpression* boolExpr = new ConstantExpression(true);
    ConstantExpression* rightInt = new ConstantExpression(5);
    BinaryExpression* boolIntOp = new BinaryExpression(boolExpr, rightInt, plusToken);
    Value* boolIntResult = interpreter.visit(boolIntOp);
    cout << "bool(true) + int(5) = " << boolIntResult->toString() << endl;
    
    // 字符 + 整数
    ConstantExpression* charExpr = new ConstantExpression('A');
    BinaryExpression* charIntOp = new BinaryExpression(charExpr, rightInt, plusToken);
    Value* charIntResult = interpreter.visit(charIntOp);
    cout << "char('A') + int(5) = " << charIntResult->toString() << endl;
    
    // 测试3：比较运算的自动类型转换
    cout << "\n=== 测试比较运算的自动类型转换 ===" << endl;
    
    // 整数 < 浮点数
    Token* lessToken = new Token('<');
    BinaryExpression* compareOp = new BinaryExpression(leftInt, rightDouble, lessToken);
    Value* compareResult = interpreter.visit(compareOp);
    cout << "int(10) < double(3.5) = " << compareResult->toString() << endl;
    
    // 布尔比较
    ConstantExpression* falseExpr = new ConstantExpression(false);
    BinaryExpression* boolCompareOp = new BinaryExpression(boolExpr, falseExpr, lessToken);
    Value* boolCompareResult = interpreter.visit(boolCompareOp);
    cout << "bool(true) < bool(false) = " << boolCompareResult->toString() << endl;
    
    // 测试4：逻辑运算的自动类型转换
    cout << "\n=== 测试逻辑运算的自动类型转换 ===" << endl;
    
    // 整数 && 布尔
    Token* andToken = new Token(AND);
    BinaryExpression* logicOp = new BinaryExpression(leftInt, boolExpr, andToken);
    Value* logicResult = interpreter.visit(logicOp);
    cout << "int(10) && bool(true) = " << logicResult->toString() << endl;
    
    // 字符串 && 整数
    BinaryExpression* strLogicOp = new BinaryExpression(strExpr, leftInt, andToken);
    Value* strLogicResult = interpreter.visit(strLogicOp);
    cout << "string(\"123\") && int(10) = " << strLogicResult->toString() << endl;
    
    cout << "\nCastExpression类型转换系统测试完成！" << endl;
    cout << "优势：" << endl;
    cout << "1. 统一的类型转换接口" << endl;
    cout << "2. 消除了复杂的dynamic_cast操作" << endl;
    cout << "3. 支持显式和自动类型转换" << endl;
    cout << "4. 类型安全的转换操作" << endl;
    
    return 0;
}
