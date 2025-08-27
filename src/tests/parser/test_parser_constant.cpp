#include "Parser/parser.h"
#include "Parser/interpreter.h"
#include <iostream>

using namespace std;

int main() {
    cout << "测试parser解析常量表达式..." << endl;
    
    // 创建parser和interpreter
    Parser parser;
    Interpreter interpreter;
    
    // 测试解析不同类型的常量
    // 注意：这里我们直接创建AST节点来测试，实际使用时parser会从文件解析
    
    // 测试整数常量
    ConstantExpression* intConst = new ConstantExpression(42);
    Value* intValue = interpreter.visit(intConst);
    cout << "解析整数常量 42 = " << intValue->toString() << endl;
    
    // 测试浮点数常量
    ConstantExpression* doubleConst = new ConstantExpression(3.14);
    Value* doubleValue = interpreter.visit(doubleConst);
    cout << "解析浮点数常量 3.14 = " << doubleValue->toString() << endl;
    
    // 测试布尔常量
    ConstantExpression* boolConst = new ConstantExpression(true);
    Value* boolValue = interpreter.visit(boolConst);
    cout << "解析布尔常量 true = " << boolValue->toString() << endl;
    
    // 测试字符常量
    ConstantExpression* charConst = new ConstantExpression('A');
    Value* charValue = interpreter.visit(charConst);
    cout << "解析字符常量 'A' = " << charValue->toString() << endl;
    
    // 测试字符串常量
    ConstantExpression* stringConst = new ConstantExpression("Hello World");
    Value* stringValue = interpreter.visit(stringConst);
    cout << "解析字符串常量 \"Hello World\" = " << stringValue->toString() << endl;
    
    // 测试Value类型的运算符重载
    if (IntegerValue* intVal = dynamic_cast<IntegerValue*>(intValue)) {
        IntegerValue result = *intVal + IntegerValue(8);
        cout << "Value运算: 42 + 8 = " << result.toString() << endl;
        
        BoolValue comparison = *intVal > IntegerValue(40);
        cout << "Value比较: 42 > 40 = " << comparison.toString() << endl;
    }
    
    if (DoubleValue* doubleVal = dynamic_cast<DoubleValue*>(doubleValue)) {
        DoubleValue result = *doubleVal * DoubleValue(2.0);
        cout << "Value运算: 3.14 * 2.0 = " << result.toString() << endl;
    }
    
    cout << "parser常量表达式解析测试完成！" << endl;
    return 0;
}
