#include "interpreter/plugins/builtin_plugin.h"
#include "interpreter/values/value.h"
#include "interpreter/types/types.h"
#include "parser/definition.h"

#include <iostream>
#include <sstream>

using namespace std;

// ==================== 类型转换函数 - 使用宏自动生成 ====================

// 使用宏自动生成类型转换函数实现
AUTO_PLUGIN_TYPE_FUNCTION(type_to_string, "to_string")
AUTO_PLUGIN_TYPE_FUNCTION(type_to_int, "to_int")
AUTO_PLUGIN_TYPE_FUNCTION(type_to_double, "to_double")

// 类型转换函数 - 通过类型系统调用convertTo方法
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
    
protected:
    map<string, FunctionInfo> getFunctionInfoMap() const override {
        return {
            {"to_string", {type_to_string, {"value"}, "将值转换为字符串"}},
            {"to_int", {type_to_int, {"value"}, "将值转换为整数"}},
            {"to_double", {type_to_double, {"value"}, "将值转换为浮点数"}},
            {"cast", {type_cast, {"value", "type"}, "将值转换为指定类型"}}
        };
    }
};

// 导出插件
EXPORT_PLUGIN(TypePlugin)
