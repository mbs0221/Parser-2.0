#include "interpreter/builtin.h"
#include "lexer/value.h"
#include "parser/function.h"
#include <iostream>
#include <sstream>
#include <typeinfo>

using namespace std;

Value* builtin_print(vector<Variable*>& args) {
    for (auto arg : args) {
        if (arg) {
            Value* value = arg->getValue();
            if (value) {
                cout << value->toString();
            }
        }
    }
    return nullptr;
};

Value* builtin_count(vector<Variable*>& args) {
    if (args.size() == 1) {
        Variable* value = args[0];
        if (value) {
            Value* val = value->getValue();
            if (Array* array = dynamic_cast<Array*>(val)) {
                return new Integer(array->size());
            } else if (String* str = dynamic_cast<String*>(val)) {
                return new Integer(str->length());
            }
        }
    }
    return nullptr;
};

Value* builtin_cin(vector<Variable*>& args) {
    String* lastInputValue = nullptr;
    
    // 处理所有参数，为每个参数读取一个值
    for (Variable* arg : args) {
        if (arg) {
            string input;
            cin >> input;
            String* inputValue = new String(input);
            arg->setValue(inputValue);
            lastInputValue = inputValue;
        }
    }
    
    // 如果没有参数，读取一个值并返回
    if (args.empty()) {
        string input;
        cin >> input;
        lastInputValue = new String(input);
    }
    
    // 返回最后一个输入值（用于 let input = cin() 的情况）
    return lastInputValue;
}