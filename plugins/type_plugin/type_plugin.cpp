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
    std::vector<FunctionInfo> getFunctions() override {
        std::vector<FunctionInfo> functions;
        
        // 添加类型转换函数
        functions.emplace_back("to_string", "to_string(value)", 
            std::vector<std::string>{"value"},
            [](Scope* scope) -> Value* {
                return type_to_string(scope);
            });
            
        functions.emplace_back("to_int", "to_int(value)", 
            std::vector<std::string>{"value"},
            [](Scope* scope) -> Value* {
                return type_to_int(scope);
            });
            
        functions.emplace_back("to_double", "to_double(value)", 
            std::vector<std::string>{"value"},
            [](Scope* scope) -> Value* {
                return type_to_double(scope);
            });
            
        functions.emplace_back("cast", "cast(value, type)", 
            std::vector<std::string>{"value", "type"},
            [](Scope* scope) -> Value* {
                return type_cast(scope);
            });
            
        return functions;
    }
};

// 导出插件
EXPORT_PLUGIN(TypePlugin)
