#include <algorithm>
#include <cmath>
#include <cctype>
#include <vector>
#include <string>
#include <cstdio>

#include "interpreter/values/value.h"
#include "interpreter/types/types.h"
#include "common/logger.h"
#include "interpreter/scope/scope.h"
#include "lexer/token.h"

using namespace std;

// 辅助函数：应用格式化规范
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

// 定义类方法数组
static const vector<Function*> stringClassMethods = {
    // 注册字符串类型的基本方法
    new BuiltinFunction([](class Scope* scope) -> Value* {
        if (String* str = scope->getThis<String>()) {
            return new String(str->getValue());
        }
        return nullptr;
    }, "toString()"),
    
    new BuiltinFunction([](class Scope* scope) -> Value* {
        if (String* str = scope->getThis<String>()) {
            return new Bool(str->toBool());
        }
        return nullptr;
    }, "toBool()"),
    
    new BuiltinFunction([](class Scope* scope) -> Value* {
        if (String* str = scope->getThis<String>()) {
            try {
                int value = std::stoi(str->getValue());
                return new Integer(value);
            } catch (const std::exception&) {
                return new Integer(0); // 转换失败返回0
            }
        }
        return nullptr;
    }, "toInt()"),
    
    new BuiltinFunction([](class Scope* scope) -> Value* {
        if (String* str = scope->getThis<String>()) {
            try {
                double value = std::stod(str->getValue());
                return new Double(value);
            } catch (const std::exception&) {
                return new Double(0.0); // 转换失败返回0.0
            }
        }
        return nullptr;
    }, "toDouble()"),
    
    new BuiltinFunction([](class Scope* scope) -> Value* {
        if (String* str = scope->getThis<String>()) {
            const std::string& value = str->getValue();
            if (value.empty()) {
                return new Char('\0');
            }
            return new Char(value[0]);
        }
        return nullptr;
    }, "toChar()"),
    
    new BuiltinFunction([](class Scope* scope) -> Value* {
        // 从作用域获取实例，而不是使用参数
        LOG_DEBUG("StringType::length() called");
        if (String* str = scope->getThis<String>()) {
            LOG_DEBUG("StringType::length() called: " + to_string(str->length()));
            return new Integer(str->length());
        }
        LOG_DEBUG("StringType::length() called: nullptr");
        return nullptr;
    }, "length()"),
    
    // 字符串常用操作 - 使用新式接口
    new BuiltinFunction([](class Scope* scope) -> Value* {
        // 使用简洁的getThis方法获取实例
        if (String* str = scope->getThis<String>()) {
            // 使用语义化的getArgument方法获取参数
            if (Integer* start = scope->getArgument<Integer>("start")) {
                if (Integer* end = scope->getArgument<Integer>("end")) {
                    return str->substring(start->getValue(), end->getValue());
                }
            }
        }
        return nullptr;
    }, "substring(start, end)"),
    
    new BuiltinFunction([](class Scope* scope) -> Value* {
        // 从作用域获取实例，而不是使用参数
        if (String* str = scope->getThis<String>()) {
            // 使用语义化的getArgument方法获取参数
            return str->trim();
        }
        return nullptr;
    }, "trim()"),
    
    new BuiltinFunction([](class Scope* scope) -> Value* {
        // 从作用域获取实例，而不是使用参数
        String* str = scope->getThis<String>();
        if (str) {
            // 使用泛型getVariable方法，直接获取指定类型的参数
            String* oldStr = scope->getArgument<String>("oldStr");
            String* newStr = scope->getArgument<String>("newStr");
            
            if (oldStr && newStr) {
                return str->replace(*oldStr, *newStr);
            }
        }
        return nullptr;
    }, "replace(oldStr, newStr)"),
    
    new BuiltinFunction([](class Scope* scope) -> Value* {
        // 从作用域获取实例，而不是使用参数
        if (String* str = scope->getThis<String>()) {
            if (String* delimiter = scope->getArgument<String>("delimiter")) {
                vector<String*> splitResult = str->split(*delimiter);
                Array* result = new Array();
                for (String* s : splitResult) {
                    result->addElement(s);
                }
                return result;
            }
        }
        return nullptr;
    }, "split(delimiter)"),
    
    new BuiltinFunction([](class Scope* scope) -> Value* {
        // 使用简洁的getThis方法获取实例
        if (String* str = scope->getThis<String>()) {
            return str->trim();
        }
        return nullptr;
    }, "trim()"),
    
    new BuiltinFunction([](class Scope* scope) -> Value* {
        // 使用简洁的getThis方法获取实例
        if (String* str = scope->getThis<String>()) {
            return str->toUpperCase();
        }
        return nullptr;
    }, "toUpper()"),
    
    new BuiltinFunction([](class Scope* scope) -> Value* {
        // 使用简洁的getThis方法获取实例
        if (String* str = scope->getThis<String>()) {
            return str->toLowerCase();
        }
        return nullptr;
    }, "toLower()"),
    
    new BuiltinFunction([](class Scope* scope) -> Value* {
        if (String* str = scope->getThis<String>()) {
            if (String* prefix = scope->getArgument<String>("prefix")) {
                return new Bool(str->startsWith(*prefix));
            }
        }
        return new Bool(false);
    }, "startsWith(prefix)"),
    
    new BuiltinFunction([](class Scope* scope) -> Value* {
        if (String* str = scope->getThis<String>()) {
            if (String* suffix = scope->getArgument<String>("suffix")) {
                if (suffix) {
                    return new Bool(str->endsWith(*suffix));
                }
            }
        }
        return new Bool(false);
    }, "endsWith(suffix)"),
    
    new BuiltinFunction([](class Scope* scope) -> Value* {
        if (String* str = scope->getThis<String>()) {
            if (String* substr = scope->getArgument<String>("substring")) {
                return new Bool(str->contains(*substr));
            }
        }
        return new Bool(false);
    }, "contains(substring)"),
    
    new BuiltinFunction([](class Scope* scope) -> Value* {
        if (String* str = scope->getThis<String>()) {
            return new Bool(str->isEmpty());
        }
        return new Bool(false);
    }, "isEmpty()"),
    
    new BuiltinFunction([](class Scope* scope) -> Value* {
        if (String* str = scope->getThis<String>()) {
            if (String* substr = scope->getArgument<String>("substring")) {
                return new Integer(str->indexOf(*substr));
            }
        }
        return new Integer(-1);
    }, "indexOf(substring)"),

    new BuiltinFunction([](class Scope* scope) -> Value* {
        if (String* str = scope->getThis<String>()) {
            if (String* substr = scope->getArgument<String>("substring")) {
                // 将参数转换为String对象
                return new Integer(str->lastIndexOf(*substr));
            }
        }
        return new Integer(-1);
    }, "lastIndexOf(substring)"),
    
    new BuiltinFunction([](class Scope* scope) -> Value* {
        if (String* str = scope->getThis<String>()) {
            String* other = scope->getArgument<String>("other");
            if (other) {
                str->append(*other);
                return str;
            }
        }
        return nullptr;
    }, "append(other)"),
};

// 定义静态方法数组
static const vector<Function*> stringStaticMethods = {
    // ==================== 注册静态方法 ====================
    // 构造函数（创建字符串）
    new BuiltinFunction([](class Scope* scope) -> Value* {
        if (String* strValue = scope->getArgument<String>("value")) {
            return strValue->clone();  // 从其他类型转换
        }
        return new String("");
    }, "string(...)"),
  
    // 从整数创建字符串
    new BuiltinFunction([](class Scope* scope) -> Value* {
        if (Integer* intValue = scope->getArgument<Integer>("value")) {
            return new String(to_string(intValue->getValue()));  // 从其他类型转换
        }
        return nullptr;
    }, "fromInt(value)"),
    
    // 从浮点数创建字符串
    new BuiltinFunction([](class Scope* scope) -> Value* {
        if (Double* doubleValue = scope->getArgument<Double>("value")) {
            return new String(to_string(doubleValue->getValue()));
        }
        return new String("");
    }, "fromDouble(value)"),
    
    // 从布尔值创建字符串
    new BuiltinFunction([](class Scope* scope) -> Value* {
        if (Bool* boolValue = scope->getArgument<Bool>("value")) {
            return new String(boolValue->getValue() ? "true" : "false");
        }
        return new String("");
    }, "fromBool(value)"),
    
    // 创建重复字符串
    new BuiltinFunction([](class Scope* scope) -> Value* {
        if (Integer* count = scope->getArgument<Integer>("count")) {
            if (String* strValue = scope->getArgument<String>("value")) {
                string result;
                for (int i = 0; i < count->getValue(); i++) {
                    result += strValue->getValue();
                }
                return new String(result);
            }
        }
        return new String("");
    }, "repeat(count, value)"),
    
    // 可变参数示例：字符串连接函数
    new BuiltinFunction([](class Scope* scope) -> Value* {
        string result;
        
        // 检查是否有可变参数
        if (scope->hasArgs()) {
            // 获取所有可变参数并连接
            vector<Value*> varArgs = scope->getArgs()->getElements();
            for (Value* arg : varArgs) {
                if (arg) {
                    result += arg->toString();
                }
            }
        }
        
        return new String(result);
    }, "concat(...)"),
    
    // Python风格的格式化函数（固定参数 + 可变参数）
    new BuiltinFunction([](class Scope* scope) -> Value* {
        // 获取固定参数：格式字符串
        String* formatStr = scope->getArgument<String>("format");
        if (!formatStr) {
            return new String("Error: format string required");
        }
        
        string result = formatStr->getValue();
        
        // 获取可变参数
        vector<Value*> varArgs;
        if (scope->hasArgs()) {
            varArgs = scope->getArgs()->getElements();
        }
        
        // 获取关键字参数
        Dict* kwargs = scope->getKwargs();
        
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
                    // {0}, {1} - 位置参数
                    if (!fieldName.empty() && isdigit(fieldName[0])) {
                        int index = stoi(fieldName);
                        if (index >= 0 && index < (int)varArgs.size()) {
                            value = varArgs[index];
                        }
                    } else {
                        // {} - 下一个位置参数
                        if (!varArgs.empty()) {
                            value = varArgs[0];
                            varArgs.erase(varArgs.begin());
                        }
                    }
                } else if (isdigit(fieldName[0])) {
                    // 位置参数 {0}, {1}, {2}...
                    int index = stoi(fieldName);
                    if (index >= 0 && index < (int)varArgs.size()) {
                        value = varArgs[index];
                    }
                } else if (kwargs) {
                    // 关键字参数 {name}
                    value = kwargs->getEntry(fieldName);
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
    }, "format(format, ...)"),
    
    // 默认值示例：带默认分隔符的字符串分割函数
    new BuiltinFunction([](class Scope* scope) -> Value* {
        // 获取字符串参数
        String* str = scope->getArgument<String>("text");
        if (!str) {
            return new String("Error: text parameter required");
        }
        
        // 获取分隔符参数（有默认值）
        String* delimiter = scope->getArgument<String>("delimiter");
        if (!delimiter) {
            // 如果没有提供分隔符，使用默认值
            delimiter = new String(" ");
        }
        
        // 执行分割
        vector<String*> splitResult = str->split(*delimiter);
        Array* result = new Array();
        for (String* s : splitResult) {
            result->addElement(s);
        }
        
        return result;
    }, "splitWithDefault(text, delimiter=,)"),  // C风格：使用 "=" 表示默认值
    
    // 复杂默认值示例：带多个默认值的字符串处理函数
    new BuiltinFunction([](class Scope* scope) -> Value* {
        // 获取必需参数
        String* str = scope->getArgument<String>("text");
        if (!str) {
            return new String("Error: text parameter required");
        }
        
        // 获取可选参数（带默认值）
        String* prefix = scope->getArgument<String>("prefix");
        if (!prefix) {
            prefix = new String(">> ");  // 默认前缀
        }
        
        String* suffix = scope->getArgument<String>("suffix");
        if (!suffix) {
            suffix = new String(" <<");  // 默认后缀
        }
        
        Integer* repeat = scope->getArgument<Integer>("repeat");
        if (!repeat) {
            repeat = new Integer(1);     // 默认重复次数
        }
        
        // 处理字符串
        string result = prefix->getValue();
        for (int i = 0; i < repeat->getValue(); ++i) {
            if (i > 0) result += " ";
            result += str->getValue();
        }
        result += suffix->getValue();
        
        return new String(result);
    }, "processString(text, prefix=>>, suffix=<<, repeat=1)"),  // 多个默认值
};

// StringType构造函数
StringType::StringType() : ClassType("string", false, true, false) {
    // 批量注册静态方法
    for (Function* method : stringStaticMethods) {
        // 将 BuiltinFunction 转换为 Function* 类型
        addStaticMethod(method);
    }

    // 批量注册类方法
    for (Function* method : stringClassMethods) {
        // 将 BuiltinFunction 转换为 Function* 类型
        addUserMethod(method);
    }
}
