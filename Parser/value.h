#ifndef VALUE_H
#define VALUE_H

#include "lexer.h"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <stdexcept>
#include <sstream>

using namespace std;

// ==================== 值类型基类 ====================
// 所有值的基类
struct Value {
    Type* valueType;  // 值的类型
    
    Value() : valueType(nullptr) {}
    Value(Type* type) : valueType(type) {}
    virtual ~Value() = default;
    
    // 获取值的字符串表示
    virtual string toString() const { return "value"; }
    
    // 访问操作 - 默认不支持访问
    virtual Value* access(Value* key) {
        throw runtime_error("Type does not support access operation");
    }
};

// ==================== 基本值类型 ====================
// 整数类型 - 作为值类型
struct IntegerValue : public Value {
    int value;
    
    IntegerValue() : Value(new Type(NUM, "int", 4)), value(0) {}
    IntegerValue(int val) : Value(new Type(NUM, "int", 4)), value(val) {}
    
    // 获取整数值
    int getValue() const {
        return value;
    }
    
    // 设置整数值
    void setValue(int val) {
        value = val;
    }
    
    // 算术运算
    IntegerValue operator+(const IntegerValue& other) const {
        return IntegerValue(value + other.value);
    }
    
    IntegerValue operator-(const IntegerValue& other) const {
        return IntegerValue(value - other.value);
    }
    
    IntegerValue operator*(const IntegerValue& other) const {
        return IntegerValue(value * other.value);
    }
    
    IntegerValue operator/(const IntegerValue& other) const {
        return IntegerValue(value / other.value);
    }
    
    string toString() const override {
        ostringstream oss;
        oss << value;
        return oss.str();
    }
};

// 浮点数类型 - 作为值类型
struct DoubleValue : public Value {
    double value;
    
    DoubleValue() : Value(new Type(REAL, "double", 8)), value(0.0) {}
    DoubleValue(double val) : Value(new Type(REAL, "double", 8)), value(val) {}
    
    // 获取浮点数值
    double getValue() const {
        return value;
    }
    
    // 设置浮点数值
    void setValue(double val) {
        value = val;
    }
    
    // 算术运算
    DoubleValue operator+(const DoubleValue& other) const {
        return DoubleValue(value + other.value);
    }
    
    DoubleValue operator-(const DoubleValue& other) const {
        return DoubleValue(value - other.value);
    }
    
    DoubleValue operator*(const DoubleValue& other) const {
        return DoubleValue(value * other.value);
    }
    
    DoubleValue operator/(const DoubleValue& other) const {
        return DoubleValue(value / other.value);
    }
    
    string toString() const override {
        ostringstream oss;
        oss << value;
        return oss.str();
    }
};

// 字符串类型 - 作为字符数组，支持字符访问
struct StringValue : public Value {
    string value;
    
    StringValue() : Value(new Type(STRING, "string", 0)) {}
    StringValue(const string& str) : Value(new Type(STRING, "string", 0)), value(str) {}
    
    // 获取字符串值
    const string& getValue() const {
        return value;
    }
    
    // 设置字符串值
    void setValue(const string& str) {
        value = str;
    }
    
    // 获取字符串长度
    size_t length() const {
        return value.length();
    }
    
    // 获取字符
    char getChar(size_t index) const {
        return index < value.length() ? value[index] : '\0';
    }
    
    // 设置字符
    void setChar(size_t index, char c) {
        if (index < value.length()) {
            value[index] = c;
        }
    }
    
    // 字符串拼接
    void append(const string& str) {
        value += str;
    }
    
    // 字符串拼接
    void append(char c) {
        value += c;
    }
    
    // 子字符串
    string substring(size_t start, size_t length) const {
        if (start >= value.length()) return "";
        return value.substr(start, length);
    }
    
    string toString() const override {
        return "\"" + value + "\"";
    }
    
    // 访问操作 - 将key作为字符索引
    Value* access(Value* key) override {
        // 将key转换为数字索引
        string keyStr = key->toString();
        try {
            int index = stoi(keyStr);
            if (index >= 0 && index < (int)value.length()) {
                // 创建一个字符字面量
                string charStr(1, value[index]);
                return new StringValue(charStr);
            }
            return nullptr;
        } catch (const exception&) {
            // 如果转换失败，返回nullptr
            return nullptr;
        }
    }
};

// 数组类型 - 支持传统数组和JSON数组
struct ArrayValue : public Value {
    vector<Value*> elements;
    
    ArrayValue() : Value(new Type(JSON, "array", 0)) {}
    
    // 添加元素
    void addElement(Value* element) {
        elements.push_back(element);
    }
    
    // 获取元素
    Value* getElement(size_t index) const {
        return index < elements.size() ? elements[index] : nullptr;
    }
    
    // 设置元素
    void setElement(size_t index, Value* element) {
        if (index < elements.size()) {
            elements[index] = element;
        } else if (index == elements.size()) {
            elements.push_back(element);
        }
    }
    
    // 获取元素数量
    size_t getElementCount() const {
        return elements.size();
    }
    
    // 获取数组大小
    size_t size() const {
        return elements.size();
    }
    
    string toString() const override {
        string result = "[";
        for (size_t i = 0; i < elements.size(); ++i) {
            if (i > 0) result += ", ";
            result += elements[i]->toString();
        }
        result += "]";
        return result;
    }
    
    // 访问操作 - 将key作为数组下标
    Value* access(Value* key) override {
        // 将key转换为数字索引
        string keyStr = key->toString();
        try {
            int index = stoi(keyStr);
            return index >= 0 && index < (int)elements.size() ? elements[index] : nullptr;
        } catch (const exception&) {
            // 如果转换失败，返回nullptr
            return nullptr;
        }
    }
};

// 字典类型 - 支持键值对存储
struct DictValue : public Value {
    map<string, Value*> entries;
    
    DictValue() : Value(new Type(JSON, "dict", 0)) {}
    
    // 设置键值对
    void setEntry(const string& key, Value* value) {
        entries[key] = value;
    }
    
    // 获取值
    Value* getEntry(const string& key) const {
        auto it = entries.find(key);
        return it != entries.end() ? it->second : nullptr;
    }
    
    // 检查键是否存在
    bool hasKey(const string& key) const {
        return entries.find(key) != entries.end();
    }
    
    // 删除键值对
    void removeEntry(const string& key) {
        entries.erase(key);
    }
    
    // 获取键值对数量
    size_t getEntryCount() const {
        return entries.size();
    }
    
    // 获取所有键
    vector<string> getKeys() const {
        vector<string> keys;
        for (const auto& pair : entries) {
            keys.push_back(pair.first);
        }
        return keys;
    }
    
    // 获取所有值
    vector<Value*> getValues() const {
        vector<Value*> values;
        for (const auto& pair : entries) {
            values.push_back(pair.second);
        }
        return values;
    }
    
    string toString() const override {
        string result = "{";
        bool first = true;
        for (const auto& pair : entries) {
            if (!first) result += ", ";
            result += "\"" + pair.first + "\": " + pair.second->toString();
            first = false;
        }
        result += "}";
        return result;
    }
    
    // 访问操作 - 将key作为字符串去查询
    Value* access(Value* key) override {
        // 将key转换为字符串
        string keyStr = key->toString();
        auto it = entries.find(keyStr);
        return it != entries.end() ? it->second : nullptr;
    }
};

#endif // VALUE_H
