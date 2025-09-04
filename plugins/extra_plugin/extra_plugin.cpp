#include "interpreter/plugins/builtin_plugin.h"

#include <iostream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>

using namespace std;

// 如果builtin_sort函数未定义，提供一个简单的实现
Value* builtin_sort(Scope* scope) {
    Value* arrVal = scope->getArgument<Value>("array");
    if (!arrVal) return nullptr;
    
    if (Array* arr = dynamic_cast<Array*>(arrVal)) {
        // 简单的排序实现
        // 这里可以添加实际的排序逻辑
        return arr;
    }
    
    return nullptr;
}

// 基础内置函数插件类
class BasicBuiltinPlugin : public BuiltinPlugin {
public:
    PluginInfo getPluginInfo() const override {
        return PluginInfo{
            "basic_builtin",
            "1.0.0",
            "基础内置函数插件，包含print、count、cin等基础功能",
            {"print", "count", "cin", "abs", "max", "min", "pow", "length", 
             "substring", "upper", "lower", "push", "pop", "sort", "to_string", 
             "to_int", "to_double", "cast", "random", "exit"}
        };
    }
    
    void registerFunctions(ScopeManager& scopeManager) override {
        // 注册基础内置函数
        scopeManager.defineFunction("print", new BuiltinFunction(
            [](Scope* scope) -> Value* {
                return builtin_print(scope);
            }, "print(value, ...)"));
            
        scopeManager.defineFunction("count", new BuiltinFunction(
            [](Scope* scope) -> Value* {
                return builtin_count(scope);
            }, "count(value)"));
            
        scopeManager.defineFunction("cin", new BuiltinFunction(
            [](Scope* scope) -> Value* {
                return builtin_cin(scope);
            }, "cin()"));
            
        scopeManager.defineFunction("abs", new BuiltinFunction(
            [](Scope* scope) -> Value* {
                return builtin_abs(scope);
            }, "abs(value)"));
            
        scopeManager.defineFunction("max", new BuiltinFunction(
            [](Scope* scope) -> Value* {
                return builtin_max(scope);
            }, "max(value, ...)"));
            
        scopeManager.defineFunction("min", new BuiltinFunction(
            [](Scope* scope) -> Value* {
                return builtin_min(scope);
            }, "min(value, ...)"));
            
        scopeManager.defineFunction("pow", new BuiltinFunction(
            [](Scope* scope) -> Value* {
                return builtin_pow(scope);
            }, "pow(base, exponent)"));
            
        scopeManager.defineFunction("length", new BuiltinFunction(
            [](Scope* scope) -> Value* {
                return builtin_length(scope);
            }, "length(value)"));
            
        scopeManager.defineFunction("substring", new BuiltinFunction(
            [](Scope* scope) -> Value* {
                return builtin_substring(scope);
            }, "substring(string, start, length)"));
            
        scopeManager.defineFunction("upper", new BuiltinFunction(
            [](Scope* scope) -> Value* {
                return builtin_upper(scope);
            }, "upper(string)"));
            
        scopeManager.defineFunction("lower", new BuiltinFunction(
            [](Scope* scope) -> Value* {
                return builtin_lower(scope);
            }, "lower(string)"));
            
        scopeManager.defineFunction("push", new BuiltinFunction(
            [](Scope* scope) -> Value* {
                return builtin_push(scope);
            }, "push(array, value)"));
            
        scopeManager.defineFunction("pop", new BuiltinFunction(
            [](Scope* scope) -> Value* {
                return builtin_pop(scope);
            }, "pop(array)"));
            
        scopeManager.defineFunction("sort", new BuiltinFunction(
            [](Scope* scope) -> Value* {
                return builtin_sort(scope);
            }, "sort(array)"));
            
        scopeManager.defineFunction("to_string", new BuiltinFunction(
            [](Scope* scope) -> Value* {
                return builtin_to_string(scope);
            }, "to_string(value)"));
            
        scopeManager.defineFunction("to_int", new BuiltinFunction(
            [](Scope* scope) -> Value* {
                return builtin_to_int(scope);
            }, "to_int(value)"));
            
        scopeManager.defineFunction("to_double", new BuiltinFunction(
            [](Scope* scope) -> Value* {
                return builtin_to_double(scope);
            }, "to_double(value)"));
            
        scopeManager.defineFunction("cast", new BuiltinFunction(
            [](Scope* scope) -> Value* {
                return builtin_cast(scope);
            }, "cast(value, type)"));
            
        scopeManager.defineFunction("random", new BuiltinFunction(
            [](Scope* scope) -> Value* {
                return builtin_random(scope);
            }, "random(max)"));
            
        scopeManager.defineFunction("exit", new BuiltinFunction(
            [](Scope* scope) -> Value* {
                return builtin_exit(scope);
            }, "exit(code)"));
    }
};

// 导出插件
EXPORT_PLUGIN(BasicBuiltinPlugin)
