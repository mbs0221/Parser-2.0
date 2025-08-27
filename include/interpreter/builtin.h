#ifndef BUILTIN_H
#define BUILTIN_H

#include "lexer/value.h"
#include "parser/function.h"

#include <vector>

using namespace std;

Value* builtin_print(vector<Variable*>& args);
Value* builtin_count(vector<Variable*>& args);
Value* builtin_cin(vector<Variable*>& args);

#endif