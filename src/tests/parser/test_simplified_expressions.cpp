#include "Parser/value.h"
#include "Parser/expression.h"
#include "Parser/interpreter.h"
#include <iostream>

using namespace std;

int main() {
    cout << "测试简化后的表达式设计..." << endl;
    
    // 创建解释器
    Interpreter interpreter;
    
    // 测试ConstantExpression（常量）
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
    
    // 测试IdentifierExpression（变量）
    // 注意：这里需要先在作用域中定义变量
    interpreter.scopeManager.defineVariable("x", new IntegerValue(100));
    
    IdentifierExpression* idExpr = new IdentifierExpression("x");
    Value* varValue = interpreter.visit(idExpr);
    cout << "IdentifierExpression(\"x\") = " << varValue->toString() << endl;
    
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
    
    if (BoolValue* boolVal = dynamic_cast<BoolValue*>(boolValue)) {
        BoolValue result = *boolVal && BoolValue(false);
        cout << "Value运算: true && false = " << result.toString() << endl;
    }
    
    cout << "简化后的表达式设计测试完成！" << endl;
    cout << "现在只有两种叶子节点：" << endl;
    cout << "1. ConstantExpression - 存储常量值" << endl;
    cout << "2. IdentifierExpression - 查询变量值" << endl;
    
    return 0;
}
