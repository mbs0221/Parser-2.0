#include "interpreter/builtin_plugin.h" 
#include "interpreter/value.h"
#include "interpreter/type_registry.h"
#include "parser/function.h"
#include "interpreter/scope.h"
#include "interpreter/interpreter.h"
#include <iostream>
#include <cmath>
#include <vector>

using namespace std;

// 通用的数学函数实现 - 通过类型系统调用方法
Value* math_sin(vector<Value*>& args) {
    if (args.size() != 1 || !args[0]) return nullptr;
    
    Value* val = args[0];
    ObjectType* type = val->getValueType();
    if (!type) return nullptr;
    
    // 通过类型系统调用sin方法
    vector<Value*> methodArgs;
    return type->callMethod(val, "sin", methodArgs);
}

Value* math_cos(vector<Value*>& args) {
    if (args.size() != 1 || !args[0]) return nullptr;
    
    Value* val = args[0];
    ObjectType* type = val->getValueType();
    if (!type) return nullptr;
    
    // 通过类型系统调用cos方法
    vector<Value*> methodArgs;
    return type->callMethod(val, "cos", methodArgs);
}

Value* math_tan(vector<Value*>& args) {
    if (args.size() != 1 || !args[0]) return nullptr;
    
    Value* val = args[0];
    ObjectType* type = val->getValueType();
    if (!type) return nullptr;
    
    // 通过类型系统调用tan方法
    vector<Value*> methodArgs;
    return type->callMethod(val, "tan", methodArgs);
}

Value* math_sqrt(vector<Value*>& args) {
    if (args.size() != 1 || !args[0]) return nullptr;
    
    Value* val = args[0];
    ObjectType* type = val->getValueType();
    if (!type) return nullptr;
    
    // 通过类型系统调用sqrt方法
    vector<Value*> methodArgs;
    return type->callMethod(val, "sqrt", methodArgs);
}

Value* math_log(vector<Value*>& args) {
    if (args.size() != 1 || !args[0]) return nullptr;
    
    Value* val = args[0];
    ObjectType* type = val->getValueType();
    if (!type) return nullptr;
    
    // 通过类型系统调用log方法
    vector<Value*> methodArgs;
    return type->callMethod(val, "log", methodArgs);
}

Value* math_floor(vector<Value*>& args) {
    if (args.size() != 1 || !args[0]) return nullptr;
    
    Value* val = args[0];
    ObjectType* type = val->getValueType();
    if (!type) return nullptr;
    
    // 通过类型系统调用floor方法
    vector<Value*> methodArgs;
    return type->callMethod(val, "floor", methodArgs);
}

Value* math_ceil(vector<Value*>& args) {
    if (args.size() != 1 || !args[0]) return nullptr;
    
    Value* val = args[0];
    ObjectType* type = val->getValueType();
    if (!type) return nullptr;
    
    // 通过类型系统调用ceil方法
    vector<Value*> methodArgs;
    return type->callMethod(val, "ceil", methodArgs);
}

Value* math_round(vector<Value*>& args) {
    if (args.size() != 1 || !args[0]) return nullptr;
    
    Value* val = args[0];
    ObjectType* type = val->getValueType();
    if (!type) return nullptr;
    
    // 通过类型系统调用round方法
    vector<Value*> methodArgs;
    return type->callMethod(val, "round", methodArgs);
}

Value* math_abs(vector<Value*>& args) {
    if (args.size() != 1 || !args[0]) return nullptr;
    
    Value* val = args[0];
    ObjectType* type = val->getValueType();
    if (!type) return nullptr;
    
    // 通过类型系统调用abs方法
    vector<Value*> methodArgs;
    return type->callMethod(val, "abs", methodArgs);
}

// 数学函数插件类
class MathPlugin : public BuiltinPlugin {
public:
    PluginInfo getPluginInfo() const override {
        return PluginInfo{
            "math_plugin",
            "1.0.0",
            "数学函数插件，通过类型系统调用数学运算方法",
            {"sin", "cos", "tan", "sqrt", "log", "floor", "ceil", "round", "abs"}
        };
    }
    
    void registerFunctions(ScopeManager& scopeManager) override {
        // 使用辅助方法批量注册函数
        defineBuiltinFunctions(scopeManager, getFunctionMap());
    }
    
    map<string, BuiltinFunctionPtr> getFunctionMap() const override {
        return {
            {"sin", math_sin},
            {"cos", math_cos},
            {"tan", math_tan},
            {"sqrt", math_sqrt},
            {"log", math_log},
            {"floor", math_floor},
            {"ceil", math_ceil},
            {"round", math_round},
            {"abs", math_abs}
        };
    }
};

// 导出插件
EXPORT_PLUGIN(MathPlugin)
