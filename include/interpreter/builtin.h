#ifndef BUILTIN_H
#define BUILTIN_H

#include "value.h"
#include "function.h"

#include <vector>

using namespace std;

Value* builtin_print(vector<Variable*>& args);
Value* builtin_count(vector<Variable*>& args);
Value* builtin_cin(vector<Variable*>& args);

#endif