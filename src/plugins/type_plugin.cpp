#include "interpreter/builtin_plugin.h"
#include "lexer/value.h"
#include "parser/function.h"

#include <iostream>
#include <sstream>

using namespace std;

// ==================== 类型转换函数 ====================

Value* type_to_string(vector<Variable*>& args) {
    if (args.size() != 1 || !args[0]) return nullptr;
    
    Value* val = args[0]->getValue();
    if (val) {
        return new String(val->str());
    }
    return nullptr;
}

Value* type_to_int(vector<Variable*>& args) {
    if (args.size() != 1 || !args[0]) return nullptr;
    
    Value* val = args[0]->getValue();
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
    return nullptr;
}

Value* type_to_double(vector<Variable*>& args) {
    if (args.size() != 1 || !args[0]) return nullptr;
    
    Value* val = args[0]->getValue();
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
    return nullptr;
}

Value* type_cast(vector<Variable*>& args) {
    if (args.size() != 2 || !args[0] || !args[1]) return nullptr;
    
    Value* val = args[0]->getValue();
    Value* typeVal = args[1]->getValue();
    
    if (!val || !typeVal) return nullptr;
    
    if (String* typeStr = dynamic_cast<String*>(typeVal)) {
        string type = typeStr->getValue();
        if (type == "int" || type == "integer") {
            vector<Variable*> tempArgs = {args[0]};
            return type_to_int(tempArgs);
        } else if (type == "double" || type == "float") {
            vector<Variable*> tempArgs = {args[0]};
            return type_to_double(tempArgs);
        } else if (type == "string") {
            vector<Variable*> tempArgs = {args[0]};
            return type_to_string(tempArgs);
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
    
    void registerFunctions(ScopeManager& scopeManager) override {
        // 使用辅助方法批量注册函数
        defineBuiltinFunctions(scopeManager, getFunctionMap());
    }
    
    map<string, BuiltinFunctionPtr> getFunctionMap() const override {
        return {
            {"to_string", type_to_string},
            {"to_int", type_to_int},
            {"to_double", type_to_double},
            {"cast", type_cast}
        };
    }
};

// 导出插件
EXPORT_PLUGIN(TypePlugin)
