 
#ifndef INTERPRETER_VALUE_H
#define INTERPRETER_VALUE_H

// 避免与lexer/value.h中的类冲突

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <stdexcept>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <functional>
#include <variant>
#include <any>
#include <unordered_map>
#include <mutex>
#include "lexer/token.h"
// 移除builtin_type.h的包含，使用前向声明

// 前向声明
class ObjectType;
class TypeRegistry;
class Bool;

// 前向声明 - AST节点
struct FunctionDefinition;

using namespace std;

// 前向声明类型系统
class TypeRegistry;

// ==================== 运行时值基类 ====================
class Value{
protected:
    ObjectType* valueType;        // 指向值的类型系统
    bool isReference;
    Value* referencedValue;

public:
    Value(ObjectType* vt = nullptr) : valueType(vt), isReference(false), referencedValue(nullptr) {}
    virtual ~Value() = default;

    // 类型信息
    ObjectType* getValueType() const { return valueType; }
    void setValueType(ObjectType* vt) { valueType = vt; }
    bool isNull() const { return valueType == nullptr; }
    bool isReferenceType() const { return isReference; }
    
    // 获取类型名称
    string getTypeName() const {
        // 使用内置类型标识符，避免循环依赖
        return getBuiltinTypeName();
    }
    
    // 获取内置类型名称（由子类实现）
    virtual string getBuiltinTypeName() const = 0;

    // 引用管理
    void setReference(Value* ref) { 
        isReference = true; 
        referencedValue = ref; 
    }
    Value* getReferencedValue() const { return referencedValue; }

    // 虚函数接口
    virtual string toString() const = 0;
    virtual bool toBool() const = 0;
    virtual Value* clone() const = 0;

    
    // 访问操作
    virtual Value* access(const string& key) {
        throw runtime_error("Type does not support access operation");
    }
    virtual Value* access(int index) {
        throw runtime_error("Type does not support index access");
    }

    // 运算符重载
    virtual Value* operator+(const Value& other) const {
        throw runtime_error("Addition not supported for this type");
    }
    virtual Value* operator-(const Value& other) const {
        throw runtime_error("Subtraction not supported for this type");
    }
    virtual Value* operator*(const Value& other) const {
        throw runtime_error("Multiplication not supported for this type");
    }
    virtual Value* operator/(const Value& other) const {
        throw runtime_error("Division not supported for this type");
    }
    virtual Value* operator==(const Value& other) const {
        throw runtime_error("Equality comparison not supported for this type");
    }
    virtual Value* operator<(const Value& other) const {
        throw runtime_error("Less than comparison not supported for this type");
    }
};

// ==================== 空值类型 ====================
class Null : public Value {
public:
    Null() : Value(nullptr) {}
    
    string toString() const override { return "null"; }
    bool toBool() const override { return false; }
    Value* clone() const override { return new Null(); }
    string getBuiltinTypeName() const override { return "null"; }
};

// ==================== 布尔值类型 ====================
class Bool : public Value {
private:
    bool value;

public:
    Bool(bool val = false) : Value(nullptr), value(val) {}
    
    bool getValue() const { return value; }
    void setValue(bool val) { value = val; }
    
    string toString() const override { return value ? "true" : "false"; }
    bool toBool() const override { return value; }
    Value* clone() const override { return new Bool(value); }
    string getBuiltinTypeName() const override { return "bool"; }
    
    // 逻辑运算
    Value* operator&&(const Value& other) const;
    Value* operator||(const Value& other) const;
    Value* operator!() const;
};

    // ==================== 整数值类型 ====================
    class Integer : public Value {
private:
    int value;

public:
    Integer(int val = 0) : Value(nullptr), value(val) {}
    
    int getValue() const { return value; }
    void setValue(int val) { value = val; }
    
    string toString() const override { return to_string(value); }
    bool toBool() const override { return value != 0; }
    Value* clone() const override { return new Integer(value); }
    string getBuiltinTypeName() const override { return "int"; }
    
    // 算术运算
    Value* operator+(const Value& other) const override;
    Value* operator-(const Value& other) const override;
    Value* operator*(const Value& other) const override;
    Value* operator/(const Value& other) const override;
    Value* operator%(const Value& other) const;
    
    // 比较运算
    Value* operator==(const Value& other) const override;
    Value* operator<(const Value& other) const override;
    Value* operator<=(const Value& other) const;
    Value* operator>(const Value& other) const;
    Value* operator>=(const Value& other) const;
    Value* operator!=(const Value& other) const;
    
    // 位运算
    Value* operator&(const Value& other) const;
    Value* operator|(const Value& other) const;
    Value* operator^(const Value& other) const;
    Value* operator<<(const Value& other) const;
    Value* operator>>(const Value& other) const;
    Value* operator~() const;
};

// ==================== 浮点数值类型 ====================
class Double : public Value {
private:
    double value;

public:
    Double(double val = 0.0) : Value(nullptr), value(val) {}
    
    double getValue() const { return value; }
    void setValue(double val) { value = val; }
    
    string toString() const override {
        ostringstream oss;
        oss << std::fixed << std::setprecision(6) << value;
        string result = oss.str();
        
        // 去除末尾的0和小数点
        size_t pos = result.find('.');
        if (pos != string::npos) {
            size_t end = result.length() - 1;
            while (end > pos && result[end] == '0') {
                end--;
            }
            if (end == pos) {
                result = result.substr(0, pos);
            } else {
                result = result.substr(0, end + 1);
            }
        }
        return result;
    }
    
    bool toBool() const override { return value != 0.0; }
    Value* clone() const override { return new Double(value); }
    string getBuiltinTypeName() const override { return "double"; }
    
    // 算术运算
    Value* operator+(const Value& other) const override;
    Value* operator-(const Value& other) const override;
    Value* operator*(const Value& other) const override;
    Value* operator/(const Value& other) const override;
    
    // 比较运算
    Value* operator==(const Value& other) const override;
    Value* operator<(const Value& other) const override;
};

// ==================== 字符值类型 ====================
class Char : public Value {
private:
    char value;

public:
    Char(char val = '\0') : Value(nullptr), value(val) {}
    
    char getValue() const { return value; }
    void setValue(char val) { value = val; }
    
    string toString() const override { return string(1, value); }
    bool toBool() const override { return value != '\0'; }
    Value* clone() const override { return new Char(value); }
    string getBuiltinTypeName() const override { return "char"; }
};

// ==================== 字符串值类型 ====================
class String : public Value {
private:
    string value;

public:
    String(const string& val = "") : Value(nullptr), value(val) {}
    
    const string& getValue() const { return value; }
    void setValue(const string& val) { value = val; }
    
    string toString() const override { return value; }
    bool toBool() const override { return !value.empty(); }
    Value* clone() const override { return new String(value); }
    string getBuiltinTypeName() const override { return "string"; }
    
    // 字符串操作
    Value* operator+(const Value& other) const override;
    Value* access(int index) override;
    size_t length() const { return value.length(); }
    string substring(size_t start, size_t end) const;
};

// ==================== 数组值类型 ====================
class Array : public Value {
private:
    vector<Value*> elements;

public:
    // 构造函数：接受元素列表作为参数
    Array(const vector<Value*>& elements = {}) : Value(nullptr) {
        for (auto element : elements) {
            this->elements.push_back(element ? element->clone() : nullptr);
        }
    }
    ~Array() {
        for (auto element : elements) {
            delete element;
        }
    }
    
    // 数组操作
    void addElement(Value* element) { elements.push_back(element); }
    Value* getElement(size_t index) const;
    void setElement(size_t index, Value* element);
    size_t size() const { return elements.size(); }
    void clear() { elements.clear(); }
    
    // 新增方法
    const vector<Value*>& getElements() const { return elements; }
    void removeElement(int index);
    
    string toString() const override;
    bool toBool() const override { return !elements.empty(); }
    Value* clone() const override;
    string getBuiltinTypeName() const override { return "array"; }
    
    // 访问操作
    Value* access(int index) override;
    Value* access(const string& key) override;
};

// ==================== 字典值类型 ====================
class Dict : public Value {
private:
    unordered_map<string, Value*> elements;

public:
    // 构造函数：接受键值对列表作为参数
    Dict(const vector<pair<string, Value*>>& pairs = {}) : Value(nullptr) {
        for (const auto& pair : pairs) {
            elements[pair.first] = pair.second ? pair.second->clone() : nullptr;
        }
    }
    ~Dict() {
        for (auto& pair : elements) {
            delete pair.second;
        }
    }
    
    // 字典操作
    void setEntry(const string& key, Value* value);
    Value* getEntry(const string& key) const;
    bool hasKey(const string& key) const;
    void removeKey(const string& key);
    vector<string> getKeys() const;
    size_t size() const { return elements.size(); }
    
    // 新增方法
    const unordered_map<string, Value*>& getEntries() const { return elements; }
    void clear() { elements.clear(); }
    
    string toString() const override;
    bool toBool() const override { return !elements.empty(); }
    Value* clone() const override;
    string getBuiltinTypeName() const override { return "dict"; }
    
    // 访问操作
    Value* access(const string& key) override;
};

// ==================== 函数值类型 ====================
class Function : public Value {
private:
    string name;
    function<Value*(vector<Value*>)> func;
    vector<string> parameters;
    
    // AST函数定义相关
    struct FunctionDefinition* astDefinition;  // AST函数定义
    bool isBuiltin;  // 是否为内置函数

public:
    // 构造函数 - 内置函数
    Function(const string& funcName, 
                  function<Value*(vector<Value*>)> f,
                  const vector<string>& params = {})
        : Value(nullptr), name(funcName), func(f), parameters(params), 
          astDefinition(nullptr), isBuiltin(true) {}
    
    // 构造函数 - AST函数定义
    Function(const string& funcName, 
                  struct FunctionDefinition* def,
                  const vector<string>& params = {})
        : Value(nullptr), name(funcName), func(nullptr), parameters(params),
          astDefinition(def), isBuiltin(false) {}
    
    string getName() const { return name; }
    const vector<string>& getParameters() const { return parameters; }
    
    // 函数调用
    Value* call(vector<Value*> args);
    
    // 获取函数类型
    bool isBuiltinFunction() const { return isBuiltin; }
    struct FunctionDefinition* getASTDefinition() const { return astDefinition; }
    
    string toString() const override { return "function " + name; }
    bool toBool() const override { return true; }
    Value* clone() const override { 
        if (isBuiltin) {
            return new Function(name, func, parameters);
        } else {
            return new Function(name, astDefinition, parameters);
        }
    }
    string getBuiltinTypeName() const override { return "function"; }
};

// ==================== 对象值类型 ====================
class ObjectValue : public Value {
private:
    string className;
    map<string, Value*> properties;
    map<string, Function*> methods;

public:
    ObjectValue(const string& className) : Value(nullptr), className(className) {}
    ~ObjectValue() {
        for (auto& pair : properties) {
            delete pair.second;
        }
        for (auto& pair : methods) {
            delete pair.second;
        }
    }
    
    // 属性操作
    void setProperty(const string& name, Value* value);
    Value* getProperty(const string& name) const;
    bool hasProperty(const string& name) const;
    
    // 方法操作
    void setMethod(const string& name, Function* method);
    Function* getMethod(const string& name) const;
    bool hasMethod(const string& name) const;
    
    string getClassName() const { return className; }
    
    string toString() const override;
    bool toBool() const override { return true; }
    Value* clone() const override;
    string getBuiltinTypeName() const override { return "object"; }
    
    // 访问操作
    Value* access(const string& key) override;
};

// ==================== 值工厂类 ====================
class ValueFactory {
public:
    static Value* createNull() { return new Null(); }
    
    // 从Token创建值 - 已移除以避免lexer依赖
    Value* createFromToken(const Token* token);
    
    // 类型转换 - 委托给类型系统
    static Value* convertValue(Value* value, class ObjectType* targetType) {
        // 暂时返回原始值，避免循环依赖
        // TODO: 实现真正的类型转换
        return value ? value->clone() : nullptr;
    }
};

// ==================== 全局常量 ====================
extern Value* NullValue;
extern Bool* TrueValue;
extern Bool* FalseValue;

// ==================== Object工厂类（单例模式） ====================
// 用于创建指定Object子类型的Value对象的工厂类
// 注意：ObjectFactory的完整定义在builtin_type.h中，这里只提供前向声明
class ObjectFactory;

// 全局访问函数
// 注意：这个函数需要在包含完整ObjectFactory定义的文件中实现
ObjectFactory* getObjectFactory();

#endif // INTERPRETER_VALUE_H