#include "lexer/value.h"
#include "parser/expression.h"
#include "interpreter/interpreter.h"
#include <iostream>

using namespace std;

int main() {
    cout << "测试合并后的determineTargetType函数..." << endl;
    
    // 创建解释器
    Interpreter interpreter;
    
    // 测试算术运算的目标类型确定
    cout << "\n=== 测试算术运算目标类型 ===" << endl;
    
    Integer* intVal = new Integer(42);
    Double* doubleVal = new Double(3.14);
    Bool* boolVal = new Bool(true);
    Char* charVal = new Char('A');
    String* strVal = new String("hello");
    
    // 测试不同类型的组合
    cout << "int + double -> " << interpreter.determineTargetType(intVal, doubleVal, false) << endl;
    cout << "int + bool -> " << interpreter.determineTargetType(intVal, boolVal, false) << endl;
    cout << "char + int -> " << interpreter.determineTargetType(charVal, intVal, false) << endl;
    cout << "bool + char -> " << interpreter.determineTargetType(boolVal, charVal, false) << endl;
    cout << "double + double -> " << interpreter.determineTargetType(doubleVal, doubleVal, false) << endl;
    
    // 测试比较运算的目标类型确定
    cout << "\n=== 测试比较运算目标类型 ===" << endl;
    
    cout << "int < double -> " << interpreter.determineTargetType(intVal, doubleVal, true) << endl;
    cout << "string == string -> " << interpreter.determineTargetType(strVal, strVal, true) << endl;
    cout << "bool < bool -> " << interpreter.determineTargetType(boolVal, boolVal, true) << endl;
    cout << "int == bool -> " << interpreter.determineTargetType(intVal, boolVal, true) << endl;
    cout << "char > int -> " << interpreter.determineTargetType(charVal, intVal, true) << endl;
    
    // 测试类型转换
    cout << "\n=== 测试类型转换 ===" << endl;
    
    Value* convertedInt = interpreter.convertValue(intVal, "double");
    cout << "int(42) -> double = " << convertedInt->toString() << endl;
    
    Value* convertedBool = interpreter.convertValue(boolVal, "int");
    cout << "bool(true) -> int = " << convertedBool->toString() << endl;
    
    Value* convertedChar = interpreter.convertValue(charVal, "int");
    cout << "char('A') -> int = " << convertedChar->toString() << endl;
    
    cout << "\n合并后的determineTargetType函数测试完成！" << endl;
    cout << "优势：" << endl;
    cout << "1. 减少了代码重复" << endl;
    cout << "2. 统一的类型确定逻辑" << endl;
    cout << "3. 通过isComparison参数区分运算类型" << endl;
    cout << "4. 更简洁的API" << endl;
    
    return 0;
}
