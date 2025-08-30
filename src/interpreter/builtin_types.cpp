
#include "interpreter/value.h"
#include "interpreter/builtin_type.h"
#include "interpreter/type_registry.h"
#include "interpreter/logger.h"
#include "lexer/token.h"
#include <algorithm>
#include <cmath>
#include <cctype>

using namespace std;

// 声明外部factory变量（用于Char类型的方法）
// extern TokenFlyweight* factory; // 已移除，避免lexer依赖

// ==================== 内置类型实现 ====================

// 整数类型实现
IntType::IntType() : ObjectType("int", true, true, false, false, false, nullptr) {
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
    
    // 添加数值运算接口方法
    registerMethod("add", [](Value* instance, vector<Value*>& args) -> Value* {
        if (Integer* intVal = dynamic_cast<Integer*>(instance)) {
            if (args.size() >= 1) {
                if (Integer* other = dynamic_cast<Integer*>(args[0])) {
                    return new Integer(intVal->getValue() + other->getValue());
                }
            }
        }
        return nullptr;
    });
    
    registerMethod("subtract", [](Value* instance, vector<Value*>& args) -> Value* {
        if (Integer* intVal = dynamic_cast<Integer*>(instance)) {
            if (args.size() >= 1) {
                if (Integer* other = dynamic_cast<Integer*>(args[0])) {
                    return new Integer(intVal->getValue() - other->getValue());
                }
            }
        }
        return nullptr;
    });
    
    registerMethod("multiply", [](Value* instance, vector<Value*>& args) -> Value* {
        if (Integer* intVal = dynamic_cast<Integer*>(instance)) {
            if (args.size() >= 1) {
                if (Integer* other = dynamic_cast<Integer*>(args[0])) {
                    return new Integer(intVal->getValue() * other->getValue());
                }
            }
        }
        return nullptr;
    });
    
    registerMethod("divide", [](Value* instance, vector<Value*>& args) -> Value* {
        if (Integer* intVal = dynamic_cast<Integer*>(instance)) {
            if (args.size() >= 1) {
                if (Integer* other = dynamic_cast<Integer*>(args[0])) {
                    if (other->getValue() != 0) {
                        return new Integer(intVal->getValue() / other->getValue());
                    }
                }
            }
        }
        return nullptr;
    });
}

Value* IntType::convertTo(ObjectType* targetType, Value* value) {
    if (!value) return nullptr;
    
    if (targetType == getTypeRegistry()->getType("double")) {
        if (Integer* intVal = dynamic_cast<Integer*>(value)) {
            return new Double(static_cast<double>(intVal->getValue()));
        }
    } else if (targetType == getTypeRegistry()->getType("string")) {
        if (Integer* intVal = dynamic_cast<Integer*>(value)) {
            return new String(to_string(intVal->getValue()));
        }
    } else if (targetType == getTypeRegistry()->getType("bool")) {
        if (Integer* intVal = dynamic_cast<Integer*>(value)) {
            return new Bool(intVal->getValue() != 0);
        }
    }
    
    return nullptr;
}

bool IntType::isCompatibleWith(ObjectType* other) const {
    if (!other) return false;
    return typeName == other->getTypeName() || 
           other->getTypeName() == "double";  // int可以转换为double
}

Value* IntType::createDefaultValue() {
    return new Integer(0);
}

// 浮点数类型实现
DoubleType::DoubleType() : ObjectType("double", true, true, false, false, false, nullptr) {
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

bool DoubleType::isCompatibleWith(ObjectType* other) const {
    if (!other) return false;
    return typeName == other->getTypeName();
}

Value* DoubleType::createDefaultValue() {
    return new Double(0.0);
}

// 字符类型实现
CharType::CharType() : ObjectType("char", true, true, false, false, false, nullptr) {
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

bool CharType::isCompatibleWith(ObjectType* other) const {
    if (!other) return false;
    return typeName == other->getTypeName();
}

Value* CharType::createDefaultValue() {
    return new Char('\0');
}

// 布尔类型实现
BoolType::BoolType() : ObjectType("bool", true, true, false, false, false, nullptr) {
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

bool BoolType::isCompatibleWith(ObjectType* other) const {
    if (!other) return false;
    return typeName == other->getTypeName();
}

Value* BoolType::createDefaultValue() {
    return new Bool(false);
}

// 字符串类型实现
StringType::StringType() : ObjectType("string", false, true, false, true, false, nullptr) {
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
    
    // 访问方法 - 通过索引访问字符串字符
    registerMethod("access", [](Value* instance, vector<Value*>& args) -> Value* {
        if (String* strVal = dynamic_cast<String*>(instance)) {
            if (args.size() >= 1) {
                if (Integer* index = dynamic_cast<Integer*>(args[0])) {
                    int idx = index->getValue();
                    string str = strVal->getValue();
                    if (idx >= 0 && idx < (int)str.length()) {
                        return new Char(str[idx]);
                    } else {
                        throw runtime_error("String index out of bounds: " + to_string(idx));
                    }
                } else {
                    throw runtime_error("String index must be an integer");
                }
            }
        }
        return nullptr;
    });
}

Value* StringType::convertTo(ObjectType* targetType, Value* value) {
    if (!value) return nullptr;
    
    if (targetType == getTypeRegistry()->getType("int")) {
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
    return typeName == other->getTypeName();
}

Value* StringType::createDefaultValue() {
    return new String("");
}

// 数组类型实现
ArrayType::ArrayType() : ObjectType("array", false, true, false, true, false, nullptr) {
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
    
    // 实例化方法
    registerMethod("create", [](Value* instance, vector<Value*>& args) -> Value* {
        Array* newArray = new Array();
        for (Value* arg : args) {
            newArray->addElement(arg);
        }
        return newArray;
    });
    
    registerMethod("new", [](Value* instance, vector<Value*>& args) -> Value* {
        Array* newArray = new Array();
        for (Value* arg : args) {
            newArray->addElement(arg);
        }
        return newArray;
    });
    
    // 访问方法 - 通过索引访问数组元素
    registerMethod("access", [](Value* instance, vector<Value*>& args) -> Value* {
        if (Array* arrVal = dynamic_cast<Array*>(instance)) {
            if (args.size() >= 1) {
                if (Integer* index = dynamic_cast<Integer*>(args[0])) {
                    int idx = index->getValue();
                    if (idx >= 0 && idx < (int)arrVal->getElements().size()) {
                        return arrVal->getElement(idx);
                    } else {
                        throw runtime_error("Array index out of bounds: " + to_string(idx));
                    }
                } else {
                    throw runtime_error("Array index must be an integer");
                }
            }
        }
        return nullptr;
    });
}

Value* ArrayType::convertTo(ObjectType* targetType, Value* value) {
    if (!value) return nullptr;
    
    if (targetType == getTypeRegistry()->getType("string")) {
        if (Array* arrVal = dynamic_cast<Array*>(value)) {
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
    } else if (targetType == getTypeRegistry()->getType("bool")) {
        if (Array* arrVal = dynamic_cast<Array*>(value)) {
            return new Bool(!arrVal->getElements().empty());
        }
    }
    
    return nullptr;
}

bool ArrayType::isCompatibleWith(ObjectType* other) const {
    if (!other) return false;
    return typeName == other->getTypeName();
}

Value* ArrayType::createDefaultValue() {
    return new Array();
}

// 字典类型实现
DictType::DictType() : ObjectType("dict", false, true, false, true, false, nullptr) {
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
    
    // 实例化方法
    registerMethod("create", [](Value* instance, vector<Value*>& args) -> Value* {
        Dict* newDict = new Dict();
        // 处理键值对参数
        for (size_t i = 0; i < args.size(); i += 2) {
            if (i + 1 < args.size()) {
                if (String* key = dynamic_cast<String*>(args[i])) {
                    newDict->setEntry(key->getValue(), args[i + 1]);
                }
            }
        }
        return newDict;
    });
    
    registerMethod("new", [](Value* instance, vector<Value*>& args) -> Value* {
        Dict* newDict = new Dict();
        // 处理键值对参数
        for (size_t i = 0; i < args.size(); i += 2) {
            if (i + 1 < args.size()) {
                if (String* key = dynamic_cast<String*>(args[i])) {
                    newDict->setEntry(key->getValue(), args[i + 1]);
                }
            }
        }
        return newDict;
    });
    
    // 访问方法 - 通过键访问字典元素
    registerMethod("access", [](Value* instance, vector<Value*>& args) -> Value* {
        if (Dict* dictVal = dynamic_cast<Dict*>(instance)) {
            if (args.size() >= 1) {
                if (String* key = dynamic_cast<String*>(args[0])) {
                    Value* value = dictVal->getEntry(key->getValue());
                    if (value) {
                        return value;
                    } else {
                        throw runtime_error("Key '" + key->getValue() + "' not found in dictionary");
                    }
                } else {
                    throw runtime_error("Dictionary key must be a string");
                }
            }
        }
        return nullptr;
    });
}

Value* DictType::convertTo(ObjectType* targetType, Value* value) {
    if (!value) return nullptr;
    
    if (targetType == getTypeRegistry()->getType("string")) {
        if (Dict* dictVal = dynamic_cast<Dict*>(value)) {
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
    } else if (targetType == getTypeRegistry()->getType("bool")) {
        if (Dict* dictVal = dynamic_cast<Dict*>(value)) {
            return new Bool(!dictVal->getEntries().empty());
        }
    }
    
    return nullptr;
}

bool DictType::isCompatibleWith(ObjectType* other) const {
    if (!other) return false;
    return typeName == other->getTypeName();
}

Value* DictType::createDefaultValue() {
    return new Dict();
}

// ==================== NullType实现 ====================

// 空值类型实现
NullType::NullType() : ObjectType("null", true, false, false, false, false, nullptr) {
    // null类型不需要注册方法，因为它表示空值
}

Value* NullType::convertTo(ObjectType* targetType, Value* value) {
    if (!value) return nullptr;
    
    // null可以转换为任何类型，但转换结果仍然是null
    if (targetType == getTypeRegistry()->getType("string")) {
        return new String("null");
    } else if (targetType == getTypeRegistry()->getType("bool")) {
        return new Bool(false);
    } else if (targetType == getTypeRegistry()->getType("int")) {
        return new Integer(0);
    } else if (targetType == getTypeRegistry()->getType("double")) {
        return new Double(0.0);
    } else if (targetType == getTypeRegistry()->getType("char")) {
        return new Char('\0');
    } else if (targetType == getTypeRegistry()->getType("array")) {
        return new Array();
    } else if (targetType == getTypeRegistry()->getType("dict")) {
        return new Dict();
    }
    
    // 对于其他类型，返回null
    return new Null();
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
// 暂时注释掉有问题的宏，手动注册类型

// 注册基本类型 - 暂时使用手动注册
// REGISTER_BUILTIN_TYPE(int, IntType)
// REGISTER_BUILTIN_TYPE(double, DoubleType)
// REGISTER_BUILTIN_TYPE(char, CharType)
// REGISTER_BUILTIN_TYPE(bool, BoolType)
// REGISTER_BUILTIN_TYPE(null, NullType)

// 注册复合类型
// REGISTER_BUILTIN_TYPE(string, StringType)
// REGISTER_BUILTIN_TYPE(array, ArrayType)
// REGISTER_BUILTIN_TYPE(dict, DictType)

// 注册带继承关系的类型
// StringType继承自ArrayType（因为字符串可以当作字符数组）
// REGISTER_BUILTIN_TYPE_WITH_PARENT(string, StringType, array, ArrayType)

// IntType继承自DoubleType（数值类型的层次结构）
// REGISTER_BUILTIN_TYPE_WITH_PARENT(int, IntType, double, DoubleType)

// 手动注册内置类型
static TypeRegistry::AutoRegistrar int_registrar("int", new IntType());
static TypeRegistry::AutoRegistrar double_registrar("double", new DoubleType());
static TypeRegistry::AutoRegistrar char_registrar("char", new CharType());
static TypeRegistry::AutoRegistrar bool_registrar("bool", new BoolType());
static TypeRegistry::AutoRegistrar null_registrar("null", new NullType());
static TypeRegistry::AutoRegistrar string_registrar("string", new StringType());
static TypeRegistry::AutoRegistrar array_registrar("array", new ArrayType());
static TypeRegistry::AutoRegistrar dict_registrar("dict", new DictType());

// ==================== 工厂方法实现 ====================

// 全局工厂方法：根据Object*类型创建默认值
// Value* createDefaultValue(Object* type) {
//     if (!type) return new Integer(0);
//     
//     // 直接调用类型对象的createDefaultValue方法
//     return type->createDefaultValue();
// }

// ==================== ObjectFactory实现（单例模式） ====================

// 静态成员初始化
ObjectFactory* ObjectFactory::instance = nullptr;
std::mutex ObjectFactory::instanceMutex;

// 获取单例实例
ObjectFactory* ObjectFactory::getInstance() {
    std::lock_guard<std::mutex> lock(instanceMutex);
    if (!instance) {
        instance = new ObjectFactory();
    }
    return instance;
}

// 销毁单例实例
void ObjectFactory::destroyInstance() {
    std::lock_guard<std::mutex> lock(instanceMutex);
    if (instance) {
        delete instance;
        instance = nullptr;
    }
}

// 私有构造函数
ObjectFactory::ObjectFactory() : initialized(false), typeRegistry(nullptr), interpreter(nullptr) {
    initializeCreators();
}

// 析构函数
ObjectFactory::~ObjectFactory() {
    // 清理资源
}

// 初始化工厂映射
void ObjectFactory::initializeCreators() {
    if (initialized) return;
    
    // 注册基本类型的创建函数
    valueCreators["int"] = []() -> Value* { return new Integer(0); };
    valueCreators["double"] = []() -> Value* { return new Double(0.0); };
    valueCreators["char"] = []() -> Value* { return new Char('\0'); };
    valueCreators["bool"] = []() -> Value* { return new Bool(false); };
    valueCreators["null"] = []() -> Value* { return new Null(); };
    
    // 注册复合类型的创建函数
    valueCreators["string"] = []() -> Value* { return new String(""); };
    valueCreators["array"] = []() -> Value* { return new Array(); };
    valueCreators["dict"] = []() -> Value* { return new Dict(); };
    
    initialized = true;
}

// 根据Object类型创建对应的Value对象
Value* ObjectFactory::createValue(ObjectType* type) {
    if (!type) return new Null();
    
    string typeName = type->getTypeName();
    auto it = valueCreators.find(typeName);
    if (it != valueCreators.end()) {
        return it->second();
    }
    
    // 如果找不到对应的创建函数，使用类型的默认创建方法
    return type->createDefaultValue();
}

// 根据类型名称创建对应的Value对象
Value* ObjectFactory::createValue(const string& typeName) {
    auto it = valueCreators.find(typeName);
    if (it != valueCreators.end()) {
        return it->second();
    }
    
    // 如果找不到对应的创建函数，通过TypeRegistry获取类型对象
    ObjectType* type = TypeRegistry::getInstance()->getType(typeName);
    if (type) {
        return type->createDefaultValue();
    }
    
    // 如果找不到类型，返回null
    return new Null();
}

// 根据Object类型创建默认值
Value* ObjectFactory::createDefaultValue(ObjectType* type) {
    return createValue(type);
}

// 根据类型名称创建默认值
Value* ObjectFactory::createDefaultValue(const string& typeName) {
    return createValue(typeName);
}

// 创建null值
Value* ObjectFactory::createNull() {
    return new Null();
}

// 创建指定类型的null值（用于类型转换）
Value* ObjectFactory::createNullForType(ObjectType* type) {
    if (!type) return new Null();
    
    string typeName = type->getTypeName();
    return createNullForType(typeName);
}

// 创建指定类型的null值（用于类型转换）
Value* ObjectFactory::createNullForType(const string& typeName) {
    if (typeName == "null") {
        return new Null();
    } else if (typeName == "string") {
        return new String("null");
    } else if (typeName == "bool") {
        return new Bool(false);
    } else if (typeName == "int") {
        return new Integer(0);
    } else if (typeName == "double") {
        return new Double(0.0);
    } else if (typeName == "char") {
        return new Char('\0');
    } else if (typeName == "array") {
        return new Array();
    } else if (typeName == "dict") {
        return new Dict();
    }
    
    // 对于其他类型，返回null
    return new Null();
}

// 检查类型是否支持null值
bool ObjectFactory::supportsNull(ObjectType* type) {
    if (!type) return true;
    return supportsNull(type->getTypeName());
}

// 检查类型名称是否支持null值
bool ObjectFactory::supportsNull(const string& typeName) {
    // 所有类型都支持null值
    return true;
}

// ==================== 静态便捷方法实现 ====================

Value* ObjectFactory::createValueStatic(ObjectType* type) {
    return getInstance()->createValue(type);
}

Value* ObjectFactory::createValueStatic(const string& typeName) {
    return getInstance()->createValue(typeName);
}

Value* ObjectFactory::createDefaultValueStatic(ObjectType* type) {
    return getInstance()->createDefaultValue(type);
}

Value* ObjectFactory::createDefaultValueStatic(const string& typeName) {
    return getInstance()->createDefaultValue(typeName);
}

Value* ObjectFactory::createNullStatic() {
    return getInstance()->createNull();
}

Value* ObjectFactory::createNullForTypeStatic(ObjectType* type) {
    return getInstance()->createNullForType(type);
}

Value* ObjectFactory::createNullForTypeStatic(const string& typeName) {
    return getInstance()->createNullForType(typeName);
}

bool ObjectFactory::supportsNullStatic(ObjectType* type) {
    return getInstance()->supportsNull(type);
}

bool ObjectFactory::supportsNullStatic(const string& typeName) {
    return getInstance()->supportsNull(typeName);
}

// ==================== 全局工厂方法（向后兼容） ====================
// 全局工厂方法：根据ObjectType*类型创建默认值
Value* createDefaultValue(ObjectType* type) {
    return ObjectFactory::createDefaultValueStatic(type);
}

// 全局工厂方法：根据类型名称创建默认值
Value* createDefaultValue(const string& typeName) {
    return ObjectFactory::createDefaultValueStatic(typeName);
}

// ==================== 结构体和类实例化方法实现 ====================

Value* ObjectFactory::createStructInstance(StructType* structType, const vector<Value*>& args) {
    if (!structType) {
        LOG_ERROR("ObjectFactory::createStructInstance: structType is null");
        return nullptr;
    }
    
    LOG_DEBUG("ObjectFactory::createStructInstance: creating instance for struct '" + structType->getTypeName() + "'");
    
    // 尝试调用结构体的构造函数（同名方法）
    string constructorName = structType->getTypeName();
    const auto& methods = structType->getMethods();
    auto it = methods.find(constructorName);
    if (it != methods.end()) {
        function<Value*(Value*, vector<Value*>&)> constructor = it->second;
        
        // 如果有构造函数，调用它
        LOG_DEBUG("ObjectFactory::createStructInstance: calling constructor '" + constructorName + "'");
        return constructor(nullptr, const_cast<vector<Value*>&>(args));
    } else {
        // 如果没有构造函数，使用默认的实例化逻辑
        LOG_DEBUG("ObjectFactory::createStructInstance: no constructor found, using default instantiation");
        
        // 创建字典来存储实例的成员
        Dict* instance = new Dict();
        
        // 获取结构体的所有成员信息
        vector<string> memberNames = structType->getMemberNames();
        
        if (memberNames.empty()) {
            LOG_WARN("ObjectFactory::createStructInstance: struct '" + structType->getTypeName() + "' has no members");
            return instance;
        }
        
        // 处理参数初始化
        if (!args.empty() && dynamic_cast<Dict*>(args[0])) {
            // 结构体实例化语法：Person {name: "Alice", age: 25}
            Dict* memberDict = dynamic_cast<Dict*>(args[0]);
            initializeFromDict(instance, structType, memberDict);
        } else {
            // 构造函数语法：Person("Alice", 25) - 按参数顺序初始化
            initializeFromArgs(instance, structType, args);
        }
        
        LOG_DEBUG("ObjectFactory::createStructInstance: successfully created struct instance with " + 
                  to_string(memberNames.size()) + " members");
        
        return instance;
    }
}

Value* ObjectFactory::createClassInstance(ClassType* classType, const vector<Value*>& args) {
    if (!classType) {
        LOG_ERROR("ObjectFactory::createClassInstance: classType is null");
        return nullptr;
    }
    
    LOG_DEBUG("ObjectFactory::createClassInstance: creating instance for class '" + classType->getTypeName() + "'");
    
    // 尝试调用类的构造函数（同名方法）
    string constructorName = classType->getTypeName();
    const auto& methods = classType->getMethods();
    auto it = methods.find(constructorName);
    if (it != methods.end()) {
        function<Value*(Value*, vector<Value*>&)> constructor = it->second;
        
        // 如果有构造函数，调用它
        LOG_DEBUG("ObjectFactory::createClassInstance: calling constructor '" + constructorName + "'");
        return constructor(nullptr, const_cast<vector<Value*>&>(args));
    } else {
        // 如果没有构造函数，使用默认的实例化逻辑
        LOG_DEBUG("ObjectFactory::createClassInstance: no constructor found, using default instantiation");
        
        // 创建字典来存储实例的成员
        Dict* instance = new Dict();
        
        // 获取类的所有成员信息
        vector<string> memberNames = classType->getMemberNames();
        
        if (memberNames.empty()) {
            LOG_WARN("ObjectFactory::createClassInstance: class '" + classType->getTypeName() + "' has no members");
            return instance;
        }
        
        // 处理参数初始化
        if (!args.empty() && dynamic_cast<Dict*>(args[0])) {
            // 类实例化语法：Student {name: "Bob", grade: 10}
            Dict* memberDict = dynamic_cast<Dict*>(args[0]);
            initializeFromDict(instance, classType, memberDict);
        } else {
            // 构造函数语法：Student("Bob", 10) - 按参数顺序初始化
            initializeFromArgs(instance, classType, args);
        }
        
        // 初始化类的方法（结构体不需要）
        initializeMethods(instance, classType);
        
        LOG_DEBUG("ObjectFactory::createClassInstance: successfully created class instance with " + 
                  to_string(memberNames.size()) + " members");
        
        return instance;
    }
}

// 根据结构体类型名称创建实例
Value* ObjectFactory::createStructInstance(const string& structTypeName, const vector<Value*>& args) {
    if (structTypeName.empty()) {
        LOG_ERROR("ObjectFactory::createStructInstance: structTypeName is empty");
        return nullptr;
    }
    
    // 通过TypeRegistry获取结构体类型
    StructType* structType = nullptr;
    if (typeRegistry) {
        ObjectType* type = typeRegistry->getType(structTypeName);
        structType = dynamic_cast<StructType*>(type);
    }
    
    if (!structType) {
        LOG_ERROR("ObjectFactory::createStructInstance: struct type '" + structTypeName + "' not found");
        return nullptr;
    }
    
    return createStructInstance(structType, args);
}

// 根据类类型名称创建实例
Value* ObjectFactory::createClassInstance(const string& classTypeName, const vector<Value*>& args) {
    if (classTypeName.empty()) {
        LOG_ERROR("ObjectFactory::createClassInstance: classTypeName is empty");
        return nullptr;
    }
    
    // 通过TypeRegistry获取类类型
    ClassType* classType = nullptr;
    if (typeRegistry) {
        ObjectType* type = typeRegistry->getType(classTypeName);
        classType = dynamic_cast<ClassType*>(type);
    }
    
    if (!classType) {
        LOG_ERROR("ObjectFactory::createClassInstance: class type '" + classTypeName + "' not found");
        return nullptr;
    }
    
    return createClassInstance(classType, args);
}

// 静态便捷方法实现
Value* ObjectFactory::createStructInstanceStatic(StructType* structType, const vector<Value*>& args) {
    return getInstance()->createStructInstance(structType, args);
}

Value* ObjectFactory::createStructInstanceStatic(const string& structTypeName, const vector<Value*>& args) {
    return getInstance()->createStructInstance(structTypeName, args);
}

Value* ObjectFactory::createClassInstanceStatic(ClassType* classType, const vector<Value*>& args) {
    return getInstance()->createClassInstance(classType, args);
}

Value* ObjectFactory::createClassInstanceStatic(const string& classTypeName, const vector<Value*>& args) {
    return getInstance()->createClassInstance(classTypeName, args);
}

// ==================== 依赖注入实现 ====================

void ObjectFactory::setTypeRegistry(TypeRegistry* registry) {
    typeRegistry = registry;
}

void ObjectFactory::setInterpreter(Interpreter* interp) {
    interpreter = interp;
}

// ==================== 通用实例化辅助方法实现 ====================

void ObjectFactory::initializeFromDict(Dict* instance, ClassType* type, Dict* memberDict) {
    if (!instance || !type || !memberDict) {
        LOG_ERROR("ObjectFactory::initializeFromDict: null parameter");
        return;
    }
    
    string typeName = type->getTypeName();
    LOG_DEBUG("ObjectFactory::initializeFromDict: initializing " + typeName + " from dictionary");
    
    // 获取成员名称
    vector<string> memberNames = type->getMemberNames();
    
    // 按照定义的成员顺序初始化
    for (const string& memberName : memberNames) {
        // 检查可见性（结构体所有成员都是公有的，类可能有私有成员）
        VisibilityType visibility = type->getMemberVisibility(memberName);
        bool isStruct = dynamic_cast<StructType*>(type) != nullptr;
        
        // 结构体所有成员都是公有的，类需要检查可见性
        if (isStruct || visibility == VIS_PUBLIC) {
            Value* memberValue = memberDict->getEntry(memberName);
            if (memberValue) {
                // 如果提供了该成员的值，使用提供的值
                instance->setEntry(memberName, memberValue);
                LOG_DEBUG("ObjectFactory::initializeFromDict: set member '" + memberName + "' from dict");
            } else {
                // 使用默认值
                Value* defaultVal = createMemberDefaultValue(type, memberName);
                instance->setEntry(memberName, defaultVal);
                LOG_DEBUG("ObjectFactory::initializeFromDict: set member '" + memberName + "' to default value");
            }
        } else {
            LOG_DEBUG("ObjectFactory::initializeFromDict: skipping private/protected member '" + memberName + "'");
        }
    }
}

void ObjectFactory::initializeFromArgs(Dict* instance, ClassType* type, const vector<Value*>& args) {
    if (!instance || !type) {
        LOG_ERROR("ObjectFactory::initializeFromArgs: null parameter");
        return;
    }
    
    string typeName = type->getTypeName();
    LOG_DEBUG("ObjectFactory::initializeFromArgs: initializing " + typeName + " from arguments");
    
    // 获取成员名称
    vector<string> memberNames = type->getMemberNames();
    
    // 按参数顺序初始化成员
    for (size_t i = 0; i < memberNames.size() && i < args.size(); ++i) {
        const string& memberName = memberNames[i];
        instance->setEntry(memberName, args[i]);
        LOG_DEBUG("ObjectFactory::initializeFromArgs: set member '" + memberName + "' from arg[" + to_string(i) + "]");
    }
    
    // 为未初始化的成员设置默认值
    for (size_t i = args.size(); i < memberNames.size(); ++i) {
        const string& memberName = memberNames[i];
        Value* defaultVal = createMemberDefaultValue(type, memberName);
        instance->setEntry(memberName, defaultVal);
        LOG_DEBUG("ObjectFactory::initializeFromArgs: set member '" + memberName + "' to default value");
    }
}

Value* ObjectFactory::createMemberDefaultValue(ClassType* type, const string& memberName) {
    if (!type) {
        LOG_ERROR("ObjectFactory::createMemberDefaultValue: type is null");
        return new Null();
    }
    
    // 获取成员类型
    Object* memberType = type->getMemberType(memberName);
    if (!memberType) {
        LOG_WARN("ObjectFactory::createMemberDefaultValue: member '" + memberName + "' not found in " + type->getTypeName());
        return new Null();
    }
    
    // 使用ObjectFactory创建该类型的默认值
    return createDefaultValue(memberType);
}

void ObjectFactory::initializeMethods(Dict* instance, ClassType* type) {
    if (!instance || !type) {
        LOG_ERROR("ObjectFactory::initializeMethods: null parameter");
        return;
    }
    
    string typeName = type->getTypeName();
    LOG_DEBUG("ObjectFactory::initializeMethods: initializing methods for " + typeName);
    
    // 检查是否为结构体（结构体不需要初始化方法）
    if (dynamic_cast<StructType*>(type) != nullptr) {
        LOG_DEBUG("ObjectFactory::initializeMethods: skipping method initialization for struct " + typeName);
        return;
    }
    
    // 获取类型的所有方法
    const map<string, function<Value*(Value*, vector<Value*>&)>>& methods = type->getMethods();
    
    // 将方法绑定到实例
    for (const auto& methodPair : methods) {
        const string& methodName = methodPair.first;
        const auto& methodFunc = methodPair.second;
        
        // 创建方法包装器
        // TODO: 这里需要实现方法绑定逻辑
        // 目前只是记录日志，实际实现需要创建方法对象
        LOG_DEBUG("ObjectFactory::initializeMethods: binding method '" + methodName + "' to instance");
    }
    
    LOG_DEBUG("ObjectFactory::initializeMethods: initialized " + to_string(methods.size()) + " methods for " + typeName);
}