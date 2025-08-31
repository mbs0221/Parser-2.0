#include "interpreter/builtin_plugin.h"
#include "interpreter/value.h"
#include "interpreter/type_registry.h"
#include "parser/function.h"
#include <iostream>
#include <algorithm>

using namespace std;

// ==================== 数组操作函数 - 通过类型系统调用方法 ====================

Value* array_push(vector<Value*>& args) {
    if (args.size() < 2 || !args[0]) return nullptr;
    
    Value* arrVal = args[0];
    ObjectType* type = arrVal->getValueType();
    if (!type) return nullptr;
    
    // 通过类型系统调用push方法
    vector<Value*> methodArgs(args.begin() + 1, args.end());
    return type->callMethod(arrVal, "push", methodArgs);
}

Value* array_pop(vector<Value*>& args) {
    if (args.size() != 1 || !args[0]) return nullptr;
    
    Value* arrVal = args[0];
    ObjectType* type = arrVal->getValueType();
    if (!type) return nullptr;
    
    // 通过类型系统调用pop方法
    vector<Value*> methodArgs;
    return type->callMethod(arrVal, "pop", methodArgs);
}

Value* array_sort(vector<Value*>& args) {
    if (args.size() != 1 || !args[0]) return nullptr;
    
    Value* arrVal = args[0];
    ObjectType* type = arrVal->getValueType();
    if (!type) return nullptr;
    
    // 通过类型系统调用sort方法
    vector<Value*> methodArgs;
    return type->callMethod(arrVal, "sort", methodArgs);
}

Value* array_length(vector<Value*>& args) {
    if (args.size() != 1 || !args[0]) return nullptr;
    
    Value* val = args[0];
    ObjectType* type = val->getValueType();
    if (!type) return nullptr;
    
    // 通过类型系统调用length方法
    vector<Value*> methodArgs;
    return type->callMethod(val, "length", methodArgs);
}

// 数组操作插件类
class ArrayPlugin : public BuiltinPlugin {
public:
    PluginInfo getPluginInfo() const override {
        return PluginInfo{
            "array",
            "1.0.0",
            "数组操作插件，通过类型系统调用数组操作方法",
            {"push", "pop", "sort", "length"}
        };
    }
    
    void registerFunctions(ScopeManager& scopeManager) override {
        // 使用辅助方法批量注册函数
        defineBuiltinFunctions(scopeManager, getFunctionMap());
    }
    
    map<string, BuiltinFunctionPtr> getFunctionMap() const override {
        return {
            {"push", array_push},
            {"pop", array_pop},
            {"sort", array_sort},
            {"length", array_length}
        };
    }
};

// 导出插件
EXPORT_PLUGIN(ArrayPlugin)
