#include "Parser/value.h"
#include "Parser/expression.h"
#include "Parser/interpreter.h"
#include <iostream>

using namespace std;

int main() {
    cout << "测试ConstantExpression和Value类型..." << endl;
    
    // 创建解释器
    Interpreter interpreter;
    
    // 测试不同类型的常量表达式
    ConstantExpression* intConst = new ConstantExpression(42);
    Value* intValue = interpreter.visit(intConst);
    cout << "ConstantExpression(42) = " << intValue->toString() << endl;
    
    ConstantExpression* doubleConst = new ConstantExpression(3.14);
    Value* doubleValue = interpreter.visit(doubleConst);
    cout << "ConstantExpression(3.14) = " << doubleValue->toString() << endl;
    
    ConstantExpression* boolConst = new ConstantExpression(true);
    Value* boolValue = interpreter.visit(boolConst);
    cout << "ConstantExpression(true) = " << boolValue->toString() << endl;
    
    ConstantExpression* charConst = new ConstantExpression('A');
    Value* charValue = interpreter.visit(charConst);
    cout << "ConstantExpression('A') = " << charValue->toString() << endl;
    
    ConstantExpression* stringConst = new ConstantExpression("Hello");
    Value* stringValue = interpreter.visit(stringConst);
    cout << "ConstantExpression(\"Hello\") = " << stringValue->toString() << endl;
    
    // 测试Value类型的运算符重载
    if (IntegerValue* intVal = dynamic_cast<IntegerValue*>(intValue)) {
        IntegerValue result = *intVal + IntegerValue(8);
        cout << "IntegerValue运算: 42 + 8 = " << result.toString() << endl;
        
        BoolValue comparison = *intVal > IntegerValue(40);
        cout << "IntegerValue比较: 42 > 40 = " << comparison.toString() << endl;
    }
    
    if (DoubleValue* doubleVal = dynamic_cast<DoubleValue*>(doubleValue)) {
        DoubleValue result = *doubleVal * DoubleValue(2.0);
        cout << "DoubleValue运算: 3.14 * 2.0 = " << result.toString() << endl;
    }
    
    if (BoolValue* boolVal = dynamic_cast<BoolValue*>(boolValue)) {
        BoolValue result = *boolVal && BoolValue(false);
        cout << "BoolValue运算: true && false = " << result.toString() << endl;
    }
    
    cout << "ConstantExpression和Value类型测试完成！" << endl;
    return 0;
}
