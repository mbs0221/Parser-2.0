#include "interpreter/builtin_plugin.h"
#include "interpreter/value.h"
#include "interpreter/type_registry.h"
#include "parser/function.h"
#include "interpreter/scope.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <vector>

using namespace std;

// 字符串函数实现 - 通过类型系统调用方法
Value* string_trim(vector<Value*>& args) {
    if (args.size() != 1 || !args[0]) return nullptr;
    
    Value* val = args[0];
    ObjectType* type = val->getValueType();
    if (!type) return nullptr;
    
    // 通过类型系统调用trim方法
    vector<Value*> methodArgs;
    return type->callMethod(val, "trim", methodArgs);
}

Value* string_replace(vector<Value*>& args) {
    if (args.size() != 3 || !args[0] || !args[1] || !args[2]) return nullptr;
    
    Value* strVal = args[0];
    ObjectType* type = strVal->getValueType();
    if (!type) return nullptr;
    
    // 通过类型系统调用replace方法
    vector<Value*> methodArgs = {args[1], args[2]};
    return type->callMethod(strVal, "replace", methodArgs);
}

Value* string_split(vector<Value*>& args) {
    if (args.size() != 2 || !args[0] || !args[1]) return nullptr;
    
    Value* strVal = args[0];
    ObjectType* type = strVal->getValueType();
    if (!type) return nullptr;
    
    // 通过类型系统调用split方法
    vector<Value*> methodArgs = {args[1]};
    return type->callMethod(strVal, "split", methodArgs);
}

Value* string_join(vector<Value*>& args) {
    if (args.size() != 2 || !args[0] || !args[1]) return nullptr;
    
    Value* arrVal = args[0];
    ObjectType* type = arrVal->getValueType();
    if (!type) return nullptr;
    
    // 通过类型系统调用join方法
    vector<Value*> methodArgs = {args[1]};
    return type->callMethod(arrVal, "join", methodArgs);
}

Value* string_starts_with(vector<Value*>& args) {
    if (args.size() != 2 || !args[0] || !args[1]) return nullptr;
    
    Value* strVal = args[0];
    Value* prefixVal = args[1];
    
    if (String* str = dynamic_cast<String*>(strVal)) {
        if (String* prefix = dynamic_cast<String*>(prefixVal)) {
            string s = str->getValue();
            string prefix_str = prefix->getValue();
            bool result = s.substr(0, prefix_str.length()) == prefix_str;
            return new Bool(result);
        }
    }
    return nullptr;
}

Value* string_ends_with(vector<Value*>& args) {
    if (args.size() != 2 || !args[0] || !args[1]) return nullptr;
    
    Value* strVal = args[0];
    Value* suffixVal = args[1];
    
    if (String* str = dynamic_cast<String*>(strVal)) {
        if (String* suffix = dynamic_cast<String*>(suffixVal)) {
            string s = str->getValue();
            string suffix_str = suffix->getValue();
            bool result = s.length() >= suffix_str.length() && 
                         s.substr(s.length() - suffix_str.length()) == suffix_str;
            return new Bool(result);
        }
    }
    return nullptr;
}

Value* string_contains(vector<Value*>& args) {
    if (args.size() != 2 || !args[0] || !args[1]) return nullptr;
    
    Value* strVal = args[0];
    Value* substrVal = args[1];
    
    if (String* str = dynamic_cast<String*>(strVal)) {
        if (String* substr = dynamic_cast<String*>(substrVal)) {
            string s = str->getValue();
            string sub = substr->getValue();
            bool result = s.find(sub) != string::npos;
            return new Bool(result);
        }
    }
    return nullptr;
}

// 字符串处理插件类
class StringPlugin : public BuiltinPlugin {
public:
    PluginInfo getPluginInfo() const override {
        return PluginInfo{
            "string_plugin",
            "1.0.0",
            "字符串处理插件，包含trim、replace、split、join等字符串操作",
            {"trim", "replace", "split", "join", "starts_with", "ends_with", "contains"}
        };
    }
    
    void registerFunctions(ScopeManager& scopeManager) override {
        // 使用辅助方法批量注册函数
        defineBuiltinFunctions(scopeManager, getFunctionMap());
    }
    
    map<string, BuiltinFunctionPtr> getFunctionMap() const override {
        return {
            {"trim", string_trim},
            {"replace", string_replace},
            {"split", string_split},
            {"join", string_join},
            {"starts_with", string_starts_with},
            {"ends_with", string_ends_with},
            {"contains", string_contains}
        };
    }
};

// 导出插件
EXPORT_PLUGIN(StringPlugin)
