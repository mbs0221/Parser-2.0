#include "interpreter/plugins/builtin_plugin.h"
// #include "interpreter/plugins/plugin_manager.h"
#include "interpreter/values/value.h"
#include "interpreter/types/types.h"
#include "parser/definition.h"

#include <iostream>
#include <sstream>

using namespace std;

// ==================== 类型转换函数 - 使用新的scope接口 ====================

// 类型转换函数 - 通过类型系统调用convertTo方法
Value* type_cast(Scope* scope) {
    if (Value* val = scope->getArgument<Value>("value")) {
        if (Value* typeVal = scope->getArgument<Value>("type")) {
            if (val && typeVal) {
                // 通过类型系统调用convertTo方法
                vector<Value*> methodArgs = {typeVal};
                return PluginManager::callMethodOnValue(val, "convertTo", methodArgs);
            }
        }
    }
    return nullptr;
}

// 类型转换函数 - 转换为字符串
Value* type_to_string(Scope* scope) {
    if (Value* val = scope->getArgument<Value>("value")) {
        if (val) {
            return new String(val->toString());
        }
    }
    return nullptr;
}

// 类型转换函数 - 转换为整数
Value* type_to_int(Scope* scope) {
    if (Value* val = scope->getArgument<Value>("value")) {
        if (val) {
            if (Integer* intVal = dynamic_cast<Integer*>(val)) {
                return new Integer(intVal->getValue());
            } else if (Double* doubleVal = dynamic_cast<Double*>(val)) {
                return new Integer(static_cast<int>(doubleVal->getValue()));
            } else if (String* strVal = dynamic_cast<String*>(val)) {
                try {
                    return new Integer(stoi(strVal->getValue()));
                } catch (...) {
                    return nullptr;
                }
            }
        }
    }
    return nullptr;
}

// 类型转换函数 - 转换为浮点数
Value* type_to_double(Scope* scope) {
    if (Value* val = scope->getArgument<Value>("value")) {
        if (val) {
            if (Integer* intVal = dynamic_cast<Integer*>(val)) {
                return new Double(static_cast<double>(intVal->getValue()));
            } else if (Double* doubleVal = dynamic_cast<Double*>(val)) {
                return new Double(doubleVal->getValue());
            } else if (String* strVal = dynamic_cast<String*>(val)) {
                try {
                    return new Double(stod(strVal->getValue()));
                } catch (...) {
                    return nullptr;
                }
            }
        }
    }
    return nullptr;
}

// 类型转换插件类
class TypePlugin : public BuiltinPlugin {
public:
    PluginInfo getPluginInfo() const override {
        return PluginInfo{
            "type",
            "1.0.0",
            "类型转换插件，包含to_string、to_int、to_double、cast等类型转换函数",
            {"to_string", "to_int", "to_double", "cast"}
        };
    }
    
protected:
    // 重写注册函数方法，使用新的scope接口和C函数原型解析
    void registerFunctions(ScopeManager& scopeManager) override {
        LOG_DEBUG("TypePlugin::registerFunctions called with new scope interface and C function prototype parsing");
        
        // 使用C函数原型直接创建BuiltinFunction对象
        vector<BuiltinFunction*> funcObjects = {
            new BuiltinFunction([](Scope* scope) -> Value* { return type_to_string(scope); }, "to_string(value)"),
            new BuiltinFunction([](Scope* scope) -> Value* { return type_to_int(scope); }, "to_int(value)"),
            new BuiltinFunction([](Scope* scope) -> Value* { return type_to_double(scope); }, "to_double(value)"),
            new BuiltinFunction([](Scope* scope) -> Value* { return type_cast(scope); }, "cast(value, type)")
        };
        
        LOG_DEBUG("Created " + to_string(funcObjects.size()) + " functions with C function prototype parsing");
        
        for (BuiltinFunction* func : funcObjects) {
            LOG_DEBUG("Registering function '" + func->getName() + "' with " + to_string(func->getParameters().size()) + " parameters");
            scopeManager.defineFunction(func->getName(), func);
        }
    }
};

// 导出插件
EXPORT_PLUGIN(TypePlugin)
