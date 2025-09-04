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
#include "common/logger.h"
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

// ==================== 访问修饰符枚举 ====================
enum VisibilityType {
    VIS_PUBLIC,
    VIS_PRIVATE,
    VIS_PROTECTED
};

// ==================== Trait 接口系统 ====================
// Trait 接口定义 - 用于扩展内置类型功能
// 暂时通过宏注释掉，专注于基础功能
/*
class ITrait {
public:
    virtual ~ITrait() = default;
    virtual string getName() const = 0;
    virtual bool canApplyTo(const ObjectType* type) const = 0;
};

// 字符串操作 Trait
class IStringOperations : public ITrait {
public:
    virtual ~IStringOperations() = default;
    virtual string getName() const override { return "StringOperations"; }
    
    // 字符串操作方法
    virtual string substring(int start, int end) const = 0;
    virtual string toUpperCase() const = 0;
    virtual string toLowerCase() const = 0;
    virtual bool startsWith(const string& prefix) const = 0;
    virtual bool endsWith(const string& suffix) const = 0;
    virtual string trim() const = 0;
};

// 数组操作 Trait
class IArrayOperations : public ITrait {
public:
    virtual ~IArrayOperations() = default;
    virtual string getName() const override { return "ArrayOperations"; }
    
    // 数组操作方法
    virtual void sort() = 0;
    virtual void reverse() = 0;
    virtual void shuffle() = 0;
    virtual bool contains(const Value* item) const = 0;
    virtual int indexOf(const Value* item) const = 0;
    virtual void removeAt(int index) = 0;
};

// 字典操作 Trait
class IDictOperations : public ITrait {
public:
    virtual ~IDictOperations() = default;
    virtual string getName() const override { return "DictOperations"; }
    
    // 字典操作方法
    virtual vector<string> keys() const = 0;
    virtual vector<Value*> values() const = 0;
    virtual vector<pair<string, Value*>> items() const = 0;
    virtual void merge(const IDictOperations* other) = 0;
    virtual bool isEmpty() const = 0;
    virtual void clear() = 0;
};
*/

// ==================== 类型支持接口 ====================

// 成员支持接口 - 支持成员变量和成员类型
class IMemberSupport {
public:
    virtual ~IMemberSupport() = default;
    
    // 成员管理
    virtual void addMember(const string& name, ObjectType* type, VisibilityType visibility = VIS_PUBLIC) = 0;
    virtual ObjectType* getMemberType(const string& name) const = 0;
    virtual bool hasMember(const string& name) const = 0;
    virtual vector<string> getMemberNames() const = 0;
    virtual map<string, ObjectType*> getMemberTypes() const = 0;
    
    // 成员初始值管理
    virtual void setMemberInitialValue(const string& memberName, Value* initialValue) = 0;
    virtual Value* getMemberInitialValue(const string& memberName) const = 0;
    virtual bool hasMemberInitialValue(const string& memberName) const = 0;
    virtual const map<string, Value*>& getAllMemberInitialValues() const = 0;
};

// 方法支持接口 - 支持实例方法和静态方法
class IMethodSupport {
public:
    virtual ~IMethodSupport() = default;
    
    // 便捷方法注册接口 - 自动从Function生成签名
    virtual void addUserMethod(Function* func, VisibilityType visibility = VIS_PUBLIC) = 0;
    virtual void addStaticMethod(Function* func, VisibilityType visibility = VIS_PUBLIC) = 0;
    
    // 方法查询
    virtual Function* findUserMethod(const FunctionSignature& signature) const = 0;
    virtual Function* findStaticMethod(const FunctionSignature& signature) const = 0;
    virtual Function* findUserMethod(const string& name, const vector<Value*>& args) const = 0;
    virtual Function* findStaticMethod(const string& name, const vector<Value*>& args) const = 0;
    
    // 方法重载支持
    virtual vector<FunctionSignature> getUserMethodOverloads(const string& name) const = 0;
    virtual vector<FunctionSignature> getStaticMethodOverloads(const string& name) const = 0;
    
    // 方法存在性检查
    virtual bool hasUserMethod(const string& name) const = 0;
    virtual bool hasStaticMethod(const string& name) const = 0;
    virtual bool hasStaticMethodName(const string& name) const = 0;
    
    // 方法映射获取
    virtual const map<FunctionSignature, Function*>& getUserFunctionMethods() const = 0;
    virtual const map<FunctionSignature, Function*>& getStaticMethods() const = 0;
    
    // 向后兼容的方法
    virtual map<string, Function*> getUserFunctionMethodsByName() const = 0;
    virtual map<string, Function*> getStaticMethodsByName() const = 0;
};

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
    
    // Trait 支持 - 暂时注释掉
    // vector<ITrait*> traits;

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
    
    // 基础类型信息查询
    virtual bool supportsMembers() const { return false; }
    virtual bool supportsMethods() const { return false; }
    virtual bool supportsInheritance() const { return false; }
    
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
    
    // 成员访问（通过接口实现）
    virtual Value* accessMember(Value* instance, const string& memberName);
    virtual bool hasMember(const string& memberName) const {
        // 检查是否支持成员
        if (const IMemberSupport* memberSupport = dynamic_cast<const IMemberSupport*>(this)) {
            return memberSupport->hasMember(memberName);
        }
        return false;
    }
    
    // 自动注册方法
    void registerConstructor();
    
    // 成员访问方法注册
    void registerMemberAccess();
    
    // 类型描述
    string getTypeDescription() const;
    
    // 类型比较
    bool operator==(const ObjectType& other) const;
    bool operator!=(const ObjectType& other) const;
    
    // Trait 管理 - 暂时注释掉
    /*
    void addTrait(ITrait* trait);
    void removeTrait(const string& traitName);
    bool hasTrait(const string& traitName) const;
    ITrait* getTrait(const string& traitName) const;
    const vector<ITrait*>& getTraits() const;
    
    // Trait 功能检查
    bool supportsStringOperations() const;
    bool supportsArrayOperations() const;
    bool supportsDictOperations() const;
    */
};

// ==================== 接口类型 ====================
// 通用接口类型，只包含函数原型 - 实现方法支持
class Interface : public ObjectType, public IMethodSupport {
private:
    // 方法原型 - 使用函数签名支持重载
    map<FunctionSignature, function<Value*(Value*, vector<Value*>&)>> methodPrototypes;
    
    // 方法名称到函数签名的映射（用于快速查找）
    map<string, vector<FunctionSignature>> methodNameToSignatures;

public:
    Interface(const string& name);
    
    // 添加方法原型（不包含实现）
    void addMethodPrototype(const string& name, function<Value*(Value*, vector<Value*>&)> prototype);
    void addMethodPrototype(const FunctionSignature& signature, function<Value*(Value*, vector<Value*>&)> prototype);
    void addMethodPrototype(const FunctionPrototype* prototype);
    
    // 获取方法原型
    function<Value*(Value*, vector<Value*>&)> getMethodPrototype(const string& name) const;
    function<Value*(Value*, vector<Value*>&)> getMethodPrototype(const FunctionSignature& signature) const;
    
    // 检查是否包含方法原型
    bool hasMethodPrototype(const string& name) const;
    bool hasMethodPrototype(const FunctionSignature& signature) const;
    
    // 获取所有方法原型名称
    vector<string> getMethodPrototypeNames() const;
    
    // 函数签名支持的方法
    // 根据函数签名查找方法原型
    function<Value*(Value*, vector<Value*>&)> findMethodPrototype(const FunctionSignature& signature) const;
    
    // 根据名称和参数查找方法原型（重载解析）
    function<Value*(Value*, vector<Value*>&)> findMethodPrototype(const string& name, const vector<Value*>& args) const;
    
    // 获取方法的所有重载版本
    vector<FunctionSignature> getMethodPrototypeOverloads(const string& name) const;
    
    // 使用基类的默认convertTo实现
    bool isCompatibleWith(ObjectType* other) const override;
    
    // 接口类型不能创建实例
    Value* createDefaultValue() override;
    
    // ==================== IMethodSupport 接口实现 ====================
    // 方法管理
    void addUserMethod(Function* func, VisibilityType visibility = VIS_PUBLIC) override;
    void addStaticMethod(Function* func, VisibilityType visibility = VIS_PUBLIC) override;
    
    // 方法查询
    Function* findUserMethod(const FunctionSignature& signature) const override;
    Function* findStaticMethod(const FunctionSignature& signature) const override;
    Function* findUserMethod(const string& name, const vector<Value*>& args) const override;
    Function* findStaticMethod(const string& name, const vector<Value*>& args) const override;
    
    // 方法重载支持
    vector<FunctionSignature> getUserMethodOverloads(const string& name) const override;
    vector<FunctionSignature> getStaticMethodOverloads(const string& name) const override;
    
    // 方法存在性检查
    bool hasUserMethod(const string& name) const override;
    bool hasStaticMethod(const string& name) const override;
    bool hasStaticMethodName(const string& name) const override;
    
    // 方法映射获取
    const map<FunctionSignature, Function*>& getUserFunctionMethods() const override;
    const map<FunctionSignature, Function*>& getStaticMethods() const override;
    
    // 向后兼容的方法
    map<string, Function*> getUserFunctionMethodsByName() const override;
    map<string, Function*> getStaticMethodsByName() const override;
    
    // 重写 ObjectType 的方法支持检查
    bool supportsMethods() const override { return true; }
};

// ==================== 类类型 ====================
// 用户定义的类类型 - 实现成员和方法支持
class ClassType : public ObjectType, public IMemberSupport, public IMethodSupport {
protected:
    // 成员类型映射
    map<string, ObjectType*> memberTypes;
    map<string, VisibilityType> memberVisibility;
    
    // 静态成员
    map<string, ObjectType*> staticMemberTypes;
    map<string, Value*> staticMemberValues;
    map<string, VisibilityType> staticMemberVisibility;
    
    // 用户定义方法 - 使用函数签名支持重载
    map<FunctionSignature, Function*> userFunctionMethods;
    map<FunctionSignature, Function*> staticMethods;
    
    // 方法名称到函数签名的映射（用于快速查找）
    map<string, vector<FunctionSignature>> methodNameToSignatures;
    map<string, vector<FunctionSignature>> staticMethodNameToSignatures;
    
    // 成员初始值
    map<string, Value*> memberInitialValues;
    
    // 默认构造函数
    function<Value*(vector<Value*>&)> defaultConstructor;

public:
    ClassType(const string& name, bool isPrimitive = false, bool isMutable = true, bool isReference = false);
    
    // 成员管理
    void addStaticMember(const string& name, ObjectType* type, Value* value, VisibilityType visibility = VIS_PUBLIC);
    
    // 成员初始值管理
    void setAllMemberInitialValues(const map<string, Value*>& initialValues);
    
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
    
    // 静态成员查询
    Value* getStaticMemberValue(const string& name) const;
    void setStaticMemberValue(const string& name, Value* value);
    
    // 静态成员访问
    bool hasStaticMember(const string& name) const;
    Value* accessStaticMember(const string& name) const;
    
    // Trait 支持实现 - 暂时注释掉
    /*
    void addTrait(ITrait* trait) override;
    void removeTrait(const string& traitName) override;
    bool hasTrait(const string& traitName) const override;
    ITrait* getTrait(const string& traitName) const override;
    const vector<ITrait*>& getTraits() const override;
    
    // Trait 功能检查实现
    bool supportsStringOperations() const override;
    bool supportsArrayOperations() const override;
    bool supportsDictOperations() const override;
    */
    
    // 获取静态成员类型映射
    const map<string, ObjectType*>& getStaticMemberTypes() const;
    
    // 获取静态成员值映射
    const map<string, Value*>& getStaticMembers() const;
    
    // 容器类型支持的方法（从原来的ContainerType移过来）
    virtual bool supportsAccess() const { return false; }
    virtual bool supportsIteration() const { return false; }
    virtual bool supportsSizeQuery() const { return false; }
    
    // ==================== IMemberSupport 接口实现 ====================
    // 成员管理
    void addMember(const string& name, ObjectType* type, VisibilityType visibility = VIS_PUBLIC) override;
    ObjectType* getMemberType(const string& name) const override;
    bool hasMember(const string& name) const override;
    vector<string> getMemberNames() const override;
    map<string, ObjectType*> getMemberTypes() const override;
    
    // 成员初始值管理
    void setMemberInitialValue(const string& memberName, Value* initialValue) override;
    Value* getMemberInitialValue(const string& memberName) const override;
    bool hasMemberInitialValue(const string& memberName) const override;
    const map<string, Value*>& getAllMemberInitialValues() const override;
    
    // ==================== IMethodSupport 接口实现 ====================
    // 方法管理
    void addUserMethod(Function* func, VisibilityType visibility = VIS_PUBLIC) override;
    void addStaticMethod(Function* func, VisibilityType visibility = VIS_PUBLIC) override;
    
    // 方法查询
    Function* findUserMethod(const FunctionSignature& signature) const override;
    Function* findStaticMethod(const FunctionSignature& signature) const override;
    Function* findUserMethod(const string& name, const vector<Value*>& args) const override;
    Function* findStaticMethod(const string& name, const vector<Value*>& args) const override;
    
    // 方法重载支持
    vector<FunctionSignature> getUserMethodOverloads(const string& name) const override;
    vector<FunctionSignature> getStaticMethodOverloads(const string& name) const override;
    
    // 方法存在性检查
    bool hasUserMethod(const string& name) const override;
    bool hasStaticMethod(const string& name) const override;
    bool hasStaticMethodName(const string& name) const override;
    
    // 方法映射获取
    const map<FunctionSignature, Function*>& getUserFunctionMethods() const override;
    const map<FunctionSignature, Function*>& getStaticMethods() const override;
    
    // 向后兼容的方法
    map<string, Function*> getUserFunctionMethodsByName() const override;
    map<string, Function*> getStaticMethodsByName() const override;
    
    // 重写 ObjectType 的方法支持检查
    bool supportsMethods() const override { return true; }
    bool supportsMembers() const override { return true; }
    
protected:
    // 自动注册下标访问方法
    void registerSubscriptAccess();
    
    // 重写 ObjectType 的成员访问方法注册
    void registerMemberAccess();
    
    // 创建带参数的值的虚方法（子类可以重写）
    virtual Value* createValueWithArgs(const vector<Value*>& args);
};

// ==================== 结构体类型 ====================
// 结构体类型：本质上是方法成员全部公有的ClassType - 继承接口实现
class StructType : public ClassType {
public:
    StructType(const string& name);
    
    // 重写addMember方法，强制所有成员为公有
    void addMember(const string& name, ObjectType* type, VisibilityType visibility = VIS_PUBLIC, Value* initialValue = nullptr);
    
    // 重写registerMethod方法，强制所有方法为公有
    void registerMethod(const string& name, function<Value*(Value*, vector<Value*>&)> method);
};

// ==================== 内置类型类定义 ====================
// 这些类型类与Value类一一对应，现在都继承ClassType以支持完整的功能

// 基本类型基类
class PrimitiveType : public ObjectType, public IMethodSupport {
public:
    PrimitiveType(const string& name, bool mutable_ = true) 
        : ObjectType(name, true, mutable_, false) {}
    
    // IMethodSupport 接口实现
    void addUserMethod(Function* func, VisibilityType visibility = VIS_PUBLIC) override {
        if (!func) return;
        
        // 从Function获取函数签名
        FunctionSignature signature = func->getSignature();
        
        // 直接存储到函数签名映射
        userFunctionMethods[signature] = func;
        
        // 更新名称到签名的映射
        string methodName = signature.getName();
        methodNameToSignatures[methodName].push_back(signature);
    }
    
    void addStaticMethod(Function* func, VisibilityType visibility = VIS_PUBLIC) override {
        if (!func) return;
        
        // 从Function获取函数签名
        FunctionSignature signature = func->getSignature();
        
        // 直接存储到函数签名映射
        staticMethods[signature] = func;
        
        // 更新名称到签名的映射
        string methodName = signature.getName();
        staticMethodNameToSignatures[methodName].push_back(signature);
    }
    
    Function* findUserMethod(const FunctionSignature& signature) const override;
    Function* findStaticMethod(const FunctionSignature& signature) const override;
    Function* findUserMethod(const string& name, const vector<Value*>& args) const override;
    Function* findStaticMethod(const string& name, const vector<Value*>& args) const override;
    vector<FunctionSignature> getUserMethodOverloads(const string& name) const override;
    vector<FunctionSignature> getStaticMethodOverloads(const string& name) const override;
    bool hasUserMethod(const string& name) const override;
    bool hasStaticMethod(const string& name) const override;
    bool hasStaticMethodName(const string& name) const override;
    const map<FunctionSignature, Function*>& getUserFunctionMethods() const override;
    const map<FunctionSignature, Function*>& getStaticMethods() const override;
    map<string, Function*> getUserFunctionMethodsByName() const override;
    map<string, Function*> getStaticMethodsByName() const override;
    
    // 重写 ObjectType 的方法支持检查
    bool supportsMethods() const override { return true; }
    
private:
    // 方法存储
    map<FunctionSignature, Function*> userFunctionMethods;
    map<FunctionSignature, Function*> staticMethods;
    map<string, vector<FunctionSignature>> methodNameToSignatures;
    map<string, vector<FunctionSignature>> staticMethodNameToSignatures;
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

// 内置容器类型（继承ClassType，支持成员和方法）
class StringType : public ClassType {
public:
    StringType();
    
    // 重写容器支持方法
    bool supportsAccess() const override { return true; }
    bool supportsIteration() const override { return true; }
    bool supportsSizeQuery() const override { return true; }
};

class ArrayType : public ClassType {
public:
    ArrayType();
    
    // 重写容器支持方法
    bool supportsAccess() const override { return true; }
    bool supportsIteration() const override { return true; }
    bool supportsSizeQuery() const override { return true; }
};

class DictType : public ClassType {
public:
    DictType();
    
    // 重写容器支持方法
    bool supportsAccess() const override { return true; }
    bool supportsIteration() const override { return true; }
    bool supportsSizeQuery() const override { return true; }
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
                LOG_INFO("AutoRegistrar: Registered type '" + name + "' to global registry");
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
        LOG_INFO("TypeRegistry: Registered type '" + name + "' (total types: " + std::to_string(types.size()) + ")");
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
        LOG_INFO("AutoRegistrar: Registered type '" + std::string(#name) + "' to global registry"); \
    } \
    /* 提供显式初始化函数，用于动态库加载后调用 */ \
    extern "C" void register_##name##_type_explicit() { \
        register_##name##_type(); \
    }

#define REGISTER_BUILTIN_TYPE_WITH_PARENT(name, TypeClass, parentName, ParentTypeClass) \
    static void register_##name##_type() { \
        /* 使用registerBuiltinType方法，确保正确注册到全局注册表 */ \
        TypeRegistry::registerBuiltinType(#name, new TypeClass()); \
        LOG_INFO("AutoRegistrar: Registered type '" + std::string(#name) + "' to global registry"); \
    } \
    /* 提供显式初始化函数，用于动态库加载后调用 */ \
    extern "C" void register_##name##_type_explicit() { \
        register_##name##_type(); \
    }



#endif // BUILTIN_TYPES_H
