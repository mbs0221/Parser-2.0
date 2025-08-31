#include "lexer/token.h"
#include "interpreter/value.h"
#include "interpreter/type_registry.h"

using namespace std;

// ==================== 全局常量定义 ====================
Value* NullValue = new Null();
Bool* TrueValue = new Bool(true);
Bool* FalseValue = new Bool(false);

// ==================== ValueFactory实现 ====================

Value* ValueFactory::create(const string& typeName, const vector<Value*>& args) {
    if (typeName == "null") {
        return createNull();
    } else if (typeName == "bool") {
        if (args.empty()) {
            return createBool(false);
        } else if (args.size() == 1 && args[0]) {
            return createFromValue(args[0], "bool");
        }
        return createBool(false);
    } else if (typeName == "int") {
        if (args.empty()) {
            return createInteger(0);
        } else if (args.size() == 1 && args[0]) {
            return createFromValue(args[0], "int");
        }
        return createInteger(0);
    } else if (typeName == "double") {
        if (args.empty()) {
            return createDouble(0.0);
        } else if (args.size() == 1 && args[0]) {
            return createFromValue(args[0], "double");
        }
        return createDouble(0.0);
    } else if (typeName == "char") {
        if (args.empty()) {
            return createChar('\0');
        } else if (args.size() == 1 && args[0]) {
            return createFromValue(args[0], "char");
        }
        return createChar('\0');
    } else if (typeName == "string") {
        if (args.empty()) {
            return createString("");
        } else if (args.size() == 1 && args[0]) {
            return createFromValue(args[0], "string");
        }
        return createString("");
    } else if (typeName == "array") {
        if (args.empty()) {
            return createArray();
        } else {
            // 创建数组并添加所有参数作为元素
            Array* newArray = createArray();
            for (Value* arg : args) {
                newArray->addElement(arg);
            }
            return newArray;
        }
    } else if (typeName == "dict") {
        if (args.empty()) {
            return createDict();
        } else {
            // 处理键值对参数
            Dict* newDict = createDict();
            for (size_t i = 0; i < args.size(); i += 2) {
                if (i + 1 < args.size()) {
                    if (String* key = dynamic_cast<String*>(args[i])) {
                        newDict->setEntry(key->getValue(), args[i + 1]);
                    }
                }
            }
            return newDict;
        }
    }
    
    return nullptr;
}

Value* ValueFactory::createNew(const string& typeName, Value* instance, const vector<Value*>& args) {
    // 忽略instance参数，直接调用create方法
    return create(typeName, args);
}

Value* ValueFactory::createFromValue(Value* source, const string& targetType) {
    if (!source) return nullptr;
    
    if (targetType == "bool") {
        if (Integer* intVal = dynamic_cast<Integer*>(source)) {
            return new Bool(intVal->getValue() != 0);
        } else if (Double* doubleVal = dynamic_cast<Double*>(source)) {
            return new Bool(doubleVal->getValue() != 0.0);
        } else if (String* strVal = dynamic_cast<String*>(source)) {
            return new Bool(!strVal->getValue().empty());
        } else if (Bool* boolVal = dynamic_cast<Bool*>(source)) {
            return new Bool(boolVal->getValue());
        } else if (Char* charVal = dynamic_cast<Char*>(source)) {
            return new Bool(charVal->getValue() != '\0');
        } else if (Array* arrVal = dynamic_cast<Array*>(source)) {
            return new Bool(!arrVal->getElements().empty());
        } else if (Dict* dictVal = dynamic_cast<Dict*>(source)) {
            return new Bool(!dictVal->getEntries().empty());
        }
    } else if (targetType == "int") {
        if (Integer* intVal = dynamic_cast<Integer*>(source)) {
            return new Integer(intVal->getValue());
        } else if (Double* doubleVal = dynamic_cast<Double*>(source)) {
            return new Integer(static_cast<int>(doubleVal->getValue()));
        } else if (String* strVal = dynamic_cast<String*>(source)) {
            try {
                return new Integer(stoi(strVal->getValue()));
            } catch (...) {
                return new Integer(0);
            }
        } else if (Bool* boolVal = dynamic_cast<Bool*>(source)) {
            return new Integer(boolVal->getValue() ? 1 : 0);
        } else if (Char* charVal = dynamic_cast<Char*>(source)) {
            return new Integer(static_cast<int>(charVal->getValue()));
        }
    } else if (targetType == "double") {
        if (Double* doubleVal = dynamic_cast<Double*>(source)) {
            return new Double(doubleVal->getValue());
        } else if (Integer* intVal = dynamic_cast<Integer*>(source)) {
            return new Double(static_cast<double>(intVal->getValue()));
        } else if (String* strVal = dynamic_cast<String*>(source)) {
            try {
                return new Double(stod(strVal->getValue()));
            } catch (...) {
                return new Double(0.0);
            }
        } else if (Bool* boolVal = dynamic_cast<Bool*>(source)) {
            return new Double(boolVal->getValue() ? 1.0 : 0.0);
        } else if (Char* charVal = dynamic_cast<Char*>(source)) {
            return new Double(static_cast<double>(charVal->getValue()));
        }
    } else if (targetType == "char") {
        if (Char* charVal = dynamic_cast<Char*>(source)) {
            return new Char(charVal->getValue());
        } else if (Integer* intVal = dynamic_cast<Integer*>(source)) {
            return new Char(static_cast<char>(intVal->getValue()));
        } else if (String* strVal = dynamic_cast<String*>(source)) {
            string str = strVal->getValue();
            return new Char(str.empty() ? '\0' : str[0]);
        } else if (Bool* boolVal = dynamic_cast<Bool*>(source)) {
            return new Char(boolVal->getValue() ? '1' : '0');
        }
    } else if (targetType == "string") {
        if (String* strVal = dynamic_cast<String*>(source)) {
            return new String(strVal->getValue());
        } else if (Integer* intVal = dynamic_cast<Integer*>(source)) {
            return new String(to_string(intVal->getValue()));
        } else if (Double* doubleVal = dynamic_cast<Double*>(source)) {
            return new String(to_string(doubleVal->getValue()));
        } else if (Bool* boolVal = dynamic_cast<Bool*>(source)) {
            return new String(boolVal->getValue() ? "true" : "false");
        } else if (Char* charVal = dynamic_cast<Char*>(source)) {
            return new String(string(1, charVal->getValue()));
        } else if (Array* arrVal = dynamic_cast<Array*>(source)) {
            return new String(arrVal->toString());
        } else if (Dict* dictVal = dynamic_cast<Dict*>(source)) {
            return new String(dictVal->toString());
        }
    } else if (targetType == "array") {
        if (Array* arrVal = dynamic_cast<Array*>(source)) {
            return new Array(arrVal->getElements());
        } else if (String* strVal = dynamic_cast<String*>(source)) {
            // 将字符串转换为字符数组
            vector<Value*> chars;
            for (char c : strVal->getValue()) {
                chars.push_back(new Char(c));
            }
            return new Array(chars);
        }
    } else if (targetType == "dict") {
        if (Dict* dictVal = dynamic_cast<Dict*>(source)) {
            return new Dict(dictVal->getEntries());
        }
    }
    
    return nullptr;
}

Value* ValueFactory::createFromToken(const lexer::Token* token) {
    if (!token) return nullptr;
    
    switch (token->Tag) {
        case lexer::NUM:
            if (const Integer* intToken = dynamic_cast<const Integer*>(token)) {
                return new Integer(intToken->getValue());
            }
            break;
        case lexer::REAL:
        case lexer::DOUBLE:
            if (const Double* doubleToken = dynamic_cast<const Double*>(token)) {
                return new Double(doubleToken->getValue());
            }
            break;
        case lexer::BOOL:
            if (const Bool* boolToken = dynamic_cast<const Bool*>(token)) {
                return new Bool(boolToken->getValue());
            }
            break;
        case lexer::CHAR:
            if (const Char* charToken = dynamic_cast<const Char*>(token)) {
                return new Char(charToken->getValue());
            }
            break;
        case lexer::STR:
            if (const String* stringToken = dynamic_cast<const String*>(token)) {
                return new String(stringToken->getValue());
            }
            break;
        default:
            break;
    }
    
    return nullptr;
}

// ==================== 运算符重载实现 ====================

// Bool类型的逻辑运算
Value* Bool::operator&&(const Value& other) const {
    if (const Bool* otherBool = dynamic_cast<const Bool*>(&other)) {
        return new Bool(value && otherBool->getValue());
    }
    throw runtime_error("Logical AND requires Bool operands");
}

Value* Bool::operator||(const Value& other) const {
    if (const Bool* otherBool = dynamic_cast<const Bool*>(&other)) {
        return new Bool(value || otherBool->getValue());
    }
    throw runtime_error("Logical OR requires Bool operands");
}

Value* Bool::operator!() const {
    return new Bool(!value);
}

// Integer类型的算术运算
Value* Integer::operator+(const Value& other) const {
    if (const Integer* otherInt = dynamic_cast<const Integer*>(&other)) {
        return new Integer(value + otherInt->getValue());
    } else if (const Double* otherDouble = dynamic_cast<const Double*>(&other)) {
        return new Double(value + otherDouble->getValue());
    }
    throw runtime_error("Addition not supported between these types");
}

Value* Integer::operator-(const Value& other) const {
    if (const Integer* otherInt = dynamic_cast<const Integer*>(&other)) {
        return new Integer(value - otherInt->getValue());
    } else if (const Double* otherDouble = dynamic_cast<const Double*>(&other)) {
        return new Double(value - otherDouble->getValue());
    }
    throw runtime_error("Subtraction not supported between these types");
}

Value* Integer::operator*(const Value& other) const {
    if (const Integer* otherInt = dynamic_cast<const Integer*>(&other)) {
        return new Integer(value * otherInt->getValue());
    } else if (const Double* otherDouble = dynamic_cast<const Double*>(&other)) {
        return new Double(value * otherDouble->getValue());
    }
    throw runtime_error("Multiplication not supported between these types");
}

Value* Integer::operator/(const Value& other) const {
    if (const Integer* otherInt = dynamic_cast<const Integer*>(&other)) {
        if (otherInt->getValue() == 0) {
            throw runtime_error("Division by zero");
        }
        return new Integer(value / otherInt->getValue());
    } else if (const Double* otherDouble = dynamic_cast<const Double*>(&other)) {
        if (otherDouble->getValue() == 0.0) {
            throw runtime_error("Division by zero");
        }
        return new Double(value / otherDouble->getValue());
    }
    throw runtime_error("Division not supported between these types");
}

Value* Integer::operator%(const Value& other) const {
    if (const Integer* otherInt = dynamic_cast<const Integer*>(&other)) {
        if (otherInt->getValue() == 0) {
            throw runtime_error("Modulo by zero");
        }
        return new Integer(value % otherInt->getValue());
    }
    throw runtime_error("Modulo operation requires Integer operands");
}

// Integer类型的比较运算
Value* Integer::operator==(const Value& other) const {
    if (const Integer* otherInt = dynamic_cast<const Integer*>(&other)) {
        return new Bool(value == otherInt->getValue());
    } else if (const Double* otherDouble = dynamic_cast<const Double*>(&other)) {
        return new Bool(value == otherDouble->getValue());
    }
    return new Bool(false);
}

Value* Integer::operator<(const Value& other) const {
    if (const Integer* otherInt = dynamic_cast<const Integer*>(&other)) {
        return new Bool(value < otherInt->getValue());
    } else if (const Double* otherDouble = dynamic_cast<const Double*>(&other)) {
        return new Bool(value < otherDouble->getValue());
    }
    throw runtime_error("Comparison not supported between these types");
}

Value* Integer::operator<=(const Value& other) const {
    if (const Integer* otherInt = dynamic_cast<const Integer*>(&other)) {
        return new Bool(value <= otherInt->getValue());
    } else if (const Double* otherDouble = dynamic_cast<const Double*>(&other)) {
        return new Bool(value <= otherDouble->getValue());
    }
    throw runtime_error("Comparison not supported between these types");
}

Value* Integer::operator>(const Value& other) const {
    if (const Integer* otherInt = dynamic_cast<const Integer*>(&other)) {
        return new Bool(value > otherInt->getValue());
    } else if (const Double* otherDouble = dynamic_cast<const Double*>(&other)) {
        return new Bool(value > otherDouble->getValue());
    }
    throw runtime_error("Comparison not supported between these types");
}

Value* Integer::operator>=(const Value& other) const {
    if (const Integer* otherInt = dynamic_cast<const Integer*>(&other)) {
        return new Bool(value >= otherInt->getValue());
    } else if (const Double* otherDouble = dynamic_cast<const Double*>(&other)) {
        return new Bool(value >= otherDouble->getValue());
    }
    throw runtime_error("Comparison not supported between these types");
}

Value* Integer::operator!=(const Value& other) const {
    if (const Integer* otherInt = dynamic_cast<const Integer*>(&other)) {
        return new Bool(value != otherInt->getValue());
    } else if (const Double* otherDouble = dynamic_cast<const Double*>(&other)) {
        return new Bool(value != otherDouble->getValue());
    }
    return new Bool(true);
}

// Integer类型的位运算
Value* Integer::operator&(const Value& other) const {
    if (const Integer* otherInt = dynamic_cast<const Integer*>(&other)) {
        return new Integer(value & otherInt->getValue());
    }
    throw runtime_error("Bitwise AND requires Integer operands");
}

Value* Integer::operator|(const Value& other) const {
    if (const Integer* otherInt = dynamic_cast<const Integer*>(&other)) {
        return new Integer(value | otherInt->getValue());
    }
    throw runtime_error("Bitwise OR requires Integer operands");
}

Value* Integer::operator^(const Value& other) const {
    if (const Integer* otherInt = dynamic_cast<const Integer*>(&other)) {
        return new Integer(value ^ otherInt->getValue());
    }
    throw runtime_error("Bitwise XOR requires Integer operands");
}

Value* Integer::operator<<(const Value& other) const {
    if (const Integer* otherInt = dynamic_cast<const Integer*>(&other)) {
        return new Integer(value << otherInt->getValue());
    }
    throw runtime_error("Left shift requires Integer operands");
}

Value* Integer::operator>>(const Value& other) const {
    if (const Integer* otherInt = dynamic_cast<const Integer*>(&other)) {
        return new Integer(value >> otherInt->getValue());
    }
    throw runtime_error("Right shift requires Integer operands");
}

Value* Integer::operator~() const {
    return new Integer(~value);
}

// Double类型的算术运算
Value* Double::operator+(const Value& other) const {
    if (const Integer* otherInt = dynamic_cast<const Integer*>(&other)) {
        return new Double(value + otherInt->getValue());
    } else if (const Double* otherDouble = dynamic_cast<const Double*>(&other)) {
        return new Double(value + otherDouble->getValue());
    }
    throw runtime_error("Addition not supported between these types");
}

Value* Double::operator-(const Value& other) const {
    if (const Integer* otherInt = dynamic_cast<const Integer*>(&other)) {
        return new Double(value - otherInt->getValue());
    } else if (const Double* otherDouble = dynamic_cast<const Double*>(&other)) {
        return new Double(value - otherDouble->getValue());
    }
    throw runtime_error("Subtraction not supported between these types");
}

Value* Double::operator*(const Value& other) const {
    if (const Integer* otherInt = dynamic_cast<const Integer*>(&other)) {
        return new Double(value * otherInt->getValue());
    } else if (const Double* otherDouble = dynamic_cast<const Double*>(&other)) {
        return new Double(value * otherDouble->getValue());
    }
    throw runtime_error("Multiplication not supported between these types");
}

Value* Double::operator/(const Value& other) const {
    if (const Integer* otherInt = dynamic_cast<const Integer*>(&other)) {
        if (otherInt->getValue() == 0) {
            throw runtime_error("Division by zero");
        }
        return new Double(value / otherInt->getValue());
    } else if (const Double* otherDouble = dynamic_cast<const Double*>(&other)) {
        if (otherDouble->getValue() == 0.0) {
            throw runtime_error("Division by zero");
        }
        return new Double(value / otherDouble->getValue());
    }
    throw runtime_error("Division not supported between these types");
}

// Double类型的比较运算
Value* Double::operator==(const Value& other) const {
    if (const Integer* otherInt = dynamic_cast<const Integer*>(&other)) {
        return new Bool(value == otherInt->getValue());
    } else if (const Double* otherDouble = dynamic_cast<const Double*>(&other)) {
        return new Bool(value == otherDouble->getValue());
    }
    return new Bool(false);
}

Value* Double::operator<(const Value& other) const {
    if (const Integer* otherInt = dynamic_cast<const Integer*>(&other)) {
        return new Bool(value < otherInt->getValue());
    } else if (const Double* otherDouble = dynamic_cast<const Double*>(&other)) {
        return new Bool(value < otherDouble->getValue());
    }
    throw runtime_error("Comparison not supported between these types");
}

// String类型的字符串操作
Value* String::operator+(const Value& other) const {
    if (const String* otherString = dynamic_cast<const String*>(&other)) {
        return new String(value + otherString->getValue());
    } else {
        // 尝试将其他类型转换为字符串
        return new String(value + other.toString());
    }
}

Value* String::access(int index) {
    if (index >= 0 && index < static_cast<int>(value.length())) {
        return new Char(value[index]);
    }
    throw runtime_error("String index out of bounds");
}

// String的下标访问运算符重载 ([] 运算符)
Value* String::operator[](const Value& index) const {
    // 检查索引类型
    if (const Integer* intIndex = dynamic_cast<const Integer*>(&index)) {
        int idx = intIndex->getValue();
        if (idx >= 0 && idx < static_cast<int>(value.length())) {
            return new Char(value[idx]);
        } else {
            throw runtime_error("String index out of bounds: " + to_string(idx));
        }
    } else {
        throw runtime_error("String index must be an integer");
    }
}

string String::substring(size_t start, size_t end) const {
    if (start >= value.length() || end > value.length() || start >= end) {
        throw runtime_error("Invalid substring range");
    }
    return value.substr(start, end - start);
}

// Array类型的实现
Array::Array(const Array& other) : Value(nullptr) {
    // 深拷贝所有元素
    for (const auto& element : other.elements) {
        elements.push_back(element ? element->clone() : nullptr);
    }
}

Array& Array::operator=(const Array& other) {
    if (this != &other) {
        // 清空当前实例
        for (auto& element : elements) {
            delete element;
        }
        elements.clear();
        
        // 深拷贝所有元素
        for (const auto& element : other.elements) {
            elements.push_back(element ? element->clone() : nullptr);
        }
    }
    return *this;
}

Value* Array::getElement(size_t index) const {
    if (index >= elements.size()) {
        throw runtime_error("Array index out of bounds");
    }
    return elements[index] ? elements[index]->clone() : nullptr;
}

void Array::setElement(size_t index, Value* element) {
    if (index >= elements.size()) {
        elements.resize(index + 1, nullptr);
    }
    if (elements[index]) {
        delete elements[index];
    }
    elements[index] = element ? element->clone() : nullptr;
}

string Array::toString() const {
    ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < elements.size(); ++i) {
        if (i > 0) oss << ", ";
        if (elements[i]) {
            oss << elements[i]->toString();
        } else {
            oss << "null";
        }
    }
    oss << "]";
    return oss.str();
}

Value* Array::clone() const {
    return new Array(elements);
}

Value* Array::access(int index) {
    if (index >= 0 && index < static_cast<int>(elements.size())) {
        return elements[index] ? elements[index]->clone() : nullptr;
    }
    throw runtime_error("Array index out of bounds");
}

Value* Array::access(const string& key) {
    // 数组不支持字符串键访问
    throw runtime_error("Array does not support string key access");
}

// Array的下标访问运算符重载 ([] 运算符)
Value* Array::operator[](const Value& index) const {
    // 检查索引类型
    if (const Integer* intIndex = dynamic_cast<const Integer*>(&index)) {
        int idx = intIndex->getValue();
        if (idx >= 0 && idx < static_cast<int>(elements.size())) {
            return elements[idx] ? elements[idx]->clone() : nullptr;
        } else {
            throw runtime_error("Array index out of bounds: " + to_string(idx));
        }
    } else {
        throw runtime_error("Array index must be an integer");
    }
}

void Array::removeElement(int index) {
    if (index >= 0 && index < static_cast<int>(elements.size())) {
        if (elements[index]) {
            delete elements[index];
        }
        elements.erase(elements.begin() + index);
    }
}

// Dict类型的实现
void Dict::setEntry(const string& key, Value* value) {
    if (elements.find(key) != elements.end() && elements[key]) {
        delete elements[key];
    }
    elements[key] = value ? value->clone() : nullptr;
}

Value* Dict::getEntry(const string& key) const {
    auto it = elements.find(key);
    if (it != elements.end()) {
        return it->second ? it->second->clone() : nullptr;
    }
    return nullptr;
}

bool Dict::hasKey(const string& key) const {
    return elements.find(key) != elements.end();
}

void Dict::removeKey(const string& key) {
    auto it = elements.find(key);
    if (it != elements.end()) {
        if (it->second) {
            delete it->second;
        }
        elements.erase(it);
    }
}

vector<string> Dict::getKeys() const {
    vector<string> keys;
    for (const auto& pair : elements) {
        keys.push_back(pair.first);
    }
    return keys;
}

string Dict::toString() const {
    ostringstream oss;
    oss << "{";
    bool first = true;
    for (const auto& pair : elements) {
        if (!first) oss << ", ";
        oss << "\"" << pair.first << "\": ";
        if (pair.second) {
            oss << pair.second->toString();
        } else {
            oss << "null";
        }
        first = false;
    }
    oss << "}";
    return oss.str();
}

Value* Dict::clone() const {
    vector<pair<string, Value*>> pairs;
    for (const auto& pair : elements) {
        pairs.push_back({pair.first, pair.second ? pair.second->clone() : nullptr});
    }
    return new Dict(pairs);
}

Value* Dict::access(const string& key) {
    return getEntry(key);
}

// Dict的下标访问运算符重载 ([] 运算符)
Value* Dict::operator[](const Value& key) const {
    // 检查键类型
    if (const String* strKey = dynamic_cast<const String*>(&key)) {
        string keyStr = strKey->getValue();
        auto it = elements.find(keyStr);
        if (it != elements.end()) {
            return it->second ? it->second->clone() : nullptr;
        } else {
            throw runtime_error("Key '" + keyStr + "' not found in dictionary");
        }
    } else {
        throw runtime_error("Dictionary key must be a string");
    }
}

// 赋值运算符重载实现，用于Object类型的赋值运算
Dict& Dict::operator=(const Dict& other) {
    if (this != &other) {
        // 清空当前实例
        clear();
        
        // 深拷贝所有键值对
        for (const auto& entry : other.elements) {
            elements[entry.first] = entry.second ? entry.second->clone() : nullptr;
        }
    }
    return *this;
}

// ObjectValue类型的实现
ObjectValue::ObjectValue(const ObjectValue& other) : Value(nullptr), className(other.className) {
    // 深拷贝属性
    for (const auto& pair : other.properties) {
        properties[pair.first] = pair.second ? pair.second->clone() : nullptr;
    }
    // 深拷贝方法
    for (const auto& pair : other.methods) {
        methods[pair.first] = pair.second ? dynamic_cast<Function*>(pair.second->clone()) : nullptr;
    }
}

ObjectValue& ObjectValue::operator=(const ObjectValue& other) {
    if (this != &other) {
        // 清空当前实例
        for (auto& pair : properties) {
            delete pair.second;
        }
        properties.clear();
        
        for (auto& pair : methods) {
            delete pair.second;
        }
        methods.clear();
        
        // 复制类名
        className = other.className;
        
        // 深拷贝属性
        for (const auto& pair : other.properties) {
            properties[pair.first] = pair.second ? pair.second->clone() : nullptr;
        }
        
        // 深拷贝方法
        for (const auto& pair : other.methods) {
            methods[pair.first] = pair.second ? dynamic_cast<Function*>(pair.second->clone()) : nullptr;
        }
    }
    return *this;
}

void ObjectValue::setProperty(const string& name, Value* value) {
    if (properties.find(name) != properties.end() && properties[name]) {
        delete properties[name];
    }
    properties[name] = value ? value->clone() : nullptr;
}

Value* ObjectValue::getProperty(const string& name) const {
    auto it = properties.find(name);
    if (it != properties.end()) {
        return it->second ? it->second->clone() : nullptr;
    }
    return nullptr;
}

bool ObjectValue::hasProperty(const string& name) const {
    return properties.find(name) != properties.end();
}

void ObjectValue::setMethod(const string& name, Function* method) {
    if (methods.find(name) != methods.end() && methods[name]) {
        delete methods[name];
    }
    methods[name] = method ? dynamic_cast<Function*>(method->clone()) : nullptr;
}

Function* ObjectValue::getMethod(const string& name) const {
    auto it = methods.find(name);
    if (it != methods.end()) {
        return it->second ? dynamic_cast<Function*>(it->second->clone()) : nullptr;
    }
    return nullptr;
}

bool ObjectValue::hasMethod(const string& name) const {
    return methods.find(name) != methods.end();
}

string ObjectValue::toString() const {
    ostringstream oss;
    oss << className << " {";
    bool first = true;
    for (const auto& pair : properties) {
        if (!first) oss << ", ";
        oss << pair.first << ": ";
        if (pair.second) {
            oss << pair.second->toString();
        } else {
            oss << "null";
        }
        first = false;
    }
    oss << "}";
    return oss.str();
}

Value* ObjectValue::clone() const {
    ObjectValue* newObj = new ObjectValue(className);
    for (const auto& pair : properties) {
        newObj->setProperty(pair.first, pair.second);
    }
    for (const auto& pair : methods) {
        newObj->setMethod(pair.first, pair.second);
    }
    return newObj;
}

Value* ObjectValue::access(const string& key) {
    // 首先检查属性
    if (hasProperty(key)) {
        return getProperty(key);
    }
    // 然后检查方法
    if (hasMethod(key)) {
        return getMethod(key);
    }
    throw runtime_error("Property or method '" + key + "' not found in " + className);
}

// Function类型的实现
Value* Function::call(vector<Value*> args) {
    if (isBuiltin) {
        if (func) {
            return func(args);
        }
    } else {
        // AST函数调用的实现
        if (astDefinition) {
            // 这里需要实现AST函数的调用逻辑
            // 暂时返回nullptr
        }
    }
    return nullptr;
}

// ==================== Value类构造函数实现 ====================

Null::Null() : Value(nullptr) {
    TypeRegistry* registry = TypeRegistry::getInstance();
    if (registry) {
        setValueType(registry->getType("null"));
    }
}

Bool::Bool(bool val) : Value(nullptr), value(val) {
    TypeRegistry* registry = TypeRegistry::getInstance();
    if (registry) {
        setValueType(registry->getType("bool"));
    }
}

Integer::Integer(int val) : Value(nullptr), value(val) {
    TypeRegistry* registry = TypeRegistry::getInstance();
    if (registry) {
        setValueType(registry->getType("int"));
    }
}

Double::Double(double val) : Value(nullptr), value(val) {
    TypeRegistry* registry = TypeRegistry::getInstance();
    if (registry) {
        setValueType(registry->getType("double"));
    }
}

Char::Char(char val) : Value(nullptr), value(val) {
    TypeRegistry* registry = TypeRegistry::getInstance();
    if (registry) {
        setValueType(registry->getType("char"));
    }
}

String::String(const string& val) : Value(nullptr), value(val) {
    TypeRegistry* registry = TypeRegistry::getInstance();
    if (registry) {
        setValueType(registry->getType("string"));
    }
}
