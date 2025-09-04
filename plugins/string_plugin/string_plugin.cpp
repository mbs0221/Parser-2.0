#include "interpreter/plugins/builtin_plugin.h"
#include "interpreter/values/value.h"
#include "interpreter/types/types.h"
#include "parser/definition.h"
#include "interpreter/scope/scope.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <vector>
#include <string>
#include <cstdio>

using namespace std;

// 前向声明
string applyFormatSpec(const string& value, const string& formatSpec, Value* originalValue);

// 手动实现字符串函数
Value* string_trim(vector<Value*>& args) {
    if (args.size() != 1 || !args[0]) return nullptr;
    
    String* str = dynamic_cast<String*>(args[0]);
    if (!str) return nullptr;
    
    string s = str->getValue();
    // 去除首尾空白字符
    size_t start = s.find_first_not_of(" \t\n\r");
    if (start == string::npos) return new String("");
    
    size_t end = s.find_last_not_of(" \t\n\r");
    return new String(s.substr(start, end - start + 1));
}

Value* string_replace(vector<Value*>& args) {
    if (args.size() != 3 || !args[0] || !args[1] || !args[2]) return nullptr;
    
    String* str = dynamic_cast<String*>(args[0]);
    String* oldStr = dynamic_cast<String*>(args[1]);
    String* newStr = dynamic_cast<String*>(args[2]);
    
    if (!str || !oldStr || !newStr) return nullptr;
    
    string s = str->getValue();
    string old_s = oldStr->getValue();
    string new_s = newStr->getValue();
    
    size_t pos = 0;
    while ((pos = s.find(old_s, pos)) != string::npos) {
        s.replace(pos, old_s.length(), new_s);
        pos += new_s.length();
    }
    
    return new String(s);
}

Value* string_split(vector<Value*>& args) {
    if (args.size() != 2 || !args[0] || !args[1]) return nullptr;
    
    String* str = dynamic_cast<String*>(args[0]);
    String* delimiter = dynamic_cast<String*>(args[1]);
    
    if (!str || !delimiter) return nullptr;
    
    string s = str->getValue();
    string delim = delimiter->getValue();
    
    vector<String*> result;
    size_t start = 0;
    size_t end = s.find(delim);
    
    while (end != string::npos) {
        result.push_back(new String(s.substr(start, end - start)));
        start = end + delim.length();
        end = s.find(delim, start);
    }
    result.push_back(new String(s.substr(start)));
    
    Array* array = new Array();
    for (String* item : result) {
        array->addElement(item);
    }
    
    return array;
}

Value* string_join(vector<Value*>& args) {
    if (args.size() != 2 || !args[0] || !args[1]) return nullptr;
    
    Array* array = dynamic_cast<Array*>(args[0]);
    String* separator = dynamic_cast<String*>(args[1]);
    
    if (!array || !separator) return nullptr;
    
    string sep = separator->getValue();
    string result;
    
    vector<Value*> elements = array->getElements();
    for (size_t i = 0; i < elements.size(); ++i) {
        if (i > 0) result += sep;
        if (elements[i]) {
            result += elements[i]->toString();
        }
    }
    
    return new String(result);
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

// Python风格的format函数实现
Value* string_format(vector<Value*>& args) {
    if (args.empty() || !args[0]) return nullptr;
    
    String* formatStr = dynamic_cast<String*>(args[0]);
    if (!formatStr) return nullptr;
    
    string result = formatStr->getValue();
    
    // 获取位置参数（除了第一个格式字符串）
    vector<Value*> varArgs(args.begin() + 1, args.end());
    
    // 解析并替换格式化占位符
    size_t pos = 0;
    while (pos < result.length()) {
        size_t start = result.find('{', pos);
        if (start == string::npos) break;
        
        size_t end = result.find('}', start);
        if (end == string::npos) break;
        
        string placeholder = result.substr(start, end - start + 1);
        string content = result.substr(start + 1, end - start - 1);
        
        string replacement;
        
        if (content.empty()) {
            // {} - 使用位置参数
            if (!varArgs.empty()) {
                replacement = varArgs[0] ? varArgs[0]->toString() : "null";
                varArgs.erase(varArgs.begin());
            } else {
                replacement = "{}";
            }
        } else {
            // 解析格式化规范
            size_t colonPos = content.find(':');
            string fieldName = (colonPos != string::npos) ? content.substr(0, colonPos) : content;
            string formatSpec = (colonPos != string::npos) ? content.substr(colonPos + 1) : "";
            
            Value* value = nullptr;
            
            // 确定要格式化的值
            if (fieldName.empty()) {
                // {} - 下一个位置参数
                if (!varArgs.empty()) {
                    value = varArgs[0];
                    varArgs.erase(varArgs.begin());
                }
            } else if (isdigit(fieldName[0])) {
                // 位置参数 {0}, {1}, {2}...
                int index = stoi(fieldName);
                if (index >= 0 && index < (int)varArgs.size()) {
                    value = varArgs[index];
                }
            }
            
            if (value) {
                replacement = value->toString();
                
                // 应用格式化规范
                if (!formatSpec.empty()) {
                    replacement = applyFormatSpec(replacement, formatSpec, value);
                }
            } else {
                replacement = placeholder; // 保持原样
            }
        }
        
        result.replace(start, placeholder.length(), replacement);
        pos = start + replacement.length();
    }
    
    return new String(result);
}

// 辅助函数：应用格式化规范（与string.cpp中的实现相同）
string applyFormatSpec(const string& value, const string& formatSpec, Value* originalValue) {
    string result = value;
    
    // 处理数字格式化
    if (Double* doubleVal = dynamic_cast<Double*>(originalValue)) {
        double num = doubleVal->getValue();
        
        // 处理精度格式化 {:.2f}
        if (formatSpec.find('.') != string::npos) {
            size_t dotPos = formatSpec.find('.');
            size_t endPos = formatSpec.find('f', dotPos);
            if (endPos != string::npos) {
                string precisionStr = formatSpec.substr(dotPos + 1, endPos - dotPos - 1);
                if (!precisionStr.empty()) {
                    int precision = stoi(precisionStr);
                    char buffer[100];
                    snprintf(buffer, sizeof(buffer), "%.*f", precision, num);
                    result = string(buffer);
                }
            }
        }
        // 处理整数部分宽度 {0:4.2f}
        else if (formatSpec.find('f') != string::npos) {
            size_t fPos = formatSpec.find('f');
            if (fPos > 0) {
                string widthStr = formatSpec.substr(0, fPos);
                if (!widthStr.empty()) {
                    int width = stoi(widthStr);
                    char buffer[100];
                    snprintf(buffer, sizeof(buffer), "%*.*f", width, 2, num);
                    result = string(buffer);
                }
            }
        }
    }
    else if (Integer* intVal = dynamic_cast<Integer*>(originalValue)) {
        int num = intVal->getValue();
        
        // 处理整数格式化 {0:4d}
        if (formatSpec.find('d') != string::npos) {
            size_t dPos = formatSpec.find('d');
            if (dPos > 0) {
                string widthStr = formatSpec.substr(0, dPos);
                if (!widthStr.empty()) {
                    int width = stoi(widthStr);
                    char buffer[100];
                    snprintf(buffer, sizeof(buffer), "%*d", width, num);
                    result = string(buffer);
                }
            }
        }
    }
    
    // 处理字符串格式化
    else if (String* strVal = dynamic_cast<String*>(originalValue)) {
        // 处理字符串宽度 {0:10s}
        if (formatSpec.find('s') != string::npos) {
            size_t sPos = formatSpec.find('s');
            if (sPos > 0) {
                string widthStr = formatSpec.substr(0, sPos);
                if (!widthStr.empty()) {
                    int width = stoi(widthStr);
                    if (width > 0) {
                        result = strVal->getValue();
                        if ((int)result.length() < width) {
                            result = string(width - result.length(), ' ') + result;
                        }
                    }
                }
            }
        }
    }
    
    return result;
}

// 字符串处理插件类
class StringPlugin : public BuiltinPlugin {
public:
    PluginInfo getPluginInfo() const override {
        return PluginInfo{
            "string_plugin",
            "1.0.0",
            "字符串处理插件，包含trim、replace、split、join、format等字符串操作",
            {"trim", "replace", "split", "join", "starts_with", "ends_with", "contains", "format"}
        };
    }
    
    std::vector<FunctionInfo> getFunctions() override {
        std::vector<FunctionInfo> functions;
        
        // 添加所有字符串处理函数
        functions.emplace_back("trim", "trim(string)", 
            std::vector<std::string>{"string"},
            [](Scope* scope) -> Value* {
                if (String* str = scope->getArgument<String>("string")) {
                    vector<Value*> args = {str};
                    return string_trim(args);
                }
                return nullptr;
            });
            
        functions.emplace_back("replace", "replace(string, old, new)", 
            std::vector<std::string>{"string", "old", "new"},
            [](Scope* scope) -> Value* {
                String* str = scope->getArgument<String>("string");
                String* oldStr = scope->getArgument<String>("old");
                String* newStr = scope->getArgument<String>("new");
                if (str && oldStr && newStr) {
                    vector<Value*> args = {str, oldStr, newStr};
                    return string_replace(args);
                }
                return nullptr;
            });
            
        functions.emplace_back("split", "split(string, delimiter)", 
            std::vector<std::string>{"string", "delimiter"},
            [](Scope* scope) -> Value* {
                String* str = scope->getArgument<String>("string");
                String* delimiter = scope->getArgument<String>("delimiter");
                if (str && delimiter) {
                    vector<Value*> args = {str, delimiter};
                    return string_split(args);
                }
                return nullptr;
            });
            
        functions.emplace_back("join", "join(array, separator)", 
            std::vector<std::string>{"array", "separator"},
            [](Scope* scope) -> Value* {
                Array* array = scope->getArgument<Array>("array");
                String* separator = scope->getArgument<String>("separator");
                if (array && separator) {
                    vector<Value*> args = {array, separator};
                    return string_join(args);
                }
                return nullptr;
            });
            
        functions.emplace_back("starts_with", "starts_with(string, prefix)", 
            std::vector<std::string>{"string", "prefix"},
            [](Scope* scope) -> Value* {
                String* str = scope->getArgument<String>("string");
                String* prefix = scope->getArgument<String>("prefix");
                if (str && prefix) {
                    vector<Value*> args = {str, prefix};
                    return string_starts_with(args);
                }
                return nullptr;
            });
            
        functions.emplace_back("ends_with", "ends_with(string, suffix)", 
            std::vector<std::string>{"string", "suffix"},
            [](Scope* scope) -> Value* {
                String* str = scope->getArgument<String>("string");
                String* suffix = scope->getArgument<String>("suffix");
                if (str && suffix) {
                    vector<Value*> args = {str, suffix};
                    return string_ends_with(args);
                }
                return nullptr;
            });
            
        functions.emplace_back("contains", "contains(string, substring)", 
            std::vector<std::string>{"string", "substring"},
            [](Scope* scope) -> Value* {
                String* str = scope->getArgument<String>("string");
                String* substring = scope->getArgument<String>("substring");
                if (str && substring) {
                    vector<Value*> args = {str, substring};
                    return string_contains(args);
                }
                return nullptr;
            });
            
        functions.emplace_back("format", "format(format, ...)", 
            std::vector<std::string>{"format"},
            [](Scope* scope) -> Value* {
                String* formatStr = scope->getArgument<String>("format");
                if (formatStr) {
                    vector<Value*> args = {formatStr};
                    if (scope->hasArgs()) {
                        vector<Value*> varArgs = scope->getArgs()->getElements();
                        args.insert(args.end(), varArgs.begin(), varArgs.end());
                    }
                    return string_format(args);
                }
                return nullptr;
            });
            
        return functions;
    }
};

// 导出插件
EXPORT_PLUGIN(StringPlugin)
