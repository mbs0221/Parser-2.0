#include "interpreter/builtin_plugin.h"
#include "interpreter/value.h"
#include "interpreter/type_registry.h"
#include "parser/function.h"

#include <iostream>
#include <sstream>

using namespace std;

// ==================== 类型转换函数 - 通过类型系统调用方法 ====================

Value* type_to_string(vector<Value*>& args) {
    if (args.size() != 1 || !args[0]) return nullptr;
    
    Value* val = args[0];
    ObjectType* type = val->getValueType();
    if (!type) return nullptr;
    
    // 通过类型系统调用to_string方法
    vector<Value*> methodArgs;
    return type->callMethod(val, "to_string", methodArgs);
}

Value* type_to_int(vector<Value*>& args) {
    if (args.size() != 1 || !args[0]) return nullptr;
    
    Value* val = args[0];
    ObjectType* type = val->getValueType();
    if (!type) return nullptr;
    
    // 通过类型系统调用to_int方法
    vector<Value*> methodArgs;
    return type->callMethod(val, "to_int", methodArgs);
}

Value* type_to_double(vector<Value*>& args) {
    if (args.size() != 1 || !args[0]) return nullptr;
    
    Value* val = args[0];
    ObjectType* type = val->getValueType();
    if (!type) return nullptr;
    
    // 通过类型系统调用to_double方法
    vector<Value*> methodArgs;
    return type->callMethod(val, "to_double", methodArgs);
}

Value* type_cast(vector<Value*>& args) {
    if (args.size() != 2 || !args[0] || !args[1]) return nullptr;
    
    Value* val = args[0];
    Value* typeVal = args[1];
    
    if (!val || !typeVal) return nullptr;
    
    ObjectType* sourceType = val->getValueType();
    if (!sourceType) return nullptr;
    
    // 通过类型系统调用convertTo方法
    vector<Value*> methodArgs = {typeVal};
    return sourceType->callMethod(val, "convertTo", methodArgs);
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
