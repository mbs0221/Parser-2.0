#include "interpreter/builtin_plugin.h"

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
        // 使用宏简化内置函数注册
        REGISTER_BUILTIN_FUNCTION(scopeManager, "print", builtin_print, "print(value, ...)");
        REGISTER_BUILTIN_FUNCTION(scopeManager, "count", builtin_count, "count(value)");
        REGISTER_BUILTIN_FUNCTION(scopeManager, "cin", builtin_cin, "cin()");
        REGISTER_BUILTIN_FUNCTION(scopeManager, "abs", builtin_abs, "abs(value)");
        REGISTER_BUILTIN_FUNCTION(scopeManager, "max", builtin_max, "max(value, ...)");
        REGISTER_BUILTIN_FUNCTION(scopeManager, "min", builtin_min, "min(value, ...)");
        REGISTER_BUILTIN_FUNCTION(scopeManager, "pow", builtin_pow, "pow(base, exponent)");
        REGISTER_BUILTIN_FUNCTION(scopeManager, "length", builtin_length, "length(value)");
        REGISTER_BUILTIN_FUNCTION(scopeManager, "substring", builtin_substring, "substring(string, start, length)");
        REGISTER_BUILTIN_FUNCTION(scopeManager, "upper", builtin_upper, "upper(string)");
        REGISTER_BUILTIN_FUNCTION(scopeManager, "lower", builtin_lower, "lower(string)");
        REGISTER_BUILTIN_FUNCTION(scopeManager, "push", builtin_push, "push(array, value)");
        REGISTER_BUILTIN_FUNCTION(scopeManager, "pop", builtin_pop, "pop(array)");
        REGISTER_BUILTIN_FUNCTION(scopeManager, "sort", builtin_sort, "sort(array)");
        REGISTER_BUILTIN_FUNCTION(scopeManager, "to_string", builtin_to_string, "to_string(value)");
        REGISTER_BUILTIN_FUNCTION(scopeManager, "to_int", builtin_to_int, "to_int(value)");
        REGISTER_BUILTIN_FUNCTION(scopeManager, "to_double", builtin_to_double, "to_double(value)");
        REGISTER_BUILTIN_FUNCTION(scopeManager, "cast", builtin_cast, "cast(value, type)");
        REGISTER_BUILTIN_FUNCTION(scopeManager, "random", builtin_random, "random(max)");
        REGISTER_BUILTIN_FUNCTION(scopeManager, "exit", builtin_exit, "exit(code)");
        
        LOG_DEBUG("Registered " + to_string(21) + " functions using simplified macro interface");
    }
};

// 导出插件
EXPORT_PLUGIN(BasicBuiltinPlugin)
