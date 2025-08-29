#include "include/lexer/value.h"
#include "src/lexer/value.cpp"
#include <iostream>

using namespace std;

int test_str_method() {
    cout << "=== 测试 Token 及其子类型的 str 方法 ===" << endl;
    
    // 测试基本 Token
    Token token(123);
    cout << "Token: " << token.str() << endl;
    
    // 测试 Word
    Word word(ID, "variable");
    cout << "Word: " << word.str() << endl;
    
    // 测试 Type
    Type type(ID, "int", 4);
    cout << "Type: " << type.str() << endl;
    
    // 测试 Operator
    Operator op(PLUS, "+", 1, true);
    cout << "Operator: " << op.str() << endl;
    
    // 测试 Visibility
    Visibility vis(PUBLIC, VIS_PUBLIC);
    cout << "Visibility: " << vis.str() << endl;
    
    // 测试 Bool
    Bool boolVal(true);
    cout << "Bool: " << boolVal.str() << endl;
    
    // 测试 Integer
    Integer intVal(42);
    cout << "Integer: " << intVal.str() << endl;
    
    // 测试 Char
    Char charVal('A');
    cout << "Char: " << charVal.str() << endl;
    
    // 测试 Double
    Double doubleVal(3.14);
    cout << "Double: " << doubleVal.str() << endl;
    
    // 测试 String
    String stringVal("Hello World");
    cout << "String: " << stringVal.str() << endl;
    
    // 测试 Array
    Array array;
    array.addElement(new Integer(1));
    array.addElement(new Integer(2));
    array.addElement(new Integer(3));
    cout << "Array: " << array.str() << endl;
    
    // 测试 Dict
    Dict dict;
    dict.setEntry("name", new String("John"));
    dict.setEntry("age", new Integer(25));
    cout << "Dict: " << dict.str() << endl;
    
    cout << "\n=== 测试完成 ===" << endl;
    return 0;
} 
