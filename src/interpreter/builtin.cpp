#include "builtin.h"
#include "value.h"
#include "variable.h"
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
            return new Integer(value->getValue()->size());
        }
    }
    return nullptr;
};

Value* builtin_cin(vector<Variable*>& args) {
    string input;
    cin >> input;
    return new String(input);
}