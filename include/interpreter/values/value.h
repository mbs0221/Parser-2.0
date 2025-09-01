#ifndef INTERPRETER_VALUE_H
#define INTERPRETER_VALUE_H
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
#include <type_traits>

using namespace std; // 已移除，使用显式std前缀

// 前向声明
class ObjectType;
class TypeRegistry;
class ObjectFactory;
class Array;
class Dict;
class Null;
class FunctionPrototype;
class FunctionDefinition;

// ==================== 运行时值基类 ====================
class Value {
protected:
    ObjectType* valueType;        // 指向值的类型系统
    bool isReference;
    Value* referencedValue;
public:
    Value(ObjectType* vt = nullptr);
    virtual ~Value() = default;
    // 类型信息
    ObjectType* getValueType() const;
    void setValueType(ObjectType* vt);
    bool isNull() const;
    bool isReferenceType() const;
    // 获取类型名称
    std::string getTypeName() const;
    // 获取内置类型名称（由子类实现）
    virtual std::string getBuiltinTypeName() const = 0;
    // 引用管理
    void setReference(Value* ref);
    Value* getReferencedValue() const;
    // 虚函数接口
    virtual std::string toString() const = 0;
    virtual bool toBool() const = 0;
    virtual Value* clone() const = 0;
    // 类型检查方法
    virtual bool isCallable() const;
    virtual bool isInstance() const;
};

// ==================== 基本值类型基类 ====================
class PrimitiveValue : public Value {
protected:
    bool mutable_;
public:
    PrimitiveValue(ObjectType* vt = nullptr, bool mutable_ = true);
    virtual ~PrimitiveValue() = default;
    // 基本值操作
    bool isMutable() const;
    void setMutable(bool m);
};

// ==================== 数值值类型基类 ====================
class NumericValue : public PrimitiveValue {
public:
    NumericValue(ObjectType* vt = nullptr, bool mutable_ = true);
    virtual ~NumericValue() = default;
    // 数值类型等级
    enum class TypeRank {
        CHAR = 0,
        INTEGER = 1,
        DOUBLE = 2
    };
    virtual TypeRank getTypeRank() const = 0;
    virtual Value* promoteTo(TypeRank targetRank) const = 0;
};

// ==================== 布尔值类型 ====================
class Bool : public PrimitiveValue {
private:
    bool value;
public:
    Bool(bool val = false);
    Bool(const Bool& other);
    // 赋值运算符重载
    Bool operator=(const Bool& other);
    bool getValue() const;
    void setValue(bool val);
    std::string toString() const override;
    bool toBool() const override;
    Value* clone() const override;
    std::string getBuiltinTypeName() const override;
    // 逻辑运算符重载
    Bool operator&&(const Bool& other) const;
    Bool operator||(const Bool& other) const;
    Bool operator!() const;
    // 比较运算符重载
    Bool operator==(const Bool& other) const;
    Bool operator!=(const Bool& other) const;
    Bool operator<(const Bool& other) const;
    Bool operator<=(const Bool& other) const;
    Bool operator>(const Bool& other) const;
    Bool operator>=(const Bool& other) const;
};

// ==================== 空值类型 ====================
class Null : public PrimitiveValue {
public:
    Null(ObjectType* vt = nullptr);
    Null(const Null& other);
    // 重写基类方法
    std::string toString() const override;
    bool toBool() const override;
    Value* clone() const override;
    std::string getBuiltinTypeName() const override;
};

// ==================== 数值值类型 ====================
class Double : public NumericValue {
private:
    double value;  // 使用 double 类型存储
public:
    Double(double val = 0.0, ObjectType* vt = nullptr);
    Double(const Double& other);
    // 实现虚函数
    TypeRank getTypeRank() const override;
    Value* promoteTo(TypeRank targetRank) const override;
    // 重写基类方法
    std::string toString() const override;
    bool toBool() const override;
    Value* clone() const override;
    std::string getBuiltinTypeName() const override;
    // 获取和设置值
    double getValue() const;
    void setValue(double val);

    // 赋值运算符重载
    Double& operator=(const Double& other);

    // 比较运算符重载
    Bool operator==(const Double& other) const;
    Bool operator!=(const Double& other) const;
    Bool operator<(const Double& other) const;
    Bool operator<=(const Double& other) const;
    Bool operator>(const Double& other) const;
    Bool operator>=(const Double& other) const;
    
    // 算术运算符重载
    Double operator+(const Double& other) const;
    Double operator-(const Double& other) const;
    Double operator*(const Double& other) const;
    Double operator/(const Double& other) const;
    
    // 一元运算符重载
    Double operator+() const;
    Double operator-() const;
    
    // 一元运算方法
    double unaryPlus() const;
    double unaryMinus() const;
};

class Integer : public NumericValue {
private:
    int value;  // 使用 int 类型存储
public:
    Integer(int val = 0, ObjectType* vt = nullptr);
    Integer(const Integer& other);
    // 实现虚函数
    TypeRank getTypeRank() const override;
    Value* promoteTo(TypeRank targetRank) const override;
    // 重写基类方法
    std::string toString() const override;
    bool toBool() const override;
    Value* clone() const override;
    std::string getBuiltinTypeName() const override;
    // 获取和设置值
    int getValue() const;
    void setValue(int val);

    // 赋值运算符重载
    Integer& operator=(const Integer& other);

    // 比较运算符重载
    Bool operator==(const Integer& other) const;
    Bool operator!=(const Integer& other) const;
    Bool operator<(const Integer& other) const;
    Bool operator<=(const Integer& other) const;
    Bool operator>(const Integer& other) const;
    Bool operator>=(const Integer& other) const;

    // 算术运算符重载
    Integer operator+(const Integer& other) const;
    Integer operator-(const Integer& other) const;
    Integer operator*(const Integer& other) const;
    Integer operator/(const Integer& other) const;
    Integer operator%(const Integer& other) const;

    // 位运算运算符重载
    Integer operator&(const Integer& other) const;
    Integer operator|(const Integer& other) const;
    Integer operator^(const Integer& other) const;
    Integer operator<<(const Integer& other) const;
    Integer operator>>(const Integer& other) const;
    Integer operator~() const;

    // 一元运算符重载
    Integer operator+() const;
    Integer operator-() const;
    Bool operator!() const;  // 逻辑非运算符重载

    // 自增自减运算符重载
    Integer& operator++();
    Integer operator++(int);
    Integer& operator--();
    Integer operator--(int);

    // 一元运算方法
    int unaryPlus() const;
    int unaryMinus() const;
    int bitwiseNot() const;
};

class Char : public NumericValue {
private:
    char value;  // 使用 char 类型存储
public:
    Char(char val = '\0', ObjectType* vt = nullptr);
    Char(const Char& other);
    // 实现虚函数
    TypeRank getTypeRank() const override;
    Value* promoteTo(TypeRank targetRank) const override;
    // 重写基类方法
    std::string toString() const override;
    bool toBool() const override;
    Value* clone() const override;
    std::string getBuiltinTypeName() const override;
    // 获取和设置值
    char getValue() const;
    void setValue(char val);

    // 赋值运算符重载
    Char& operator=(const Char& other);

    // 比较运算符重载
    Bool operator==(const Char& other) const;
    Bool operator!=(const Char& other) const;
    Bool operator<(const Char& other) const;
    Bool operator<=(const Char& other) const;
    Bool operator>(const Char& other) const;
    Bool operator>=(const Char& other) const;
    
    // 算术运算符重载
    Char operator+(const Char& other) const;
    Char operator-(const Char& other) const;
    Char operator*(const Char& other) const;
    Char operator/(const Char& other) const;
    Char operator%(const Char& other) const;
    
    // 位运算运算符重载
    Char operator&(const Char& other) const;
    Char operator|(const Char& other) const;
    Char operator^(const Char& other) const;
    Char operator<<(const Char& other) const;
    Char operator>>(const Char& other) const;
    Char operator~() const;
    
    // 一元运算符重载
    Char operator+() const;
    Char operator-() const;
    Bool operator!() const;  // 逻辑非运算符重载
    
    // 一元运算方法
    char unaryPlus() const;
    char unaryMinus() const;
};

// ==================== 字符串值类型 ====================
class String : public PrimitiveValue {
private:
    std::string value;
public:
    String(const std::string& val = "", ObjectType* vt = nullptr);
    String(const String& other);
    // 赋值运算符重载
    String& operator=(const String& other);
    // 算术运算符重载
    String operator+(const String& other) const;  // 字符串连接
    
    // 比较运算符重载
    Bool operator==(const String& other) const;
    Bool operator!=(const String& other) const;
    Bool operator<(const String& other) const;
    Bool operator<=(const String& other) const;
    Bool operator>(const String& other) const;
    Bool operator>=(const String& other) const;
    const std::string& getValue() const;
    void setValue(const std::string& val);
    // 重写基类方法
    std::string toString() const override;
    bool toBool() const override;
    Value* clone() const override;
    std::string getBuiltinTypeName() const override;

    // 一元运算方法
    std::string unaryPlus() const;  // 一元加号，返回字符串本身
    std::string unaryMinus() const;  // 一元减号，返回负号前缀的字符串
    bool logicalNot() const;  // 逻辑非，空字符串为false
    // 字符串函数
    size_t length() const;  // 字符串长度
    bool isEmpty() const;  // 是否为空
    String* substring(int start, int end) const;  // 子字符串
    String* toUpperCase() const;  // 转换为大写
    String* toLowerCase() const;  // 转换为小写
    String* trim() const;  // 去除首尾空格
    vector<String*> split(const String& delimiter) const;  // 分割字符串
    bool contains(const String& substr) const;  // 是否包含子串
    bool startsWith(const String& prefix) const;  // 是否以指定前缀开始
    bool endsWith(const String& suffix) const;  // 是否以指定后缀结束
    int indexOf(const String& substr) const;  // 子串首次出现位置
    int lastIndexOf(const String& substr) const;  // 子串最后出现位置
    String* replace(const String& oldStr, const String& newStr) const;  // 替换字符串
    void append(const String& other);  // 追加字符串
    void append(const std::string& other);  // 追加std::string
    void append(char ch);  // 追加单个字符
    // 下标运算符重载
    Char operator[](const Integer& index) const;  // 下标访问
    
    // 字符访问（模拟数组行为，但不是继承）
    Value* getChar(int index) const;  // 获取指定位置的字符
    void setChar(int index, char ch);  // 设置指定位置的字符
    bool isValidIndex(int index) const;  // 检查索引是否有效
    // 迭代器支持（模拟容器行为）
    class StringIterator {
    private:
        const String* str;
        size_t pos;
    
    public:
        StringIterator(const String* s, size_t p);
        char operator*() const;
        StringIterator& operator++();
        StringIterator& operator--();
        bool operator!=(const StringIterator& other) const;
    };

    // 迭代器方法
    StringIterator begin() const;
    StringIterator end() const;
};

// ==================== 容器值基类 ====================
class ContainerValue : public Value {
protected:
    // 容器类型的通用属性
    bool isMutable;
    bool isOrdered;
public:
    ContainerValue(ObjectType* vt = nullptr, bool mutable_ = true, bool ordered = true);
    virtual ~ContainerValue() = default;
    // 容器通用方法
    virtual size_t getSize() const = 0;
    virtual bool isEmpty() const = 0;
    virtual void clear() = 0;
    // 容器属性
    bool isMutableContainer() const;
    bool isOrderedContainer() const;
};

// ==================== 数组值类型 ====================
class Array : public ContainerValue {
private:
    std::vector<Value*> elements;
    ObjectType* elementType;
public:
    Array(ObjectType* et = nullptr, ObjectType* vt = nullptr);
    Array(const std::vector<Value*>& elems, ObjectType* et = nullptr, ObjectType* vt = nullptr);
    Array(const Array& other);
    ~Array();
    // 赋值运算符重载
    Array operator=(const Array& other);
    // 容器操作实现
    size_t getSize() const override;
    bool isEmpty() const override;
    void clear() override;
    // 容器类型支持
    bool supportsElementType() const;
    ObjectType* getElementType() const;
    // 迭代器支持
    Value* begin() const;
    Value* end() const;
    Value* next(Value* iterator) const;
    // 容量管理
    void reserve(size_t capacity);
    size_t getCapacity() const;
    // 数组特有操作
    void addElement(Value* element);
    void setElement(size_t index, Value* element);
    Value* getElement(size_t index) const;
    const std::vector<Value*>& getElements() const;
    std::vector<Value*>& getElements();
    void setElementType(ObjectType* et);
    void removeElement(size_t index);
    void insertElement(size_t index, Value* element);

    // 比较运算符重载
    Bool operator==(const Array& other) const;
    Bool operator!=(const Array& other) const;
    Bool operator<(const Array& other) const;
    Bool operator<=(const Array& other) const;
    Bool operator>(const Array& other) const;
    Bool operator>=(const Array& other) const;
    // 下标运算符重载
    Value* operator[](const Integer& index) const;
    // 重写基类方法
    std::string toString() const override;
    bool toBool() const override;
    Value* clone() const override;
    std::string getBuiltinTypeName() const override;
    // 数组函数
    Value* sort() const;
    Value* reverse() const;
    Value* slice(int start, int end) const;
    Value* filter(Value* predicate) const;
    Value* map(Value* mapper) const;
    Value* indexOf(Value* element) const;  // 查找元素在数组中的位置
    Value* contains(Value* element) const;  // 检查数组是否包含指定元素

};

// ==================== 字典值类型 ====================
class Dict : public ContainerValue {
private:
    std::map<std::string, Value*> entries;
    ObjectType* keyType;
    ObjectType* valueType;
public:
    Dict(ObjectType* kt = nullptr, ObjectType* vt = nullptr);
    Dict(const std::map<std::string, Value*>& ents, ObjectType* kt = nullptr, ObjectType* vt = nullptr);
    Dict(const Dict& other);
    ~Dict();
    // 赋值运算符重载
    Dict operator=(const Dict& other);
    // 容器操作实现
    size_t getSize() const override;
    bool isEmpty() const override;
    void clear() override;
    // 容器类型支持
    bool supportsElementType() const;
    ObjectType* getElementType() const;
    // 迭代器支持
    Value* begin() const;
    Value* end() const;
    Value* next(Value* iterator) const;
    // 容量管理
    void reserve(size_t capacity);  // 字典通常不需要预分配
    size_t getCapacity() const;     // 字典通常不需要容量概念
    // 字典特有操作
    void setEntry(const std::string& key, Value* value);
    Value* getEntry(const std::string& key) const;
    void removeEntry(const std::string& key);
    bool hasKey(const std::string& key) const;

    // 比较运算符重载
    Bool operator==(const Dict& other) const;
    Bool operator!=(const Dict& other) const;
    Bool operator<(const Dict& other) const;
    Bool operator<=(const Dict& other) const;
    Bool operator>(const Dict& other) const;
    Bool operator>=(const Dict& other) const;
    // 重写基类方法
    std::string toString() const override;
    bool toBool() const override;
    Value* clone() const override;
    std::string getBuiltinTypeName() const override;
    // 字典函数
    Value* keys() const;
    Value* values() const;
    Value* items() const;
    Value* get(const std::string& key, Value* defaultValue = nullptr) const;
    void update(const Dict& other);

    // 获取键和值的类型
    ObjectType* getKeyType() const;
    ObjectType* getValueType() const;

    // 获取键列表和值列表
    std::vector<std::string> getKeys() const;
    std::vector<Value*> getValues() const;

    // 获取条目引用
    const std::map<std::string, Value*>& getEntries() const;
};

// ==================== 对象值类型 ====================
class ObjectValue : public Value {
private:
    std::string className;
    std::map<std::string, Value*> properties;
    std::map<std::string, class Function*> methods;
public:
    ObjectValue(const std::string& name, ObjectType* vt = nullptr);
    ObjectValue(const ObjectValue& other);
    ~ObjectValue();
    // 赋值运算符重载
    ObjectValue operator=(const ObjectValue& other);
    // 对象特有操作
    void setProperty(const std::string& name, Value* value);
    Value* getProperty(const std::string& name) const;
    void removeProperty(const std::string& name);
    bool hasProperty(const std::string& name) const;
    void setMethod(const std::string& name, class Function* method);
    class Function* getMethod(const std::string& name) const;
    void removeMethod(const std::string& name);
    bool hasMethod(const std::string& name) const;
    // 重写基类方法
    std::string toString() const override;
    bool toBool() const override;
    Value* clone() const override;
    std::string getBuiltinTypeName() const override;
    // 对象特有方法
    std::string getClassName() const;
    bool isInstance() const override;
    // 获取属性引用
    const std::map<std::string, Value*>& getProperties() const;
    const std::map<std::string, class Function*>& getMethods() const;
};

// ==================== 函数基类 ====================
class Function : public Value {
protected:
    std::string name;
    std::vector<std::string> parameters;
public:
    Function(const std::string& funcName, const std::vector<std::string>& params = {});
    virtual ~Function() = default;
    std::string getName() const;
    const std::vector<std::string>& getParameters() const;
    // 获取参数列表（用于参数绑定）
    virtual const std::vector<std::string>& getParameterList() const;
    // 纯虚函数：函数调用
    virtual Value* call(std::vector<Value*> args) = 0;
    // 新增：参数验证
    bool validateArguments(const std::vector<Value*>& args) const;
    // 新增：获取函数签名
    std::string getSignature() const;
    // 重写类型检查方法
    bool isCallable() const override;
    std::string toString() const override;
    bool toBool() const override;
    std::string getBuiltinTypeName() const override;
    // 获取函数类型信息
    virtual std::string getFunctionType() const = 0;
};

// ==================== 内置函数类型 ====================
class BuiltinFunction : public Function {
private:
    std::function<Value*(std::vector<Value*>)> func;
public:
    BuiltinFunction(const std::string& funcName, 
                    std::function<Value*(std::vector<Value*>)> f,
                    const std::vector<std::string>& params = {});
    
    // 新增：接受函数指针的构造函数
    BuiltinFunction(const std::string& funcName, 
                    Value* (*f)(std::vector<Value*>&),
                    const std::vector<std::string>& params = {});
    
    // 函数调用
    Value* call(std::vector<Value*> args) override;
    // 获取函数类型信息
    std::string getFunctionType() const override;
    // 重写clone方法
    Value* clone() const override;
    // 检查是否为内置函数
    bool isBuiltinFunction() const;
};

// ==================== 方法引用类型 ====================
class MethodValue : public Function {
private:
    ObjectType* targetType;      // 目标对象的类型
    Value* target;               // 目标对象实例
    std::string methodName;      // 方法名称
public:
    MethodValue(ObjectType* type, Value* instance, const std::string& name);
    virtual ~MethodValue() = default;
    
    // 获取目标类型
    ObjectType* getTargetType() const;
    // 获取目标实例
    Value* getTarget() const;
    // 获取方法名称
    std::string getMethodName() const;
    
    // 函数调用 - 直接调用类型系统的方法
    Value* call(std::vector<Value*> args) override;
    // 获取函数类型信息
    std::string getFunctionType() const override;
    // 重写getTypeName方法
    std::string getTypeName() const;
    // 重写clone方法
    Value* clone() const override;
    // 重写toString方法
    std::string toString() const override;
};

// ==================== 类方法引用类型 ====================
class ClassMethodValue : public Function {
private:
    ObjectType* targetType;      // 目标类型
    std::string methodName;      // 方法名称
public:
    ClassMethodValue(ObjectType* type, const std::string& name);
    virtual ~ClassMethodValue() = default;
    
    // 获取目标类型
    ObjectType* getTargetType() const;
    // 获取方法名称
    std::string getMethodName() const;
    
    // 函数调用 - 调用类型的类方法（不需要实例）
    Value* call(std::vector<Value*> args) override;
    // 获取函数类型信息
    std::string getFunctionType() const override;
    // 重写getTypeName方法
    std::string getTypeName() const;
    // 重写clone方法
    Value* clone() const override;
    // 重写toString方法
    std::string toString() const override;
};

// ==================== 用户函数类型 ====================
class UserFunction : public Function {
private:
    class Statement* functionBody;        // 函数体语句
    class FunctionExecutor* executor;     // 函数执行器（依赖注入）
public:
    UserFunction(const std::string& funcName, 
                 const std::vector<std::string>& params,
                 class Statement* body = nullptr);
    // 函数调用
    Value* call(std::vector<Value*> args) override;
    // 获取函数类型信息
    std::string getFunctionType() const override;
    // 重写clone方法
    Value* clone() const override;
    // 获取函数体
    class Statement* getFunctionBody() const;
    // 设置函数体
    void setFunctionBody(class Statement* body);
    // 获取执行器
    class FunctionExecutor* getExecutor() const;
    // 设置执行器
    void setExecutor(class FunctionExecutor* exec);
    // 检查是否为内置函数
    bool isBuiltinFunction() const;
};

// ==================== FunctionSignature类 ====================
// 表示函数的签名信息，包括函数名和参数类型列表
class FunctionSignature {
private:
    std::string name;
    std::vector<std::string> parameterTypes;  // 参数类型名称列表
public:
    // 构造函数
    FunctionSignature(const std::string& n, const std::vector<std::string>& types = {});
    // 从FunctionPrototype构造的构造函数
    FunctionSignature(const FunctionPrototype* prototype);
    // 从FunctionDefinition构造的构造函数
    FunctionSignature(const FunctionDefinition* funcDef);
    // 拷贝构造函数
    FunctionSignature(const FunctionSignature& other);
    // 赋值运算符
    FunctionSignature& operator=(const FunctionSignature& other);
    // 析构函数
    ~FunctionSignature() = default;
    // 获取函数名
    std::string getName() const;
    // 获取参数类型列表
    const std::vector<std::string>& getParameterTypes() const;
    // 获取参数数量
    size_t getParameterCount() const;
    // 比较函数签名是否匹配
    bool matches(const std::string& methodName, const std::vector<Value*>& args) const;
    // 检查是否与另一个签名匹配（用于重载解析）
    bool matches(const FunctionSignature& other) const;
    // 获取签名字符串表示
    std::string toString() const;
    // 比较两个签名是否相等
    bool operator==(const FunctionSignature& other) const;
    // 比较两个签名的大小（用于排序）
    bool operator<(const FunctionSignature& other) const;
    // 检查是否为构造函数（函数名与类型名相同）
    bool isConstructor() const;
    // 检查是否为默认构造函数（无参数）
    bool isDefaultConstructor() const;
    // 检查是否为拷贝构造函数
    bool isCopyConstructor() const;
    // 检查是否为移动构造函数
    bool isMoveConstructor() const;
    // 检查是否为析构函数
    bool isDestructor() const;
    // 获取函数类型（普通函数、构造函数、析构函数等）
    std::string getFunctionType() const;
    // 打印函数签名信息
    void print() const;
    // ==================== 静态工厂方法 ====================
    // 从函数原型创建函数签名
    static FunctionSignature fromPrototype(const FunctionPrototype* prototype);
    // 从函数定义创建函数签名
    static FunctionSignature fromDefinition(const FunctionDefinition* funcDef);
    // 从函数名和参数类型列表创建函数签名
    static FunctionSignature fromTypes(const std::string& funcName, const std::vector<std::string>& paramTypes);
    // 从函数名和参数类型字符串创建函数签名（用于解析）
    static FunctionSignature fromString(const std::string& signature);
private:
    // 辅助函数：根据类型名称获取类型对象
    class ObjectType* getTypeByName(const std::string& typeName) const;
};

// 函数签名哈希函数（用于unordered_map）
// 对象注册表 - 管理当前作用域中的所有对象
class ObjectRegistry {
private:
    // 变量对象映射：标识符 -> 值对象
    std::map<std::string, Value*> variableObjects;
    // 可调用对象映射：标识符 -> 可调用对象
    std::map<std::string, Value*> callableObjects;
    // 实例对象映射：标识符 -> 实例对象
    std::map<std::string, Value*> instanceObjects;
public:
    ObjectRegistry();
    ~ObjectRegistry();
    // ==================== 变量对象管理 ====================
    void defineVariable(const std::string& name, Value* value);
    Value* lookupVariable(const std::string& name) const;
    bool hasVariable(const std::string& name) const;
    // ==================== 可调用对象管理 ====================
    void defineCallable(const std::string& name, Value* callable);
    Value* lookupCallable(const std::string& name) const;
    bool hasCallable(const std::string& name) const;
    // 获取所有可调用对象（用于函数签名匹配）
    const std::map<std::string, Value*>& getCallableObjects() const { return callableObjects; }
    // ==================== 实例对象管理 ====================
    void defineInstance(const std::string& name, Value* instance);
    Value* lookupInstance(const std::string& name) const;
    bool hasInstance(const std::string& name) const;
    // ==================== 统一对象查找 ====================
    // 查找任何类型的对象
    std::tuple<Value*, Value*, Value*> lookupObject(const std::string& name) const;
    // 检查对象是否存在于当前作用域
    bool hasObject(const std::string& name) const;
    // ==================== 资源管理 ====================
    void cleanup();
    // 打印对象注册表内容
    void print() const;
    // ==================== 获取器方法 ====================
    const std::map<std::string, Value*>& getVariableObjects() const { return variableObjects; }
    const std::map<std::string, Value*>& getInstanceObjects() const { return instanceObjects; }
    // 获取对象数量统计
    size_t getVariableCount() const { return variableObjects.size(); }
    size_t getCallableCount() const { return callableObjects.size(); }
    size_t getInstanceCount() const { return instanceObjects.size(); }
    size_t getTotalObjectCount() const { return variableObjects.size() + callableObjects.size() + instanceObjects.size(); }
};

// ==================== ObjectFactory类 ====================
// 用于创建指定Object子类型的Value对象的工厂类
class ObjectFactory {
private:
    bool initialized;
    class TypeRegistry* typeRegistry;
    // 初始化工厂映射
    void initializeCreators();
public:
    // 构造函数和析构函数
    ObjectFactory();
    ObjectFactory(class TypeRegistry* registry);  // 简化构造函数，只需要TypeRegistry
    ~ObjectFactory();
    // 根据ObjectType类型创建对应的Value对象
    Value* createValue(class ObjectType* type);
    // 根据类型名称创建对应的Value对象
    Value* createValue(const std::string& typeName);
    // 根据ObjectType类型创建默认值
    Value* createDefaultValue(class ObjectType* type);
    // 根据类型名称创建默认值
    Value* createDefaultValue(const std::string& typeName);
    // 创建null值
    Value* createNull();
    // 创建指定类型的null值（用于类型转换）
    Value* createNullForType(class ObjectType* type);
    // 创建指定类型的null值（用于类型转换）
    Value* createNullForType(const std::string& typeName);
    // 检查类型是否支持null值
    bool supportsNull(class ObjectType* type);
    // 检查类型名称是否支持null值
    bool supportsNull(const std::string& typeName);
    // ==================== 基本类型创建方法 ====================
    // 创建基本类型的Value对象
    Bool* createBool(bool value = false);
    Integer* createInteger(int value = 0);
    Double* createDouble(double value = 0.0);
    Char* createChar(char value = '\0');
    String* createString(const std::string& value = "");
    // 创建复合类型的Value对象
    Array* createArray(const std::vector<Value*>& elements = {});
    Dict* createDict(const std::vector<std::pair<std::string, Value*>>& pairs = {});
    // 通用创建方法 - 根据类型名称和参数创建Value对象
    Value* create(const std::string& typeName, const std::vector<Value*>& args = {});
    // 从其他Value创建指定类型的Value（类型转换）
    Value* createFromValue(Value* source, const std::string& targetType);

    // 批量创建方法
    std::vector<Value*> createMultiple(const std::string& typeName, int count, const std::vector<Value*>& args = {});
    // 静态便捷方法（保持向后兼容）
    static Value* createValueStatic(class ObjectType* type);
    static Value* createValueStatic(const std::string& typeName);
    static Value* createDefaultValueStatic(class ObjectType* type);
    static Value* createDefaultValueStatic(const std::string& typeName);
    static Value* createNullStatic();
    static Value* createNullForTypeStatic(class ObjectType* type);
    static Value* createNullForTypeStatic(const std::string& typeName);
    static bool supportsNullStatic(class ObjectType* type);
    static bool supportsNullStatic(const std::string& typeName);
    // 静态基本类型创建方法
    static Bool* createBoolStatic(bool value = false);
    static Integer* createIntegerStatic(int value = 0);
    static Double* createDoubleStatic(double value = 0.0);
    static Char* createCharStatic(char value = '\0');
    static String* createStringStatic(const std::string& value = "");
    static Array* createArrayStatic(const std::vector<Value*>& elements = {});
    static Dict* createDictStatic(const std::vector<std::pair<std::string, Value*>>& pairs = {});
    static Value* createStatic(const std::string& typeName, const std::vector<Value*>& args = {});
    static Value* createFromValueStatic(Value* source, const std::string& targetType);
    // ==================== 结构体和类实例化方法 ====================
    // 结构体实例化（需要先通过类型系统获取StructType）
    Value* createStructInstance(class StructType* structType, const std::vector<Value*>& args);
    // 已废弃：类型查找应该由类型系统负责
    Value* createStructInstance(const std::string& structTypeName, const std::vector<Value*>& args);
    // 类实例化（需要先通过类型系统获取ClassType）
    Value* createClassInstance(class ClassType* classType, const std::vector<Value*>& args);
    // 已废弃：类型查找应该由类型系统负责
    Value* createClassInstance(const std::string& classTypeName, const std::vector<Value*>& args);
    // 静态便捷方法实现
    static Value* createStructInstanceStatic(class StructType* structType, const std::vector<Value*>& args);
    static Value* createStructInstanceStatic(const std::string& structTypeName, const std::vector<Value*>& args);
    static Value* createClassInstanceStatic(class ClassType* classType, const std::vector<Value*>& args);
    static Value* createClassInstanceStatic(const std::string& classTypeName, const std::vector<Value*>& args);
    // ==================== 依赖注入实现 ====================
    void setTypeRegistry(class TypeRegistry* registry);
    // ==================== 通用实例化辅助方法实现 ====================
    void initializeFromDict(Dict* instance, class ClassType* type, Dict* memberDict);
    void initializeFromArgs(Dict* instance, class ClassType* type, const std::vector<Value*>& args);
    Value* createMemberDefaultValue(class ClassType* type, const std::string& memberName);
    void initializeMethods(Dict* instance, class ClassType* type);
    // ==================== 函数创建方法 ====================
    // 创建内置函数
    Value* createBuiltinFunction(const std::string& name, 
                                std::function<Value*(std::vector<Value*>&)> func,
                                const std::vector<std::string>& params = {});
    // 创建用户函数（从AST定义）
    Value* createUserFunction(const std::string& name,
                             void* funcDef,  // FunctionDefinition*
                             const std::vector<std::string>& params = {});
    // 创建用户函数（从参数列表）
    Value* createUserFunction(const std::string& name,
                             const std::vector<std::string>& params,
                             void* funcDef = nullptr);

    // 从函数定义创建可调用对象
    Value* createCallableFromDefinition(const std::string& name,
                                       void* funcDef,
                                       const std::vector<std::string>& params = {});
    // 静态便捷方法
    static Value* createBuiltinFunctionStatic(const std::string& name,
                                            std::function<Value*(std::vector<Value*>&)> func,
                                            const std::vector<std::string>& params = {});
    static Value* createUserFunctionStatic(const std::string& name,
                                         void* funcDef,
                                         const std::vector<std::string>& params = {});
    static Value* createCallableFromDefinitionStatic(const std::string& name,
                                                   void* funcDef,
                                                   const std::vector<std::string>& params = {});
};

// ==================== 全局工厂方法（向后兼容） ====================
// 全局工厂方法：根据ObjectType*类型创建默认值
Value* createDefaultValue(class ObjectType* type);
// 全局工厂方法：根据类型名称创建默认值
Value* createDefaultValue(const std::string& typeName);
// 全局访问函数
ObjectFactory* getObjectFactory();

#endif // INTERPRETER_VALUE_H
