#include "interpreter/values/value.h"
#include "interpreter/types/types.h"
#include <cmath>
#include <algorithm>
#include <iostream>
#include <sstream>

using namespace std;

// ==================== Value基类实现 ====================
Value::Value(ObjectType* vt) : valueType(vt), isReference(false), referencedValue(nullptr) {
}

ObjectType* Value::getValueType() const {
    return valueType;
}

void Value::setValueType(ObjectType* vt) {
    valueType = vt;
}

bool Value::isNull() const {
    return dynamic_cast<const Null*>(this) != nullptr;
}

bool Value::isReferenceType() const {
    return isReference;
}

std::string Value::getTypeName() const {
    if (valueType) {
        return valueType->getTypeName();
    }
    return "unknown";
}

void Value::setReference(Value* ref) {
    isReference = true;
    referencedValue = ref;
}

Value* Value::getReferencedValue() const {
    if (isReference && referencedValue) {
        return referencedValue;
    }
    return const_cast<Value*>(this);
}

bool Value::isCallable() const {
    return false; // 基类默认不可调用
}

bool Value::isInstance() const {
    return false; // 基类默认不是实例
}

// ==================== 全局常量定义 ====================
Value* NullValue = new Null();
Bool* TrueValue = new Bool(true);
Bool* FalseValue = new Bool(false);

// ==================== PrimitiveValue实现 ====================
PrimitiveValue::PrimitiveValue(ObjectType* vt, bool mutable_) 
    : Value(vt), mutable_(mutable_) {
}

bool PrimitiveValue::isMutable() const {
    return mutable_;
}

void PrimitiveValue::setMutable(bool mutable_) {
    this->mutable_ = mutable_;
}

// ==================== NumericValue实现 ====================
NumericValue::NumericValue(ObjectType* vt, bool mutable_) 
    : PrimitiveValue(vt, mutable_) {
}

// ==================== Null类型实现 ====================
Null::Null(ObjectType* vt) : PrimitiveValue(vt, false) {
    TypeRegistry* registry = TypeRegistry::getGlobalInstance();
    if (registry) {
        setValueType(registry->getType("null"));
    }
}

Null::Null(const Null& other) : PrimitiveValue(other.getValueType(), other.isMutable()) {
}

std::string Null::toString() const {
    return "null";
}

bool Null::toBool() const {
    return false;
}

Value* Null::clone() const {
    return new Null(*this);
}

std::string Null::getBuiltinTypeName() const {
    return "null";
}

// ==================== Bool类型实现 ====================
Bool::Bool(bool val) : PrimitiveValue(nullptr, false), value(val) {
    TypeRegistry* registry = TypeRegistry::getGlobalInstance();
    if (registry) {
        setValueType(registry->getType("bool"));
    }
}

Bool::Bool(const Bool& other) : PrimitiveValue(other.getValueType(), other.isMutable()), value(other.value) {}

Bool Bool::operator=(const Bool& other) {
    if (this != &other) {
        value = other.value;
        setValueType(other.getValueType());
    }
    return *this;
}

bool Bool::getValue() const {
    return value;
}

void Bool::setValue(bool val) {
    value = val;
}

string Bool::toString() const {
    return value ? "true" : "false";
}

bool Bool::toBool() const {
    return value;
}

Value* Bool::clone() const {
    return new Bool(*this);
}

string Bool::getBuiltinTypeName() const {
    return "bool";
}

// 布尔运算
Bool Bool::operator&&(const Bool& other) const {
    return Bool(value && other.value);
}

Bool Bool::operator||(const Bool& other) const {
    return Bool(value || other.value);
}

Bool Bool::operator!() const {
    return Bool(!value);
}

// 比较运算符重载
Bool Bool::operator==(const Bool& other) const {
    return Bool(value == other.value);
}

Bool Bool::operator!=(const Bool& other) const {
    return Bool(value != other.value);
}

Bool Bool::operator<(const Bool& other) const {
    return Bool(value < other.value);
}

Bool Bool::operator<=(const Bool& other) const {
    return Bool(value <= other.value);
}

Bool Bool::operator>(const Bool& other) const {
    return Bool(value > other.value);
}

Bool Bool::operator>=(const Bool& other) const {
    return Bool(value >= other.value);
}

// ==================== Integer类型实现 ====================
Integer::Integer(int val, ObjectType* vt) : NumericValue(vt, true), value(val) {
    TypeRegistry* registry = TypeRegistry::getGlobalInstance();
    if (registry) {
        setValueType(registry->getType("int"));
    }
}

Integer::Integer(const Integer& other) : NumericValue(other.getValueType(), other.isMutable()), value(other.value) {}

Integer& Integer::operator=(const Integer& other) {
    if (this != &other) {
        value = other.value;
        setValueType(other.getValueType());
    }
    return *this;
}

int Integer::getValue() const {
    return value;
}

void Integer::setValue(int val) {
    value = val;
}

NumericValue::TypeRank Integer::getTypeRank() const {
    return NumericValue::TypeRank::INTEGER;
}

Value* Integer::promoteTo(TypeRank targetRank) const {
    switch (targetRank) {
        case TypeRank::CHAR:
            return new Char(static_cast<char>(value));
        case TypeRank::INTEGER:
            return new Integer(value);
        case TypeRank::DOUBLE:
            return new Double(static_cast<double>(value));
        default:
            return new Integer(value);
    }
}

// 算术运算
Integer Integer::operator+(const Integer& other) const {
    return Integer(value + other.value);
}



Integer Integer::operator-(const Integer& other) const {
    return Integer(value - other.value);
}



Integer Integer::operator*(const Integer& other) const {
    return Integer(value * other.value);
}



Integer Integer::operator/(const Integer& other) const {
    if (other.value == 0) {
        throw runtime_error("Division by zero");
    }
    return Integer(value / other.value);
}



Integer Integer::operator%(const Integer& other) const {
    if (other.value == 0) {
        throw runtime_error("Modulo by zero");
    }
    return Integer(value % other.value);
}

// 比较运算
Bool Integer::operator==(const Integer& other) const {
    return Bool(static_cast<int>(value) == other.value);
}

Bool Integer::operator!=(const Integer& other) const {
    return Bool(static_cast<int>(value) != other.value);
}



Bool Integer::operator<(const Integer& other) const {
    return Bool(static_cast<int>(value) < other.value);
}

Bool Integer::operator<=(const Integer& other) const {
    return Bool(static_cast<int>(value) <= other.value);
}

Bool Integer::operator>(const Integer& other) const {
    return Bool(static_cast<int>(value) > other.value);
}

Bool Integer::operator>=(const Integer& other) const {
    return Bool(static_cast<int>(value) >= other.value);
}

// 位运算
Integer Integer::operator&(const Integer& other) const {
    return Integer(static_cast<int>(value) & other.value);
}

Integer Integer::operator|(const Integer& other) const {
    return Integer(static_cast<int>(value) | other.value);
}

Integer Integer::operator^(const Integer& other) const {
    return Integer(static_cast<int>(value) ^ other.value);
}

Integer Integer::operator<<(const Integer& other) const {
    return Integer(static_cast<int>(value) << other.value);
}

Integer Integer::operator>>(const Integer& other) const {
    return Integer(static_cast<int>(value) >> other.value);
}

Integer Integer::operator~() const {
    return Integer(~static_cast<int>(value));
}

// 一元运算
Integer Integer::operator+() const {
    return Integer(+static_cast<int>(value));
}

Integer Integer::operator-() const {
    return Integer(-static_cast<int>(value));
}

Bool Integer::operator!() const {
    return Bool(static_cast<int>(value) == 0);
}

// 自增自减
Integer& Integer::operator++() {
    ++value;
    return *this;
}

Integer Integer::operator++(int) {
    Integer result(*this);
    ++value;
    return result;
}

Integer& Integer::operator--() {
    --value;
    return *this;
}

Integer Integer::operator--(int) {
    Integer result(*this);
    --value;
    return result;
}

// 其他方法
string Integer::toString() const {
    return to_string(static_cast<int>(value));
}

bool Integer::toBool() const {
    return static_cast<int>(value) != 0;
}

double Integer::toDouble() const {
    return static_cast<double>(value);
}

Value* Integer::clone() const {
    return new Integer(*this);
}

string Integer::getBuiltinTypeName() const {
    return "int";
}

// ==================== Double类型实现 ====================
Double::Double(double val, ObjectType* vt) : NumericValue(vt, true), value(val) {
    TypeRegistry* registry = TypeRegistry::getGlobalInstance();
    if (registry) {
        setValueType(registry->getType("double"));
    }
}

Double::Double(const Double& other) : NumericValue(other.getValueType(), other.isMutable()), value(other.value) {}

Double& Double::operator=(const Double& other) {
    if (this != &other) {
        value = other.value;
        setValueType(other.getValueType());
    }
    return *this;
}

double Double::getValue() const {
    return value;
}

void Double::setValue(double val) {
    value = val;
}

NumericValue::TypeRank Double::getTypeRank() const {
    return NumericValue::TypeRank::DOUBLE;
}

Value* Double::promoteTo(TypeRank targetRank) const {
    switch (targetRank) {
        case TypeRank::CHAR:
            return new Char(static_cast<char>(value));
        case TypeRank::INTEGER:
            return new Integer(static_cast<int>(value));
        case TypeRank::DOUBLE:
            return new Double(value);
        default:
            return new Double(value);
    }
}



// 算术运算
Double Double::operator+(const Double& other) const {
    return Double(value + other.value);
}

Double Double::operator-(const Double& other) const {
    return Double(value - other.value);
}

Double Double::operator*(const Double& other) const {
    return Double(value * other.value);
}

Double Double::operator/(const Double& other) const {
    return Double(value / other.value);
}

// 一元运算符重载
Double Double::operator+() const {
    return Double(value);
}

Double Double::operator-() const {
    return Double(-value);
}

// 一元运算方法
double Double::unaryPlus() const {
    return value;
}

double Double::unaryMinus() const {
    return -value;
}

// 比较运算符重载
Bool Double::operator==(const Double& other) const {
    return Bool(value == other.value);
}

Bool Double::operator!=(const Double& other) const {
    return Bool(value != other.value);
}

Bool Double::operator<(const Double& other) const {
    return Bool(value < other.value);
}

Bool Double::operator<=(const Double& other) const {
    return Bool(value <= other.value);
}

Bool Double::operator>(const Double& other) const {
    return Bool(value > other.value);
}

Bool Double::operator>=(const Double& other) const {
    return Bool(value >= other.value);
}



// 其他方法
string Double::toString() const {
    ostringstream oss;
    oss << value;
    return oss.str();
}

bool Double::toBool() const {
    return value != 0.0;
}

Value* Double::clone() const {
    return new Double(*this);
}

string Double::getBuiltinTypeName() const {
    return "double";
}

// ==================== Char类型实现 ====================
Char::Char(char val, ObjectType* vt) : NumericValue(vt, true), value(val) {
    TypeRegistry* registry = TypeRegistry::getGlobalInstance();
    if (registry) {
        setValueType(registry->getType("char"));
    }
}

Char::Char(const Char& other) : NumericValue(other.getValueType(), other.isMutable()), value(other.value) {}

Char& Char::operator=(const Char& other) {
    if (this != &other) {
        value = other.value;
        setValueType(other.getValueType());
    }
    return *this;
}

NumericValue::TypeRank Char::getTypeRank() const {
    return NumericValue::TypeRank::CHAR;
}

Value* Char::promoteTo(TypeRank targetRank) const {
    switch (targetRank) {
        case TypeRank::CHAR:
            return new Char(value);
        case TypeRank::INTEGER:
            return new Integer(static_cast<int>(value));
        case TypeRank::DOUBLE:
            return new Double(static_cast<double>(value));
        default:
            return new Char(value);
    }
}

char Char::getValue() const {
    return value;
}

void Char::setValue(char val) {
    value = val;
}

// 比较运算
Bool Char::operator==(const Char& other) const {
    return Bool(static_cast<char>(value) == other.getValue());
}

Bool Char::operator!=(const Char& other) const {
    return Bool(static_cast<char>(value) != other.getValue());
}

Bool Char::operator<(const Char& other) const {
    return Bool(static_cast<char>(value) < other.getValue());
}

Bool Char::operator<=(const Char& other) const {
    return Bool(static_cast<char>(value) <= other.getValue());
}

Bool Char::operator>(const Char& other) const {
    return Bool(static_cast<char>(value) > other.getValue());
}

Bool Char::operator>=(const Char& other) const {
    return Bool(static_cast<char>(value) >= other.getValue());
}

// 算术运算符重载
Char Char::operator+(const Char& other) const {
    return Char(static_cast<char>(value) + other.value);
}

Char Char::operator-(const Char& other) const {
    return Char(static_cast<char>(value) - other.value);
}

Char Char::operator*(const Char& other) const {
    return Char(static_cast<char>(value) * other.value);
}

Char Char::operator/(const Char& other) const {
    if (other.value == 0) {
        // 避免除零错误，返回0
        return Char(0);
    }
    return Char(static_cast<char>(value) / other.value);
}

Char Char::operator%(const Char& other) const {
    if (other.value == 0) {
        // 避免除零错误，返回0
        return Char(0);
    }
    return Char(static_cast<char>(value) % static_cast<char>(other.value));
}

// 位运算运算符重载
Char Char::operator&(const Char& other) const {
    return Char(static_cast<char>(value) & static_cast<char>(other.value));
}

Char Char::operator|(const Char& other) const {
    return Char(static_cast<char>(value) | static_cast<char>(other.value));
}

Char Char::operator^(const Char& other) const {
    return Char(static_cast<char>(value) ^ static_cast<char>(other.value));
}

Char Char::operator<<(const Char& other) const {
    return Char(static_cast<char>(value) << static_cast<char>(other.value));
}

Char Char::operator>>(const Char& other) const {
    return Char(static_cast<char>(value) >> static_cast<char>(other.value));
}

Char Char::operator~() const {
    return Char(~static_cast<char>(value));
}

// Char类型的一元运算符实现
Char Char::operator+() const {
    return Char(+static_cast<char>(value));
}

Char Char::operator-() const {
    return Char(-static_cast<char>(value));
}

Bool Char::operator!() const {
    return Bool(static_cast<char>(value) == 0);
}

// 其他方法
string Char::toString() const {
    return string(1, static_cast<char>(value));
}

bool Char::toBool() const {
    return static_cast<char>(value) != '\0';
}

Value* Char::clone() const {
    return new Char(*this);
}

string Char::getBuiltinTypeName() const {
    return "char";
}

// ==================== String类型实现 ====================
String::String(const string& val, ObjectType* vt) : PrimitiveValue(vt, true), value(val) {
    // 如果传入的vt参数不为nullptr，使用它
    if (vt) {
        setValueType(vt);
    } else {
        // 否则尝试从TypeRegistry获取
        TypeRegistry* registry = TypeRegistry::getGlobalInstance();
        if (registry) {
            ObjectType* stringType = registry->getType("string");
            if (stringType) {
                setValueType(stringType);
            }
        }
        // 如果TypeRegistry还没有初始化或者StringType还没有注册，暂时不设置valueType
        // 这将在后续的某个时候被正确设置
    }
}

String::String(const String& other) : PrimitiveValue(other.getValueType(), other.isMutable()), value(other.value) {}

String& String::operator=(const String& other) {
    if (this != &other) {
        value = other.value;
        setValueType(other.getValueType());
    }
    return *this;
}

const string& String::getValue() const {
    return value;
}

void String::setValue(const string& val) {
    value = val;
}

// 字符串运算
String String::operator+(const String& other) const {
    return String(value + other.getValue());
}


// 比较运算符重载
Bool String::operator==(const String& other) const {
    return Bool(value == other.value);
}

Bool String::operator!=(const String& other) const {
    return Bool(value != other.value);
}

Bool String::operator<(const String& other) const {
    return Bool(value < other.value);
}

Bool String::operator<=(const String& other) const {
    return Bool(value <= other.value);
}

Bool String::operator>(const String& other) const {
    return Bool(value > other.value);
}

Bool String::operator>=(const String& other) const {
    return Bool(value >= other.value);
}

// 下标运算符重载
Char String::operator[](const Integer& index) const {
    int i = index.getValue();
    if (i >= 0 && i < static_cast<int>(value.length())) {
        return Char(value[i]);
    }
    throw runtime_error("String index out of bounds");
}

// 其他方法
string String::toString() const {
    return value;
}

bool String::toBool() const {
    return !value.empty();
}

Value* String::clone() const {
    return new String(*this);
}

string String::getBuiltinTypeName() const {
    return "string";
}

// 字符串特有方法
size_t String::length() const {
    return value.length();
}

bool String::isEmpty() const {
    return value.empty();
}

int String::indexOf(const String& substr) const {
    size_t pos = value.find(substr.getValue());
    return pos != string::npos ? static_cast<int>(pos) : -1;
}

int String::lastIndexOf(const String& substr) const {
    size_t pos = value.rfind(substr.getValue());
    return pos != string::npos ? static_cast<int>(pos) : -1;
}

String* String::substring(int start, int end) const {
    if (start < 0) start = 0;
    if (end > static_cast<int>(value.length())) end = value.length();
    if (start >= end) return new String("");
    
    return new String(value.substr(start, end - start));
}

String* String::toUpperCase() const {
    string upper = value;
    transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
    return new String(upper);
}

String* String::toLowerCase() const {
    string lower = value;
    transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    return new String(lower);
}

bool String::contains(const String& other) const {
    return value.find(other.getValue()) != string::npos;
}

bool String::startsWith(const String& other) const {
    const string& otherStr = other.getValue();
    if (otherStr.length() > value.length()) return false;
    return value.substr(0, otherStr.length()) == otherStr;
}

bool String::endsWith(const String& other) const {
    const string& otherStr = other.getValue();
    if (otherStr.length() > value.length()) return false;
    return value.substr(value.length() - otherStr.length()) == otherStr;
}

String* String::trim() const {
    string trimmed = value;
    trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r"));
    trimmed.erase(trimmed.find_last_not_of(" \t\n\r") + 1);
    return new String(trimmed);
}

vector<String*> String::split(const String& delimiter) const {
    vector<String*> result;
    string delim = delimiter.getValue();
    if (delim.empty()) {
        // 按字符分割
        for (char c : value) {
            result.push_back(new String(string(1, c)));
        }
        return result;
    }
    
    size_t pos = 0;
    size_t prev = 0;
    while ((pos = value.find(delim, prev)) != string::npos) {
        result.push_back(new String(value.substr(prev, pos - prev)));
        prev = pos + delim.length();
    }
    result.push_back(new String(value.substr(prev)));
    
    return result;
}

// append方法实现
void String::append(const String& other) {
    value += other.getValue();
}

void String::append(const std::string& other) {
    value += other;
}

void String::append(char ch) {
    value += ch;
}

String* String::replace(const String& oldStr, const String& newStr) const {
    string result = value;
    string old = oldStr.getValue();
    string new_ = newStr.getValue();
    
    size_t pos = 0;
    while ((pos = result.find(old, pos)) != string::npos) {
        result.replace(pos, old.length(), new_);
        pos += new_.length();
    }
    
    return new String(result);
}

// 注意：MethodValue 和 ClassMethodValue 已被合并到 MethodReference 中
// 所有相关功能现在通过统一的 MethodReference 类提供
