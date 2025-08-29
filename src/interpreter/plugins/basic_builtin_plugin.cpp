#include "interpreter/builtin_plugin.h"
#include "interpreter/builtin.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>

using namespace std;

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
        // 基础函数
        scopeManager.defineIdentifier("print", new BuiltinFunction("print", builtin_print));
        scopeManager.defineIdentifier("count", new BuiltinFunction("count", builtin_count));
        scopeManager.defineIdentifier("cin", new BuiltinFunction("cin", builtin_cin));
        
        // 数学函数
        scopeManager.defineIdentifier("abs", new BuiltinFunction("abs", builtin_abs));
        scopeManager.defineIdentifier("max", new BuiltinFunction("max", builtin_max));
        scopeManager.defineIdentifier("min", new BuiltinFunction("min", builtin_min));
        scopeManager.defineIdentifier("pow", new BuiltinFunction("pow", builtin_pow));
        
        // 字符串函数
        scopeManager.defineIdentifier("length", new BuiltinFunction("length", builtin_length));
        scopeManager.defineIdentifier("substring", new BuiltinFunction("substring", builtin_substring));
        scopeManager.defineIdentifier("upper", new BuiltinFunction("upper", builtin_upper));
        scopeManager.defineIdentifier("lower", new BuiltinFunction("lower", builtin_lower));
        
        // 数组函数
        scopeManager.defineIdentifier("push", new BuiltinFunction("push", builtin_push));
        scopeManager.defineIdentifier("pop", new BuiltinFunction("pop", builtin_pop));
        scopeManager.defineIdentifier("sort", new BuiltinFunction("sort", builtin_sort));
        
        // 类型转换函数
        scopeManager.defineIdentifier("to_string", new BuiltinFunction("to_string", builtin_to_string));
        scopeManager.defineIdentifier("to_int", new BuiltinFunction("to_int", builtin_to_int));
        scopeManager.defineIdentifier("to_double", new BuiltinFunction("to_double", builtin_to_double));
        scopeManager.defineIdentifier("cast", new BuiltinFunction("cast", builtin_cast));
        
        // 系统函数
        scopeManager.defineIdentifier("random", new BuiltinFunction("random", builtin_random));
        scopeManager.defineIdentifier("exit", new BuiltinFunction("exit", builtin_exit));
    }
    
    map<string, BuiltinFunctionPtr> getFunctionMap() const override {
        return {
            {"print", builtin_print},
            {"count", builtin_count},
            {"cin", builtin_cin},
            {"abs", builtin_abs},
            {"max", builtin_max},
            {"min", builtin_min},
            {"pow", builtin_pow},
            {"length", builtin_length},
            {"substring", builtin_substring},
            {"upper", builtin_upper},
            {"lower", builtin_lower},
            {"push", builtin_push},
            {"pop", builtin_pop},
            {"sort", builtin_sort},
            {"to_string", builtin_to_string},
            {"to_int", builtin_to_int},
            {"to_double", builtin_to_double},
            {"cast", builtin_cast},
            {"random", builtin_random},
            {"exit", builtin_exit}
        };
    }
};

// 导出插件
EXPORT_PLUGIN(BasicBuiltinPlugin)
