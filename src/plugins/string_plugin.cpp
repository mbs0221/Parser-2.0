#include "interpreter/plugins/builtin_plugin.h"
#include "interpreter/values/value.h"
#include "interpreter/types/types.h"
#include "parser/definition.h"
#include "interpreter/scope/scope.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <vector>

using namespace std;

// 使用宏自动生成字符串函数实现
AUTO_PLUGIN_STRING_FUNCTION(string_trim, "trim")
AUTO_PLUGIN_STRING_FUNCTION(string_replace, "replace")
AUTO_PLUGIN_STRING_FUNCTION(string_split, "split")
AUTO_PLUGIN_STRING_FUNCTION(string_join, "join")

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
    
protected:
    map<string, FunctionInfo> getFunctionInfoMap() const override {
        return {
            {"trim", {string_trim, {"string"}, "去除字符串首尾空白字符"}},
            {"replace", {string_replace, {"string", "old", "new"}, "替换字符串中的子串"}},
            {"split", {string_split, {"string", "delimiter"}, "按分隔符分割字符串"}},
            {"join", {string_join, {"array", "separator"}, "用分隔符连接字符串数组"}},
            {"starts_with", {string_starts_with, {"string", "prefix"}, "检查字符串是否以指定前缀开头"}},
            {"ends_with", {string_ends_with, {"string", "suffix"}, "检查字符串是否以指定后缀结尾"}},
            {"contains", {string_contains, {"string", "substring"}, "检查字符串是否包含指定子串"}}
        };
    }
};

// 导出插件
EXPORT_PLUGIN(StringPlugin)
