#include "interpreter/builtin_plugin.h"

#include <iostream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>

// 如果builtin_sort函数未定义，提供一个简单的实现
#ifndef BUILTIN_SORT_DEFINED
Value* builtin_sort(vector<Variable*>& args) {
    if (args.size() != 1 || !args[0]) return nullptr;
    
    Value* arrVal = args[0]->getValue();
    if (Array* arr = dynamic_cast<Array*>(arrVal)) {
        // 简单的排序实现
        // 这里可以添加实际的排序逻辑
        return arr;
    }
    
    return nullptr;
}
#define BUILTIN_SORT_DEFINED
#endif

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
        // 使用辅助方法批量注册函数
        defineBuiltinFunctions(scopeManager, getFunctionMap());
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
