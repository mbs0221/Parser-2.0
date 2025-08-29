#include "lexer/value.h"
#include "parser/expression.h"
#include "interpreter/interpreter.h"
#include <iostream>

using namespace std;

int test_constant_expression() {
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
    if (Integer* intVal = dynamic_cast<Integer*>(intValue)) {
        Integer result = *intVal + Integer(8);
        cout << "Integer运算: 42 + 8 = " << result.toString() << endl;
        
        Bool comparison = *intVal > Integer(40);
        cout << "Integer比较: 42 > 40 = " << comparison.toString() << endl;
    }
    
    if (Double* doubleVal = dynamic_cast<Double*>(doubleValue)) {
        Double result = *doubleVal * Double(2.0);
        cout << "DoubleValue运算: 3.14 * 2.0 = " << result.toString() << endl;
    }
    
    if (Bool* boolVal = dynamic_cast<Bool*>(boolValue)) {
        Bool result = *boolVal && Bool(false);
        cout << "BoolValue运算: true && false = " << result.toString() << endl;
    }
    
    cout << "ConstantExpression和Value类型测试完成！" << endl;
    return 0;
}
