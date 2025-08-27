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
    string input;
    cin >> input;
    return new String(input);
}