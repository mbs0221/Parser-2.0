#ifndef BUILTIN_TYPES_H
#define BUILTIN_TYPES_H

// 前向声明，避免循环依赖
class Value;
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <stdexcept>
#include <mutex>

// 前向声明
class Interpreter;
class TypeRegistry;
class Dict;

using namespace std;

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
    
    // 内置方法映射
    map<string, function<Value*(Value*, vector<Value*>&)>> methods;

public:
    ObjectType(const string& name, bool primitive = false, bool mutable_ = true, 
           bool reference = false, bool container = false, bool userDefined = false,
           ObjectType* parent = nullptr)
        : typeName(name), isPrimitive(primitive), isMutable(mutable_), 
          isReference(reference), isContainer(container), isUserDefined(userDefined),
          parentType(parent) {}
    
    virtual ~ObjectType() = default;
    
    // 类型信息
    string getTypeName() const { return typeName; }
    bool isPrimitiveType() const { return isPrimitive; }
    bool isMutableType() const { return isMutable; }
    bool isReferenceType() const { return isReference; }
    bool isContainerType() const { return isContainer; }
    bool isUserDefinedType() const { return isUserDefined; }
    
    // 继承关系管理
    ObjectType* getParentType() const { return parentType; }
    void setParentType(ObjectType* parent) { parentType = parent; }
    const vector<ObjectType*>& getInterfaces() const { return interfaces; }
    void addInterface(ObjectType* interface) { interfaces.push_back(interface); }
    
    // 继承关系检查
    bool isSubtypeOf(ObjectType* other) const;
    bool implements(ObjectType* interface) const;
    
    // 方法管理
    void registerMethod(const string& name, function<Value*(Value*, vector<Value*>&)> method) {
        methods[name] = method;
    }
    
    // 获取所有方法
    const map<string, function<Value*(Value*, vector<Value*>&)>>& getMethods() const {
        return methods;
    }
    
    // 检查是否有指定方法
    bool hasMethod(const string& methodName) const {
        // 在当前类型中查找方法
        if (methods.find(methodName) != methods.end()) {
            return true;
        }
        
        // 在父类中查找方法
        if (parentType && parentType->hasMethod(methodName)) {
            return true;
        }
        
        // 在接口中查找方法
        for (ObjectType* interface : interfaces) {
            if (interface->hasMethod(methodName)) {
                return true;
            }
        }
        
        return false;
    }
    
    virtual Value* callMethod(Value* instance, const string& methodName, vector<Value*>& args) {
        // 在当前类型中查找方法
        auto it = methods.find(methodName);
        if (it != methods.end()) {
            return it->second(instance, args);
        }
        
        // 在父类中查找方法
        if (parentType) {
            return parentType->callMethod(instance, methodName, args);
        }
        
        // 在接口中查找方法
        for (ObjectType* interface : interfaces) {
            try {
                return interface->callMethod(instance, methodName, args);
            } catch (const runtime_error&) {
                // 继续查找下一个接口
                continue;
            }
        }
        
        throw runtime_error("Method '" + methodName + "' not found in type '" + typeName + "'");
    }
    
    // 类型转换
    virtual Value* convertTo(ObjectType* targetType, Value* value) = 0;
    
    // 类型比较
    virtual bool isCompatibleWith(ObjectType* other) const = 0;
    
    // 获取类型信息
    virtual string getTypeInfo() const {
        return "Object: " + typeName + 
               " (primitive: " + (isPrimitive ? "true" : "false") + 
               ", mutable: " + (isMutable ? "true" : "false") + 
               ", reference: " + (isReference ? "true" : "false") + 
               ", container: " + (isContainer ? "true" : "false") + 
               ", userDefined: " + (isUserDefined ? "true" : "false") + ")";
    }
    
    // 创建默认值
    virtual Value* createDefaultValue() = 0;
    
    // 实例化方法：创建对象实例并绑定方法
    virtual Value* instantiate(vector<Value*>& args) {
        // 创建默认值作为基础
        Value* instance = createDefaultValue();
        if (!instance) return nullptr;
        
        // 绑定成员方法到实例
        bindMethodsToInstance(instance);
        
        // 调用自定义初始化逻辑
        initializeInstance(instance, args);
        
        return instance;
    }
    
    // 绑定方法到实例
    virtual void bindMethodsToInstance(Value* instance) {
        // 默认实现：将类型的方法绑定到实例
        // 子类可以重写此方法来自定义绑定逻辑
    }
    
    // 初始化实例
    virtual void initializeInstance(Value* instance, vector<Value*>& args) {
        // 默认实现：空操作
        // 子类可以重写此方法来自定义初始化逻辑
    }
};

// ==================== 访问修饰符枚举 ====================
enum VisibilityType {
    VIS_PUBLIC,
    VIS_PRIVATE,
    VIS_PROTECTED
};

// ==================== 接口类型 ====================
// 通用接口类型，只包含函数原型
class Interface : public ObjectType {
private:
    map<string, function<Value*(Value*, vector<Value*>&)>> methodPrototypes;  // 方法原型

public:
    Interface(const string& name) 
        : ObjectType(name, false, false, false, false, false) {}
    
    // 添加方法原型（不包含实现）
    void addMethodPrototype(const string& name, function<Value*(Value*, vector<Value*>&)> prototype) {
        methodPrototypes[name] = prototype;
        registerMethod(name, prototype);
    }
    
    // 获取方法原型
    function<Value*(Value*, vector<Value*>&)> getMethodPrototype(const string& name) const {
        auto it = methodPrototypes.find(name);
        return (it != methodPrototypes.end()) ? it->second : nullptr;
    }
    
    // 检查是否包含方法原型
    bool hasMethodPrototype(const string& name) const {
        return methodPrototypes.find(name) != methodPrototypes.end();
    }
    
    // 获取所有方法原型名称
    vector<string> getMethodPrototypeNames() const {
        vector<string> names;
        for (const auto& pair : methodPrototypes) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    Value* convertTo(ObjectType* targetType, Value* value) override {
        // 接口不能进行类型转换
        return nullptr;
    }
    
    bool isCompatibleWith(ObjectType* other) const override {
        // 接口只与自身兼容
        return typeName == other->getTypeName();
    }
    
    Value* createDefaultValue() override {
        // 接口不能创建默认值
        return nullptr;
    }
};

// ==================== 类类型 ====================
// 用户定义的类类型（支持继承、多态等面向对象特性）
class ClassType : public ObjectType {
private:
    map<string, ObjectType*> memberTypes;  // 成员类型
    map<string, function<Value*(Value*, vector<Value*>&)>> userMethods;  // 用户定义方法
    map<string, VisibilityType> memberVisibility;  // 成员访问修饰符
    function<Value*(vector<Value*>&)> defaultConstructor;  // 默认构造函数

public:
    ClassType(const string& name) 
        : ObjectType(name, false, true, true, false, true) {}
    
    // 添加成员
    virtual void addMember(const string& name, ObjectType* type, VisibilityType visibility = VIS_PUBLIC) {
        memberTypes[name] = type;
        memberVisibility[name] = visibility;
    }
    
    // 添加方法
    virtual void addMethod(const string& name, function<Value*(Value*, vector<Value*>&)> method, 
                   VisibilityType visibility = VIS_PUBLIC) {
        userMethods[name] = method;
        registerMethod(name, method);
        memberVisibility[name] = visibility;
    }
    
    // 获取成员访问修饰符
    VisibilityType getMemberVisibility(const string& name) const {
        auto it = memberVisibility.find(name);
        return (it != memberVisibility.end()) ? it->second : VIS_PUBLIC;
    }
    
    // 获取成员类型
    ObjectType* getMemberType(const string& name) const {
        auto it = memberTypes.find(name);
        return (it != memberTypes.end()) ? it->second : nullptr;
    }
    
    // 检查是否有成员
    bool hasMember(const string& name) const {
        return memberTypes.find(name) != memberTypes.end();
    }
    
    // 获取所有成员名称
    vector<string> getMemberNames() const {
        vector<string> names;
        for (const auto& pair : memberTypes) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    // 获取所有成员类型
    map<string, ObjectType*> getMemberTypes() const {
        return memberTypes;
    }
    
    // 获取所有用户定义方法
    const map<string, function<Value*(Value*, vector<Value*>&)>>& getUserMethods() const {
        return userMethods;
    }
    
    // 获取所有方法（包括继承的方法）
    map<string, function<Value*(Value*, vector<Value*>&)>> getMethods() const {
        map<string, function<Value*(Value*, vector<Value*>&)>> allMethods = userMethods;
        
        // 添加从ObjectType基类继承的方法
        const map<string, function<Value*(Value*, vector<Value*>&)>>& baseMethods = ObjectType::getMethods();
        allMethods.insert(baseMethods.begin(), baseMethods.end());
        
        return allMethods;
    }
    
    // 构造函数管理
    void setDefaultConstructor(function<Value*(vector<Value*>&)> constructor) {
        defaultConstructor = constructor;
    }
    
    function<Value*(vector<Value*>&)> getDefaultConstructor() const {
        return defaultConstructor;
    }
    
    bool hasDefaultConstructor() const {
        return defaultConstructor != nullptr;
    }
    
    Value* convertTo(ObjectType* targetType, Value* value) override {
        // 用户定义类型的转换逻辑
        return nullptr;
    }
    
    bool isCompatibleWith(ObjectType* other) const override {
        return typeName == other->getTypeName();
    }
    
    Value* createDefaultValue() override {
        // 创建用户定义类型的默认值
        return nullptr;
    }
};

// ==================== 结构体类型 ====================
// 结构体类型：本质上是方法成员全部公有的ClassType
class StructType : public ClassType {
public:
    StructType(const string& name) 
        : ClassType(name) {}
    
    // 重写addMember方法，强制所有成员为公有
    void addMember(const string& name, ObjectType* type, VisibilityType visibility = VIS_PUBLIC) override {
        ClassType::addMember(name, type, VIS_PUBLIC);
    }
    
    // 重写addMethod方法，强制所有方法为公有
    void addMethod(const string& name, function<Value*(Value*, vector<Value*>&)> method, 
                   VisibilityType visibility = VIS_PUBLIC) override {
        ClassType::addMethod(name, method, VIS_PUBLIC);
    }
};

// ==================== 内置类型定义 ====================

// 整数类型
class IntType : public ObjectType {
public:
    IntType();
    Value* convertTo(ObjectType* targetType, Value* value) override;
    bool isCompatibleWith(ObjectType* other) const override;
    Value* createDefaultValue() override;
};

// 浮点数类型
class DoubleType : public ObjectType {
public:
    DoubleType();
    Value* convertTo(ObjectType* targetType, Value* value) override;
    bool isCompatibleWith(ObjectType* other) const override;
    Value* createDefaultValue() override;
};

// 字符类型
class CharType : public ObjectType {
public:
    CharType();
    Value* convertTo(ObjectType* targetType, Value* value) override;
    bool isCompatibleWith(ObjectType* other) const override;
    Value* createDefaultValue() override;
};

// 布尔类型
class BoolType : public ObjectType {
public:
    BoolType();
    Value* convertTo(ObjectType* targetType, Value* value) override;
    bool isCompatibleWith(ObjectType* other) const override;
    Value* createDefaultValue() override;
};

// 字符串类型
class StringType : public ObjectType {
public:
    StringType();
    Value* convertTo(ObjectType* targetType, Value* value) override;
    bool isCompatibleWith(ObjectType* other) const override;
    Value* createDefaultValue() override;
};

// 数组类型
class ArrayType : public ObjectType {
public:
    ArrayType();
    Value* convertTo(ObjectType* targetType, Value* value) override;
    bool isCompatibleWith(ObjectType* other) const override;
    Value* createDefaultValue() override;
};

// 字典类型
class DictType : public ObjectType {
public:
    DictType();
    Value* convertTo(ObjectType* targetType, Value* value) override;
    bool isCompatibleWith(ObjectType* other) const override;
    Value* createDefaultValue() override;
};

// 空值类型
class NullType : public ObjectType {
public:
    NullType();
    Value* convertTo(ObjectType* targetType, Value* value) override;
    bool isCompatibleWith(ObjectType* other) const override;
    Value* createDefaultValue() override;
};

// ==================== Object工厂类（单例模式） ====================
// 用于创建指定Object子类型的Value对象的工厂类
class ObjectFactory {
public:
    // 获取单例实例
    static ObjectFactory* getInstance();
    static void destroyInstance();
    
    // 根据ObjectType类型创建对应的Value对象
    Value* createValue(ObjectType* type);
    
    // 根据类型名称创建对应的Value对象
    Value* createValue(const string& typeName);
    
    // 根据ObjectType类型创建默认值
    Value* createDefaultValue(ObjectType* type);
    
    // 根据类型名称创建默认值
    Value* createDefaultValue(const string& typeName);
    
    // 创建null值
    Value* createNull();
    
    // 创建指定类型的null值（用于类型转换）
    Value* createNullForType(ObjectType* type);
    
    // 创建指定类型的null值（用于类型转换）
    Value* createNullForType(const string& typeName);
    
    // 检查类型是否支持null值
    bool supportsNull(ObjectType* type);
    
    // 检查类型名称是否支持null值
    bool supportsNull(const string& typeName);
    
    // 静态便捷方法（保持向后兼容）
    static Value* createValueStatic(ObjectType* type);
    static Value* createValueStatic(const string& typeName);
    static Value* createDefaultValueStatic(ObjectType* type);
    static Value* createDefaultValueStatic(const string& typeName);
    static Value* createNullStatic();
    static Value* createNullForTypeStatic(ObjectType* type);
    static Value* createNullForTypeStatic(const string& typeName);
    static bool supportsNullStatic(ObjectType* type);
    static bool supportsNullStatic(const string& typeName);
    
    // ==================== 结构体和类实例化方法 ====================
    
    // 结构体实例化
    Value* createStructInstance(StructType* structType, const vector<Value*>& args);
    Value* createStructInstance(const string& structTypeName, const vector<Value*>& args);
    
    // 类实例化
    Value* createClassInstance(ClassType* classType, const vector<Value*>& args);
    Value* createClassInstance(const string& classTypeName, const vector<Value*>& args);
    
    // 静态实例化方法
    static Value* createStructInstanceStatic(StructType* structType, const vector<Value*>& args);
    static Value* createStructInstanceStatic(const string& structTypeName, const vector<Value*>& args);
    static Value* createClassInstanceStatic(ClassType* classType, const vector<Value*>& args);
    static Value* createClassInstanceStatic(const string& classTypeName, const vector<Value*>& args);
    
    // 私有辅助方法
    void initializeFromDict(Dict* instance, ClassType* type, Dict* memberDict);
    void initializeFromArgs(Dict* instance, ClassType* type, const vector<Value*>& args);
    Value* createMemberDefaultValue(ClassType* type, const string& memberName);
    void initializeMethods(Dict* instance, ClassType* type);
    
    // ==================== 依赖注入接口 ====================
    
    // 设置依赖组件（实例方法）
    void setTypeRegistry(TypeRegistry* registry);
    void setInterpreter(Interpreter* interp);
    
private:
    // 私有构造函数（单例模式）
    ObjectFactory();
    ~ObjectFactory();
    
    // 禁用拷贝构造和赋值
    ObjectFactory(const ObjectFactory&) = delete;
    ObjectFactory& operator=(const ObjectFactory&) = delete;
    
    // 单例实例
    static ObjectFactory* instance;
    static std::mutex instanceMutex;
    
    // 依赖注入的组件
    TypeRegistry* typeRegistry;
    Interpreter* interpreter;
    
    // 类型名称到创建函数的映射
    map<string, function<Value*()>> valueCreators;
    
    // 初始化工厂映射
    void initializeCreators();
    
    // 检查是否已初始化
    bool initialized;
};

// ==================== 全局访问函数 ====================
// 全局访问函数实现
inline ObjectFactory* getObjectFactory() {
    return ObjectFactory::getInstance();
}

#endif // BUILTIN_TYPES_H