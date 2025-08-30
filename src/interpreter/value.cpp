#include "lexer/token.h"
#include "interpreter/value.h"
#include "interpreter/type_registry.h"

using namespace std;

// ==================== 全局常量定义 ====================
Value* NullValue = new Null();
Bool* TrueValue = new Bool(true);
Bool* FalseValue = new Bool(false);

// ==================== ValueFactory实现 ====================

Value* ValueFactory::createFromToken(const Token* token) {
    if (!token) return nullptr;
    
    switch (token->Tag) {
        case NUM:
            if (const Integer* intToken = dynamic_cast<const Integer*>(token)) {
                return new Integer(intToken->getValue());
            }
            break;
        case REAL:
        case DOUBLE:
            if (const Double* doubleToken = dynamic_cast<const Double*>(token)) {
                return new Double(doubleToken->getValue());
            }
            break;
        case BOOL:
            if (const Bool* boolToken = dynamic_cast<const Bool*>(token)) {
                return new Bool(boolToken->getValue());
            }
            break;
        case CHAR:
            if (const Char* charToken = dynamic_cast<const Char*>(token)) {
                return new Char(charToken->getValue());
            }
            break;
        case STR:
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

string String::substring(size_t start, size_t end) const {
    if (start >= value.length() || end > value.length() || start >= end) {
        throw runtime_error("Invalid substring range");
    }
    return value.substr(start, end - start);
}

// Array类型的实现
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

// ObjectValue类型的实现
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
