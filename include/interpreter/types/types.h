#ifndef BUILTIN_TYPES_H
#define BUILTIN_TYPES_H

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <stdexcept>
#include <mutex>
#include <iostream>
#include <sstream>
#include <type_traits>
#include <algorithm>
#include <cctype>
#include "interpreter/utils/logger.h"
#include "parser/definition.h"  // 包含FunctionPrototype和FunctionDefinition定义
#include "interpreter/values/value.h"  // 包含FunctionSignature完整定义

using namespace std;

// 前向声明
class Interpreter;
class TypeRegistry;
class FunctionPrototype;
class UserFunction;

// 前向声明
class Value;
class FunctionSignature;
class ObjectType;
class StructType;
class ClassType;
class Interface;

// 前向声明
class TypeRegistry;

// ==================== 类型系统基类 ====================
// 所有类型的基类
class ObjectType {
protected:
    string typeName;
    bool isPrimitive;
    bool isMutable;
    bool isReference;
    bool isContainer;
    bool isUserDefined;
    
    // 继承关系
    ObjectType* parentType;
    vector<ObjectType*> interfaces;
    
    // 带函数签名的方法映射
    std::map<FunctionSignature, std::function<Value*(Value*, std::vector<Value*>&)>> methods;
    
    // 静态方法映射（不需要实例）
    std::map<FunctionSignature, std::function<Value*(std::vector<Value*>&)>> staticMethods;

public:
    ObjectType(const string& name, bool primitive = false, bool mutable_ = true, 
           bool reference = false, bool container = false, bool userDefined = false,
           ObjectType* parent = nullptr);
    
    virtual ~ObjectType() = default;
    
    // 类型信息
    string getTypeName() const;
    bool isPrimitiveType() const;
    bool isMutableType() const;
    bool isReferenceType() const;
    bool isContainerType() const;
    bool isUserDefinedType() const;
    
    // 继承关系管理
    ObjectType* getParentType() const;
    void setParentType(ObjectType* parent);
    const vector<ObjectType*>& getInterfaces() const;
    void addInterface(ObjectType* interface);
    
    // 继承关系检查
    bool isSubtypeOf(ObjectType* other) const;
    bool implements(ObjectType* interface) const;
    
    // 方法管理 - 支持函数签名
    void registerMethod(const string& name, function<Value*(Value*, vector<Value*>&)> method);
    void registerMethod(const string& name, function<Value*(Value*, vector<Value*>&)> method, const vector<string>& parameterTypes);
    void registerMethod(const FunctionSignature& signature, 
                       function<Value*(Value*, vector<Value*>&)> method);
    void registerMethod(const FunctionPrototype* prototype, 
                       function<Value*(Value*, vector<Value*>&)> method);
    
    // 静态方法管理 - 不需要实例
    void registerStaticMethod(const string& name, function<Value*(vector<Value*>&)> method);
    void registerStaticMethod(const string& name, function<Value*(vector<Value*>&)> method, const vector<string>& parameterTypes);
    void registerStaticMethod(const FunctionSignature& signature, 
                             function<Value*(vector<Value*>&)> method);
    void registerStaticMethod(const FunctionPrototype* prototype, 
                             function<Value*(vector<Value*>&)> method);
    
    // 注册构造函数（使用类型名称作为方法名）
    void registerConstructor(const vector<string>& parameterTypes = {}, 
                           function<Value*(Value*, vector<Value*>&)> constructor = nullptr);
    void registerConstructor(const FunctionSignature& signature, 
                           function<Value*(Value*, vector<Value*>&)> constructor = nullptr);
    void registerConstructor(const FunctionPrototype* prototype, 
                           function<Value*(Value*, vector<Value*>&)> constructor = nullptr);
    
    // 获取所有方法
    const map<FunctionSignature, function<Value*(Value*, vector<Value*>&)>>& getMethods() const;
    
    // 获取所有静态方法
    const map<FunctionSignature, function<Value*(vector<Value*>&)>>& getStaticMethods() const;
    
    // 检查是否有指定方法（支持参数匹配）
    bool hasMethod(const string& methodName, const vector<Value*>& args = {}) const;
    bool hasMethodName(const string& methodName) const;
    
    // 检查是否有指定静态方法（支持参数匹配）
    bool hasStaticMethod(const string& methodName, const vector<Value*>& args = {}) const;
    bool hasStaticMethodName(const string& methodName) const;
    
    // 调用方法
    virtual Value* callMethod(Value* instance, const string& methodName, vector<Value*>& args);
    
    // 调用静态方法
    virtual Value* callStaticMethod(const string& methodName, vector<Value*>& args);
    
    // 类型转换
    virtual Value* convertTo(ObjectType* targetType, Value* value);
    
    // 类型兼容性检查
    virtual bool isCompatibleWith(ObjectType* other) const;
    
    // 创建默认值
    virtual Value* createDefaultValue();
    
    // 创建带参数的值
    virtual Value* createValueWithArgs(const vector<Value*>& args);
    
    // 实例管理
    virtual void bindMethodsToInstance(Value* instance);
    virtual void initializeInstance(Value* instance, vector<Value*>& args);
    
    // 成员访问
    virtual Value* accessMember(Value* instance, const string& memberName);
    virtual bool hasMember(const string& memberName) const;
    
    // 自动注册方法
    void registerConstructor();
    
    // 成员访问方法注册
    void registerMemberAccess();
    
    // 类型描述
    string getTypeDescription() const;
    
    // 类型比较
    bool operator==(const ObjectType& other) const;
    bool operator!=(const ObjectType& other) const;
};

// ==================== 访问修饰符枚举 ====================
enum VisibilityType {
    VIS_PUBLIC,
    VIS_PRIVATE,
    VIS_PROTECTED
};

// ==================== 容器类型 ====================
// 容器类型基类：数组、字典等
class ContainerType : public ObjectType {
public:
    ContainerType(const string& typeName, bool isPrimitive = false, bool isMutable = true, bool isReference = false);
    
    // 容器类型都支持访问操作
    virtual bool supportsAccess() const;
    
    // 容器类型都支持迭代
    virtual bool supportsIteration() const;
    
    // 容器类型都支持大小查询
    virtual bool supportsSizeQuery() const;
    
    // 容器类型的兼容性检查：容器类型只与自身兼容
    bool isCompatibleWith(ObjectType* other) const override;
    
protected:
    // 自动注册下标访问方法
    void registerSubscriptAccess();
    
    // 创建带参数的值的虚方法（子类可以重写）
    virtual Value* createValueWithArgs(const vector<Value*>& args);
};

// ==================== 接口类型 ====================
// 通用接口类型，只包含函数原型
class Interface : public ObjectType {
private:
    map<string, function<Value*(Value*, vector<Value*>&)>> methodPrototypes;  // 方法原型

public:
    Interface(const string& name);
    
    // 添加方法原型（不包含实现）
    void addMethodPrototype(const string& name, function<Value*(Value*, vector<Value*>&)> prototype);
    
    // 获取方法原型
    function<Value*(Value*, vector<Value*>&)> getMethodPrototype(const string& name) const;
    
    // 检查是否包含方法原型
    bool hasMethodPrototype(const string& name) const;
    
    // 获取所有方法原型名称
    vector<string> getMethodPrototypeNames() const;
    
    // 使用基类的默认convertTo实现
    bool isCompatibleWith(ObjectType* other) const override;
    
    // 接口类型不能创建实例
    Value* createDefaultValue() override;
};

// ==================== 类类型 ====================
// 用户定义的类类型
class ClassType : public ObjectType {
protected:
    // 成员类型映射
    map<string, ObjectType*> memberTypes;
    map<string, VisibilityType> memberVisibility;
    
    // 静态成员
    map<string, ObjectType*> staticMemberTypes;
    map<string, Value*> staticMemberValues;
    map<string, VisibilityType> staticMemberVisibility;
    
    // 用户定义方法
    map<string, UserFunction*> userFunctionMethods;
    map<string, UserFunction*> staticMethods;
    
    // 成员初始值
    map<string, Value*> memberInitialValues;
    
    // 默认构造函数
    function<Value*(vector<Value*>&)> defaultConstructor;

public:
    ClassType(const string& name, bool isPrimitive = false, bool isMutable = true, bool isReference = false);
    
    // 成员管理
    virtual void addMember(const string& name, ObjectType* type, VisibilityType visibility = VIS_PUBLIC, Value* initialValue = nullptr);
    void addStaticMember(const string& name, ObjectType* type, Value* value, VisibilityType visibility = VIS_PUBLIC);
    
    // 用户方法管理
    void addUserMethod(const string& name, UserFunction* userFunc, VisibilityType visibility = VIS_PUBLIC);
    void addUserMethod(const FunctionPrototype* prototype, UserFunction* userFunc);
    void addStaticMethod(const string& name, UserFunction* userFunc, VisibilityType visibility = VIS_PUBLIC);
    void addStaticMethod(const FunctionPrototype* prototype, UserFunction* userFunc);
    
    // 成员初始值管理
    void setMemberInitialValue(const string& memberName, Value* initialValue);
    Value* getMemberInitialValue(const string& memberName) const;
    void setAllMemberInitialValues(const map<string, Value*>& initialValues);
    const map<string, Value*>& getAllMemberInitialValues() const;
    bool hasMemberInitialValue(const string& memberName) const;
    
    // 获取所有方法（包括继承的方法）
    map<string, function<Value*(Value*, vector<Value*>&)>> getMethods() const;
    
    // 构造函数管理
    void setDefaultConstructor(function<Value*(vector<Value*>&)> constructor);
    function<Value*(vector<Value*>&)> getDefaultConstructor() const;
    bool hasDefaultConstructor() const;
    
    // 类型转换
    Value* convertTo(ObjectType* targetType, Value* value) override;
    
    // 类型兼容性检查
    bool isCompatibleWith(ObjectType* other) const override;
    
    // 创建默认值
    Value* createDefaultValue() override;
    
    // 成员访问方法注册
    void registerMemberAccessMethod(const string& memberName, ObjectType* memberType);
    
    // 成员查询
    ObjectType* getMemberType(const string& name) const;
    bool hasMember(const string& name) const;
    vector<string> getMemberNames() const;
    map<string, ObjectType*> getMemberTypes() const;
    const map<string, UserFunction*>& getUserFunctionMethods() const;
    
    // 静态成员查询
    Value* getStaticMemberValue(const string& name) const;
    void setStaticMemberValue(const string& name, Value* value);
    
    // 获取静态方法映射
    const map<string, UserFunction*>& getStaticMethods() const;
    
    // 获取静态成员类型映射
    const map<string, ObjectType*>& getStaticMemberTypes() const;
    
    // 获取静态成员值映射
    const map<string, Value*>& getStaticMembers() const;
};

// ==================== 结构体类型 ====================
// 结构体类型：本质上是方法成员全部公有的ClassType
class StructType : public ClassType {
public:
    StructType(const string& name);
    
    // 重写addMember方法，强制所有成员为公有
    void addMember(const string& name, ObjectType* type, VisibilityType visibility = VIS_PUBLIC, Value* initialValue = nullptr) override;
    
    // 重写registerMethod方法，强制所有方法为公有
    void registerMethod(const string& name, function<Value*(Value*, vector<Value*>&)> method);
};

// ==================== 内置类型类定义 ====================
// 这些类型类与Value类一一对应，具有相同的继承关系

// 基本类型基类
class PrimitiveType : public ObjectType {
public:
    PrimitiveType(const string& name, bool mutable_ = true) 
        : ObjectType(name, true, mutable_, false) {}
};

// 数值类型基类
class NumericType : public PrimitiveType {
public:
    NumericType(const string& name, bool mutable_ = true) 
        : PrimitiveType(name, mutable_) {}
};

// 基础内置类型（基本类型和数值类型）
class BoolType : public PrimitiveType {
public:
    BoolType();
};

class NullType : public PrimitiveType {
public:
    NullType();
};

class CharType : public NumericType {
public:
    CharType();
};

class IntegerType : public NumericType {
public:
    IntegerType();
};

class DoubleType : public NumericType {
public:
    DoubleType();
};

// 容器类型
class StringType : public ContainerType {
public:
    StringType();
};

class ArrayType : public ContainerType {
public:
    ArrayType();
};

class DictType : public ContainerType {
public:
    DictType();
};

// ==================== Object工厂类 ====================
// ObjectFactory类已移动到专门的object_factory.h文件中
// 使用前向声明避免循环依赖
class ObjectFactory;

// ==================== 类型注册表 ====================
// 专注于运行时类型的注册和管理
// 职责：类型定义、类型注册、类型查询
// 不负责：对象创建、变量管理、作用域管理
class TypeRegistry {
public:
    std::map<std::string, ObjectType*> types;
    bool isGlobalRegistry;  // 标识是否为全局注册表

public:
    // 构造函数
    TypeRegistry(bool global = false) : isGlobalRegistry(global) {
        // 如果是全局注册表，可以在这里初始化内置类型
    }
    
    // 自动注册宏系统（仅用于全局注册表）
    static void registerBuiltinType(const std::string& name, ObjectType* type);
    
    // 动态库初始化函数 - 在动态库加载后调用
    static void initializeBuiltinTypes();
    
    // 静态注册器类，用于在main()之前自动注册到全局注册表
    class AutoRegistrar {
    public:
        AutoRegistrar(const std::string& name, ObjectType* type) {
            // 注册到全局注册表
            TypeRegistry* globalRegistry = getGlobalInstance();
            if (globalRegistry) {
                globalRegistry->registerType(name, type);
                // 添加调试信息（使用标准输出，避免编译问题）
                std::cout << "AutoRegistrar: Registered type '" << name << "' to global registry" << std::endl;
            }
        }
    };
    
    // 获取全局注册表实例（仅用于内置类型注册）
    static TypeRegistry* getGlobalInstance() {
        static TypeRegistry globalInstance(true);
        return &globalInstance;
    }
    
    // 注册类型
    void registerType(const std::string& name, ObjectType* type) {
        types[name] = type;
        std::cout << "TypeRegistry: Registered type '" << name << "' (total types: " << types.size() << ")" << std::endl;
    }
    
    // 获取类型
    ObjectType* getType(const std::string& name) const {
        // 首先尝试精确匹配
        auto it = types.find(name);
        if (it != types.end()) {
            return it->second;
        }
        
        // 如果精确匹配失败，尝试大小写不敏感匹配
        for (const auto& pair : types) {
            if (pair.first.length() == name.length() && 
                std::equal(pair.first.begin(), pair.first.end(), name.begin(), 
                          [](char a, char b) { return std::tolower(a) == std::tolower(b); })) {
                return pair.second;
            }
        }
        
        // 检查全局注册表中是否有该类型
        TypeRegistry* globalRegistry = getGlobalInstance();
        if (globalRegistry && globalRegistry != this) {
            // 直接访问全局注册表的types映射，避免递归调用
            auto globalIt = globalRegistry->types.find(name);
            if (globalIt != globalRegistry->types.end()) {
                return globalIt->second;
            }
            
            // 在全局注册表中也尝试大小写不敏感匹配
            for (const auto& pair : globalRegistry->types) {
                if (pair.first.length() == name.length() && 
                    std::equal(pair.first.begin(), pair.first.end(), name.begin(), 
                              [](char a, char b) { return std::tolower(a) == std::tolower(b); })) {
                    return pair.second;
                }
            }
        }
        
        return nullptr;
    }
    
    // 根据ObjectType指针获取类型名称
    std::string getTypeName(ObjectType* type) const;
    
    // 注册结构体类型
    void registerStructType(const std::string& name, StructType* type) {
        types[name] = type;
    }
    
    // 注册类类型
    void registerClassType(const std::string& name, ClassType* type) {
        types[name] = type;
    }
    
    // 创建用户定义类型的辅助方法
    Interface* createInterface(const std::string& name);
    StructType* createStructType(const std::string& name);
    ClassType* createClassType(const std::string& name);
    ClassType* createClassType(const std::string& name, const std::string& parentTypeName);
    ClassType* createClassType(const std::string& name, const std::vector<std::string>& interfaceNames);
    
    // 模板方法：创建类型
    template<typename T>
    T* createType(const std::string& name) {
        if (std::is_same<T, StructType>::value) {
            return createStructType(name);
        } else if (std::is_same<T, ClassType>::value) {
            return createClassType(name);
        } else if (std::is_same<T, Interface>::value) {
            return createInterface(name);
        }
        return nullptr;
    }
    
    // 模板方法：创建带父类的类型
    template<typename T>
    T* createType(const std::string& name, const std::string& parentTypeName) {
        if (std::is_same<T, ClassType>::value) {
            return createClassType(name, parentTypeName);
        }
        return nullptr;
    }
    
    // 获取所有类型
    const std::map<std::string, ObjectType*>& getAllTypes() const {
        return types;
    }
    
    // 检查类型是否存在
    bool hasType(const std::string& name) const {
        return types.find(name) != types.end();
    }
    
    // 获取类型数量
    
    // 从字符串创建类型
    ObjectType* createTypeFromString(const std::string& typeName);
    size_t getTypeCount() const {
        return types.size();
    }
    
    // 清理所有类型
    void clear() {
        types.clear();
    }
};

// ==================== 内置类型注册宏 ====================
// 动态库兼容的类型注册宏
// 提供显式初始化函数，确保类型在正确的时机被注册
#define REGISTER_BUILTIN_TYPE(name, TypeClass) \
    static void register_##name##_type() { \
        /* 使用registerBuiltinType方法，确保正确注册到全局注册表 */ \
        TypeRegistry::registerBuiltinType(#name, new TypeClass()); \
        std::cout << "AutoRegistrar: Registered type '" << #name << "' to global registry" << std::endl; \
    } \
    /* 提供显式初始化函数，用于动态库加载后调用 */ \
    extern "C" void register_##name##_type_explicit() { \
        register_##name##_type(); \
    }

#define REGISTER_BUILTIN_TYPE_WITH_PARENT(name, TypeClass, parentName, ParentTypeClass) \
    static void register_##name##_type() { \
        /* 使用registerBuiltinType方法，确保正确注册到全局注册表 */ \
        TypeRegistry::registerBuiltinType(#name, new TypeClass()); \
        std::cout << "AutoRegistrar: Registered type '" << #name << "' to global registry" << std::endl; \
    } \
    /* 提供显式初始化函数，用于动态库加载后调用 */ \
    extern "C" void register_##name##_type_explicit() { \
        register_##name##_type(); \
    }



#endif // BUILTIN_TYPES_H
