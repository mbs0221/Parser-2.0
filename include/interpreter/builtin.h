#ifndef BUILTIN_H
#define BUILTIN_H

#include "lexer/value.h"
#include "parser/function.h"
#include "interpreter/scope.h"

#include <vector>

using namespace std;

Value* builtin_print(vector<Variable*>& args);
Value* builtin_count(vector<Variable*>& args);
Value* builtin_cin(vector<Variable*>& args);

// 数学函数
Value* builtin_abs(vector<Variable*>& args);
Value* builtin_max(vector<Variable*>& args);
Value* builtin_min(vector<Variable*>& args);
Value* builtin_pow(vector<Variable*>& args);

// 字符串函数
Value* builtin_length(vector<Variable*>& args);
Value* builtin_substring(vector<Variable*>& args);
Value* builtin_upper(vector<Variable*>& args);
Value* builtin_lower(vector<Variable*>& args);

// 数组函数
Value* builtin_push(vector<Variable*>& args);
Value* builtin_pop(vector<Variable*>& args);
Value* builtin_sort(vector<Variable*>& args);

// 类型转换函数
Value* builtin_to_string(vector<Variable*>& args);
Value* builtin_to_int(vector<Variable*>& args);
Value* builtin_to_double(vector<Variable*>& args);
Value* builtin_cast(vector<Variable*>& args);

// 系统函数
Value* builtin_random(vector<Variable*>& args);
Value* builtin_exit(vector<Variable*>& args);

// 内置函数注册
void registerBuiltinFunctionsToScope(ScopeManager& scopeManager);

#endif