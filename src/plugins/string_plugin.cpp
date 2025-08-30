#include "interpreter/builtin_plugin.h"
#include "interpreter/value.h"
#include "parser/function.h"
#include "interpreter/scope.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <vector>

using namespace std;

// 字符串函数实现
Value* string_trim(vector<Value*>& args) {
    if (args.size() != 1 || !args[0]) return nullptr;
    
    Value* val = args[0];
    if (String* str = dynamic_cast<String*>(val)) {
        string s = str->getValue();
        s.erase(0, s.find_first_not_of(" \t\n\r"));
        s.erase(s.find_last_not_of(" \t\n\r") + 1);
        return new String(s);
    }
    return nullptr;
}

Value* string_replace(vector<Value*>& args) {
    if (args.size() != 3 || !args[0] || !args[1] || !args[2]) return nullptr;
    
    Value* strVal = args[0];
    Value* oldVal = args[1];
    Value* newVal = args[2];
    
    if (String* str = dynamic_cast<String*>(strVal)) {
        if (String* oldStr = dynamic_cast<String*>(oldVal)) {
            if (String* newStr = dynamic_cast<String*>(newVal)) {
                string result = str->getValue();
                string oldStr_val = oldStr->getValue();
                string newStr_val = newStr->getValue();
                
                size_t pos = 0;
                while ((pos = result.find(oldStr_val, pos)) != string::npos) {
                    result.replace(pos, oldStr_val.length(), newStr_val);
                    pos += newStr_val.length();
                }
                return new String(result);
            }
        }
    }
    return nullptr;
}

Value* string_split(vector<Value*>& args) {
    if (args.size() != 2 || !args[0] || !args[1]) return nullptr;
    
    Value* strVal = args[0];
    Value* delimVal = args[1];
    
    if (String* str = dynamic_cast<String*>(strVal)) {
        if (String* delim = dynamic_cast<String*>(delimVal)) {
            string s = str->getValue();
            string delimiter = delim->getValue();
            
            Array* result = new Array();
            size_t pos = 0;
            string token;
            
            while ((pos = s.find(delimiter)) != string::npos) {
                token = s.substr(0, pos);
                result->addElement(new String(token));
                s.erase(0, pos + delimiter.length());
            }
            result->addElement(new String(s));
            
            return result;
        }
    }
    return nullptr;
}

Value* string_join(vector<Value*>& args) {
    if (args.size() != 2 || !args[0] || !args[1]) return nullptr;
    
    Value* arrVal = args[0];
    Value* delimVal = args[1];
    
    if (Array* arr = dynamic_cast<Array*>(arrVal)) {
        if (String* delim = dynamic_cast<String*>(delimVal)) {
            string delimiter = delim->getValue();
            string result;
            
            for (size_t i = 0; i < arr->size(); ++i) {
                Value* element = arr->getElement(i);
                if (String* str = dynamic_cast<String*>(element)) {
                    if (i > 0) result += delimiter;
                    result += str->getValue();
                }
            }
            
            return new String(result);
        }
    }
    return nullptr;
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
