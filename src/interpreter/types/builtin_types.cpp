#include "interpreter/value.h"
#include "interpreter/builtin_type.h"
#include "interpreter/type_registry.h"
#include "interpreter/logger.h"
#include "lexer/token.h"
#include <algorithm>
#include <cmath>
#include <cctype>

using namespace std;

// ==================== 内置类型实现 ====================

// 整数类型实现
IntType::IntType() : NumericType("int", true, nullptr) {
    // 注册整数类型的内置方法
    registerMethod("to_string", [](Value* instance, vector<Value*>& args) -> Value* {
        if (Integer* intVal = dynamic_cast<Integer*>(instance)) {
            return new String(to_string(intVal->getValue()));
        }
        return nullptr;
    });
    
    registerMethod("abs", [](Value* instance, vector<Value*>& args) -> Value* {
        if (Integer* intVal = dynamic_cast<Integer*>(instance)) {
            return new Integer(abs(intVal->getValue()));
        }
        return nullptr;
    });
    
    registerMethod("to_double", [](Value* instance, vector<Value*>& args) -> Value* {
        if (Integer* intVal = dynamic_cast<Integer*>(instance)) {
            return new Double(static_cast<double>(intVal->getValue()));
        }
        return nullptr;
    });
    
    // new方法 - 创建新的Integer实例
    REGISTER_NEW_METHOD("int");
    
    // ==================== 运算符注册 ====================
    REGISTER_INTEGER_OPERATORS();
}

Value* IntType::createDefaultValue() {
    return new Integer(0);
}

// 浮点数类型实现
DoubleType::DoubleType() : NumericType("double", true, nullptr) {
    // 注册浮点数类型的内置方法
    registerMethod("to_string", [](Value* instance, vector<Value*>& args) -> Value* {
        if (Double* doubleVal = dynamic_cast<Double*>(instance)) {
            return new String(to_string(doubleVal->getValue()));
        }
        return nullptr;
    });
    
    registerMethod("to_int", [](Value* instance, vector<Value*>& args) -> Value* {
        if (Double* doubleVal = dynamic_cast<Double*>(instance)) {
            return new Integer(static_cast<int>(doubleVal->getValue()));
        }
        return nullptr;
    });
    
    registerMethod("floor", [](Value* instance, vector<Value*>& args) -> Value* {
        if (Double* doubleVal = dynamic_cast<Double*>(instance)) {
            return new Double(floor(doubleVal->getValue()));
        }
        return nullptr;
    });
    
    registerMethod("ceil", [](Value* instance, vector<Value*>& args) -> Value* {
        if (Double* doubleVal = dynamic_cast<Double*>(instance)) {
            return new Double(ceil(doubleVal->getValue()));
        }
        return nullptr;
    });
    
    registerMethod("round", [](Value* instance, vector<Value*>& args) -> Value* {
        if (Double* doubleVal = dynamic_cast<Double*>(instance)) {
            return new Double(round(doubleVal->getValue()));
        }
        return nullptr;
    });
    
    // new方法 - 创建新的Double实例
    REGISTER_NEW_METHOD("double");
    
    // ==================== 运算符注册 ====================
    REGISTER_FLOAT_OPERATORS();
}

Value* DoubleType::convertTo(ObjectType* targetType, Value* value) {
    if (!value) return nullptr;
    
    if (targetType == getTypeRegistry()->getType("int")) {
        if (Double* doubleVal = dynamic_cast<Double*>(value)) {
            return new Integer(static_cast<int>(doubleVal->getValue()));
        }
    } else if (targetType == getTypeRegistry()->getType("string")) {
        if (Double* doubleVal = dynamic_cast<Double*>(value)) {
            return new String(to_string(doubleVal->getValue()));
        }
    } else if (targetType == getTypeRegistry()->getType("bool")) {
        if (Double* doubleVal = dynamic_cast<Double*>(value)) {
            return new Bool(doubleVal->getValue() != 0.0);
        }
    }
    
    return nullptr;
}



Value* DoubleType::createDefaultValue() {
    return new Double(0.0);
}

// 字符类型实现
CharType::CharType() : NumericType("char", true, nullptr) {
    // 注册字符类型的内置方法
    registerMethod("to_string", [](Value* instance, vector<Value*>& args) -> Value* {
        if (Char* charVal = dynamic_cast<Char*>(instance)) {
            return new String(string(1, charVal->getValue()));
        }
        return nullptr;
    });
    
    registerMethod("to_int", [](Value* instance, vector<Value*>& args) -> Value* {
        if (Char* charVal = dynamic_cast<Char*>(instance)) {
            return new Integer(static_cast<int>(charVal->getValue()));
        }
        return nullptr;
    });
    
    registerMethod("is_digit", [](Value* instance, vector<Value*>& args) -> Value* {
        if (Char* charVal = dynamic_cast<Char*>(instance)) {
            return new Bool(isdigit(charVal->getValue()));
        }
        return nullptr;
    });
    
    registerMethod("is_alpha", [](Value* instance, vector<Value*>& args) -> Value* {
        if (Char* charVal = dynamic_cast<Char*>(instance)) {
            return new Bool(isalpha(charVal->getValue()));
        }
        return nullptr;
    });
    
    registerMethod("is_space", [](Value* instance, vector<Value*>& args) -> Value* {
        if (Char* charVal = dynamic_cast<Char*>(instance)) {
            return new Bool(isspace(charVal->getValue()));
        }
        return nullptr;
    });
    
    registerMethod("to_upper", [](Value* instance, vector<Value*>& args) -> Value* {
        if (Char* charVal = dynamic_cast<Char*>(instance)) {
            return new Char(toupper(charVal->getValue()));
        }
        return nullptr;
    });
    
    registerMethod("to_lower", [](Value* instance, vector<Value*>& args) -> Value* {
        if (Char* charVal = dynamic_cast<Char*>(instance)) {
            return new Char(tolower(charVal->getValue()));
        }
        return nullptr;
    });
    
    // ==================== 运算符注册 ====================
    REGISTER_CHAR_OPERATORS();
}

Value* CharType::convertTo(ObjectType* targetType, Value* value) {
    if (!value) return nullptr;
    
    if (targetType == getTypeRegistry()->getType("int")) {
        if (Char* charVal = dynamic_cast<Char*>(value)) {
            return new Integer(static_cast<int>(charVal->getValue()));
        }
    } else if (targetType == getTypeRegistry()->getType("string")) {
        if (Char* charVal = dynamic_cast<Char*>(value)) {
            return new String(string(1, charVal->getValue()));
        }
    }
    
    return nullptr;
}



Value* CharType::createDefaultValue() {
    return new Char('\0');
}

// 布尔类型实现
BoolType::BoolType() : PrimitiveType("bool", true, nullptr) {
    // 注册布尔类型的内置方法
    registerMethod("to_string", [](Value* instance, vector<Value*>& args) -> Value* {
        if (Bool* boolVal = dynamic_cast<Bool*>(instance)) {
            return new String(boolVal->getValue() ? "true" : "false");
        }
        return nullptr;
    });
    
    registerMethod("to_int", [](Value* instance, vector<Value*>& args) -> Value* {
        if (Bool* boolVal = dynamic_cast<Bool*>(instance)) {
            return new Integer(boolVal->getValue() ? 1 : 0);
        }
        return nullptr;
    });
    
    registerMethod("not", [](Value* instance, vector<Value*>& args) -> Value* {
        if (Bool* boolVal = dynamic_cast<Bool*>(instance)) {
            return new Bool(!boolVal->getValue());
        }
        return nullptr;
    });
    

    
    // ==================== 运算符注册 ====================
    REGISTER_BOOL_OPERATORS();
}

Value* BoolType::convertTo(ObjectType* targetType, Value* value) {
    if (!value) return nullptr;
    
    if (targetType == getTypeRegistry()->getType("int")) {
        if (Bool* boolVal = dynamic_cast<Bool*>(value)) {
            return new Integer(boolVal->getValue() ? 1 : 0);
        }
    } else if (targetType == getTypeRegistry()->getType("string")) {
        if (Bool* boolVal = dynamic_cast<Bool*>(value)) {
            return new String(boolVal->getValue() ? "true" : "false");
        }
    }
    
    return nullptr;
}



Value* BoolType::createDefaultValue() {
    return new Bool(false);
}

// 字符串类型实现 - 继承自ArrayType，因为字符串本质上是字符数组
StringType::StringType() : ArrayType() {
    // 注册字符串类型的内置方法
    registerMethod("length", [](Value* instance, vector<Value*>& args) -> Value* {
        if (String* strVal = dynamic_cast<String*>(instance)) {
            return new Integer(static_cast<int>(strVal->getValue().length()));
        }
        return nullptr;
    });
    
    registerMethod("substring", [](Value* instance, vector<Value*>& args) -> Value* {
        if (String* strVal = dynamic_cast<String*>(instance)) {
            if (args.size() >= 2) {
                if (Integer* start = dynamic_cast<Integer*>(args[0])) {
                    if (Integer* end = dynamic_cast<Integer*>(args[1])) {
                        string str = strVal->getValue();
                        int startPos = start->getValue();
                        int endPos = end->getValue();
                        
                        if (startPos >= 0 && endPos <= str.length() && startPos <= endPos) {
                            return new String(str.substr(startPos, endPos - startPos));
                        }
                    }
                }
            }
        }
        return nullptr;
    });
    
    registerMethod("to_upper", [](Value* instance, vector<Value*>& args) -> Value* {
        if (String* strVal = dynamic_cast<String*>(instance)) {
            string str = strVal->getValue();
            transform(str.begin(), str.end(), str.begin(), ::toupper);
            return new String(str);
        }
        return nullptr;
    });
    
    registerMethod("to_lower", [](Value* instance, vector<Value*>& args) -> Value* {
        if (String* strVal = dynamic_cast<String*>(instance)) {
            string str = strVal->getValue();
            transform(str.begin(), str.end(), str.begin(), ::tolower);
            return new String(str);
        }
        return nullptr;
    });
    
    registerMethod("split", [](Value* instance, vector<Value*>& args) -> Value* {
        if (String* strVal = dynamic_cast<String*>(instance)) {
            if (args.size() >= 1) {
                if (String* delimiter = dynamic_cast<String*>(args[0])) {
                    string str = strVal->getValue();
                    string delim = delimiter->getValue();
                    
                    Array* result = new Array();
                    size_t pos = 0;
                    string token;
                    
                    while ((pos = str.find(delim)) != string::npos) {
                        token = str.substr(0, pos);
                        result->addElement(new String(token));
                        str.erase(0, pos + delim.length());
                    }
                    
                    if (!str.empty()) {
                        result->addElement(new String(str));
                    }
                    
                    return result;
                }
            }
        }
        return nullptr;
    });
    
    // to_string方法 - 返回字符串本身
    registerMethod("to_string", [](Value* instance, vector<Value*>& args) -> Value* {
        if (String* strVal = dynamic_cast<String*>(instance)) {
            return new String(strVal->getValue());
        }
        return nullptr;
    });
    

    
    // new方法 - 创建新的String实例
    REGISTER_NEW_METHOD("string");
    
    // ==================== 运算符注册 ====================
    // 字符串继承自数组，所以自动获得数组的运算符支持
    // 同时注册字符串特有的运算符
    REGISTER_STRING_OPERATORS();
}

Value* StringType::convertTo(ObjectType* targetType, Value* value) {
    if (!value) return nullptr;
    
    // 字符串类型特有的转换逻辑
    if (targetType == getTypeRegistry()->getType("string")) {
        if (String* strVal = dynamic_cast<String*>(value)) {
            return new String(strVal->getValue());
        }
    } else if (targetType == getTypeRegistry()->getType("int")) {
        if (String* strVal = dynamic_cast<String*>(value)) {
            try {
                return new Integer(stoi(strVal->getValue()));
            } catch (...) {
                return nullptr;
            }
        }
    } else if (targetType == getTypeRegistry()->getType("double")) {
        if (String* strVal = dynamic_cast<String*>(value)) {
            try {
                return new Double(stod(strVal->getValue()));
            } catch (...) {
                return nullptr;
            }
        }
    } else if (targetType == getTypeRegistry()->getType("bool")) {
        if (String* strVal = dynamic_cast<String*>(value)) {
            string str = strVal->getValue();
            transform(str.begin(), str.end(), str.begin(), ::tolower);
            return new Bool(str == "true" || str == "1");
        }
    } else if (targetType == getTypeRegistry()->getType("char")) {
        if (String* strVal = dynamic_cast<String*>(value)) {
            if (strVal->getValue().length() == 1) {
                return new Char(strVal->getValue()[0]);
            }
        }
    }
    
    return nullptr;
}

bool StringType::isCompatibleWith(ObjectType* other) const {
    if (!other) return false;
    
    // 字符串与数组类型兼容（因为字符串是字符数组）
    if (other->getTypeName() == "array") {
        return true;
    }
    
    return typeName == other->getTypeName();
}

Value* StringType::createDefaultValue() {
    String* str = new String("");
    // 设置类型信息
    str->setValueType(this);
    return str;
}

Value* StringType::convertToString(Value* value) {
    if (String* strVal = dynamic_cast<String*>(value)) {
        return new String(strVal->getValue());
    }
    return nullptr;
}

Value* StringType::convertToBool(Value* value) {
    if (String* strVal = dynamic_cast<String*>(value)) {
        return new Bool(!strVal->getValue().empty());
    }
    return nullptr;
}

// 数组类型实现
ArrayType::ArrayType() : ContainerType("array", true, nullptr) {
    // 注册数组类型的内置方法
    registerMethod("length", [](Value* instance, vector<Value*>& args) -> Value* {
        if (Array* arrVal = dynamic_cast<Array*>(instance)) {
            return new Integer(static_cast<int>(arrVal->getElements().size()));
        }
        return nullptr;
    });
    
    registerMethod("add", [](Value* instance, vector<Value*>& args) -> Value* {
        if (Array* arrVal = dynamic_cast<Array*>(instance)) {
            if (args.size() >= 1) {
                arrVal->addElement(args[0]);
            }
        }
        return nullptr;
    });
    
    // push方法 - 向数组末尾添加元素
    registerMethod("push", [](Value* instance, vector<Value*>& args) -> Value* {
        if (Array* arrVal = dynamic_cast<Array*>(instance)) {
            if (args.size() >= 1) {
                arrVal->addElement(args[0]);
                return new Integer(static_cast<int>(arrVal->getElements().size()));
            }
        }
        return nullptr;
    });
    
    // pop方法 - 从数组末尾移除并返回元素
    registerMethod("pop", [](Value* instance, vector<Value*>& args) -> Value* {
        if (Array* arrVal = dynamic_cast<Array*>(instance)) {
            const vector<Value*>& elements = arrVal->getElements();
            if (!elements.empty()) {
                Value* lastElement = elements.back();
                arrVal->removeElement(elements.size() - 1);
                return lastElement ? lastElement->clone() : nullptr;
            }
        }
        return nullptr;
    });
    
    registerMethod("remove", [](Value* instance, vector<Value*>& args) -> Value* {
        if (Array* arrVal = dynamic_cast<Array*>(instance)) {
            if (args.size() >= 1) {
                if (Integer* index = dynamic_cast<Integer*>(args[0])) {
                    arrVal->removeElement(index->getValue());
                }
            }
        }
        return nullptr;
    });
    
    registerMethod("clear", [](Value* instance, vector<Value*>& args) -> Value* {
        if (Array* arrVal = dynamic_cast<Array*>(instance)) {
            arrVal->clear();
        }
        return nullptr;
    });
    
    // new方法 - 创建新的Array实例
    REGISTER_NEW_METHOD("array");
    
    // 下标访问运算符重载 ([] 运算符)
    REGISTER_SUBSCRIPT_OPERATOR();
    
    // 成员访问运算符重载 (. 运算符)
    REGISTER_MEMBER_ACCESS_OPERATOR();
    

    
    // to_string方法 - 将数组转换为字符串
    registerMethod("to_string", [](Value* instance, vector<Value*>& args) -> Value* {
        if (Array* arrVal = dynamic_cast<Array*>(instance)) {
            string result = "[";
            const vector<Value*>& elements = arrVal->getElements();
            for (size_t i = 0; i < elements.size(); ++i) {
                if (i > 0) result += ", ";
                if (elements[i]) {
                    result += elements[i]->toString();
                } else {
                    result += "null";
                }
            }
            result += "]";
            return new String(result);
        }
        return nullptr;
    });
    

    

    
    // ==================== 运算符注册 ====================
    REGISTER_ARRAY_OPERATORS();
}



Value* ArrayType::createDefaultValue() {
    return new Array();
}

// 字典类型实现
DictType::DictType() : ContainerType("dict", true, nullptr) {
    // 注册字典类型的内置方法
    registerMethod("size", [](Value* instance, vector<Value*>& args) -> Value* {
        if (Dict* dictVal = dynamic_cast<Dict*>(instance)) {
            return new Integer(static_cast<int>(dictVal->getEntries().size()));
        }
        return nullptr;
    });
    
    registerMethod("keys", [](Value* instance, vector<Value*>& args) -> Value* {
        if (Dict* dictVal = dynamic_cast<Dict*>(instance)) {
            Array* keys = new Array();
            for (const auto& entry : dictVal->getEntries()) {
                keys->addElement(new String(entry.first));
            }
            return keys;
        }
        return nullptr;
    });
    
    registerMethod("values", [](Value* instance, vector<Value*>& args) -> Value* {
        if (Dict* dictVal = dynamic_cast<Dict*>(instance)) {
            Array* values = new Array();
            for (const auto& entry : dictVal->getEntries()) {
                values->addElement(entry.second);
            }
            return values;
        }
        return nullptr;
    });
    
    registerMethod("clear", [](Value* instance, vector<Value*>& args) -> Value* {
        if (Dict* dictVal = dynamic_cast<Dict*>(instance)) {
            dictVal->clear();
        }
        return nullptr;
    });
    
    // has方法 - 检查字典是否包含指定键
    registerMethod("has", [](Value* instance, vector<Value*>& args) -> Value* {
        if (Dict* dictVal = dynamic_cast<Dict*>(instance)) {
            if (args.size() >= 1) {
                if (String* key = dynamic_cast<String*>(args[0])) {
                    return new Bool(dictVal->getEntry(key->getValue()) != nullptr);
                }
            }
        }
        return nullptr;
    });
    
    // has_key方法 - 别名，与has相同
    registerMethod("has_key", [](Value* instance, vector<Value*>& args) -> Value* {
        if (Dict* dictVal = dynamic_cast<Dict*>(instance)) {
            if (args.size() >= 1) {
                if (String* key = dynamic_cast<String*>(args[0])) {
                    return new Bool(dictVal->getEntry(key->getValue()) != nullptr);
                }
            }
        }
        return nullptr;
    });
    
    // get方法 - 获取字典中指定键的值
    registerMethod("get", [](Value* instance, vector<Value*>& args) -> Value* {
        if (Dict* dictVal = dynamic_cast<Dict*>(instance)) {
            if (args.size() >= 1) {
                if (String* key = dynamic_cast<String*>(args[0])) {
                    Value* value = dictVal->getEntry(key->getValue());
                    return value ? value->clone() : nullptr;
                }
            }
        }
        return nullptr;
    });
    
    // new方法 - 创建新的Dict实例
    REGISTER_NEW_METHOD("dict");
    
    // 成员访问运算符重载 (. 运算符)
    REGISTER_MEMBER_ACCESS_OPERATOR();
    
    // 下标访问运算符重载 ([] 运算符)
    REGISTER_SUBSCRIPT_OPERATOR();
    
    // to_string方法 - 将字典转换为字符串
    registerMethod("to_string", [](Value* instance, vector<Value*>& args) -> Value* {
        if (Dict* dictVal = dynamic_cast<Dict*>(instance)) {
            string result = "{";
            const unordered_map<string, Value*>& entries = dictVal->getEntries();
            bool first = true;
            for (const auto& entry : entries) {
                if (!first) result += ", ";
                result += "\"" + entry.first + "\": ";
                if (entry.second) {
                    result += entry.second->toString();
                } else {
                    result += "null";
                }
                first = false;
            }
            result += "}";
            return new String(result);
        }
        return nullptr;
    });
    
    // convertTo方法 - 类型转换
    registerMethod("convertTo", [](Value* instance, vector<Value*>& args) -> Value* {
        if (Dict* dictVal = dynamic_cast<Dict*>(instance)) {
            if (args.size() >= 1 && args[0]) {
                // 获取目标类型名称
                string targetTypeName = args[0]->toString();
                
                // 直接实现转换逻辑
                if (targetTypeName == "string") {
                    string result = "{";
                    const unordered_map<string, Value*>& entries = dictVal->getEntries();
                    bool first = true;
                    for (const auto& entry : entries) {
                        if (!first) result += ", ";
                        result += "\"" + entry.first + "\": ";
                        if (entry.second) {
                            result += entry.second->toString();
                        } else {
                            result += "null";
                        }
                        first = false;
                    }
                    result += "}";
                    return new String(result);
                } else if (targetTypeName == "bool") {
                    return new Bool(!dictVal->getEntries().empty());
                }
            }
        }
        return nullptr;
    });
    
    // new方法 - 创建新的Dict实例
    REGISTER_DICT_NEW_METHOD();
    
    // ==================== 运算符注册 ====================
    REGISTER_DICT_OPERATORS();
}



Value* DictType::createDefaultValue() {
    return new Dict();
}

// ==================== NullType实现 ====================

// 空值类型实现
NullType::NullType() : ObjectType("null", true, false, false, false, false, nullptr) {
    // 注册null类型的toString方法
    registerMethod("toString", [](Value* instance, vector<Value*>& args) -> Value* {
        return new String("null");
    });
    
    // convertTo方法 - 类型转换
    registerMethod("convertTo", [](Value* instance, vector<Value*>& args) -> Value* {
        if (args.size() >= 1 && args[0]) {
            // 获取目标类型名称
            string targetTypeName = args[0]->toString();
            
            // 直接实现转换逻辑
            if (targetTypeName == "string") {
                return new String("null");
            } else if (targetTypeName == "bool") {
                return new Bool(false);
            } else if (targetTypeName == "int") {
                return new Integer(0);
            } else if (targetTypeName == "double") {
                return new Double(0.0);
            } else if (targetTypeName == "char") {
                return new Char('\0');
            } else if (targetTypeName == "array") {
                return new Array();
            } else if (targetTypeName == "dict") {
                return new Dict();
            }
        }
        return nullptr;
    });
    
    // assign方法 - 赋值操作（null类型不支持赋值）
    registerMethod("assign", [](Value* instance, vector<Value*>& args) -> Value* {
        // null类型不支持赋值，返回null
        return new Null();
    });

    // new方法 - 创建新的Null实例
    REGISTER_NEW_METHOD(Null);
}

bool NullType::isCompatibleWith(ObjectType* other) const {
    // null类型与所有类型都兼容
    return true;
}

Value* NullType::createDefaultValue() {
    return new Null();
}

// ==================== 自动注册内置类型 ====================
// 在main()之前自动注册所有内置类型

// 注册基本类型
// REGISTER_BUILTIN_TYPE(int, IntType)
REGISTER_BUILTIN_TYPE(double, DoubleType)
REGISTER_BUILTIN_TYPE(char, CharType)
REGISTER_BUILTIN_TYPE(bool, BoolType)
REGISTER_BUILTIN_TYPE(null, NullType)

// 注册复合类型
// REGISTER_BUILTIN_TYPE(string, StringType)
REGISTER_BUILTIN_TYPE(array, ArrayType)
REGISTER_BUILTIN_TYPE(dict, DictType)

// 注册带继承关系的类型
// StringType继承自ArrayType（因为字符串本质上是字符数组）
REGISTER_BUILTIN_TYPE_WITH_PARENT(string, StringType, array, ArrayType)

// IntType继承自DoubleType（数值类型的层次结构）
REGISTER_BUILTIN_TYPE_WITH_PARENT(int, IntType, double, DoubleType)
