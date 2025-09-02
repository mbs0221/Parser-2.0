# ObjectType重构：接口分离设计

## 重构概述

我们将ObjectType重构为抽象基类，去掉了方法管理功能，然后创建了IMemberSupport和IMethodSupport接口，让不同的子类根据需要实现这些功能。

## 重构内容

### 1. **ObjectType基类简化**

#### 重构前：
```cpp
class ObjectType {
protected:
    // 方法管理相关字段
    std::map<FunctionSignature, std::function<Value*(Value*, std::vector<Value*>&)>> methods;
    std::map<FunctionSignature, std::function<Value*(std::vector<Value*>&)>> staticMethods;
    
public:
    // 大量方法管理相关方法
    void registerMethod(...);
    void registerStaticMethod(...);
    bool hasMethod(...);
    Value* callMethod(...);
    // ... 更多方法管理代码
};
```

#### 重构后：
```cpp
class ObjectType {
protected:
    // 只保留基本类型信息
    string typeName;
    bool isPrimitive;
    bool isMutable;
    bool isReference;
    bool isContainer;
    bool isUserDefined;
    
    // 继承关系
    ObjectType* parentType;
    vector<ObjectType*> interfaces;

public:
    // 基础类型信息查询
    virtual bool supportsMembers() const { return false; }
    virtual bool supportsMethods() const { return false; }
    virtual bool supportsInheritance() const { return false; }
    
    // 成员访问通过接口实现
    virtual bool hasMember(const string& memberName) const {
        if (const IMemberSupport* memberSupport = dynamic_cast<const IMemberSupport*>(this)) {
            return memberSupport->hasMember(memberName);
        }
        return false;
    }
};
```

### 2. **新增接口定义**

#### IMemberSupport接口：
```cpp
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
```

#### IMethodSupport接口：
```cpp
class IMethodSupport {
public:
    virtual ~IMethodSupport() = default;
    
    // 方法管理
    virtual void addUserMethod(const FunctionSignature& signature, UserFunction* userFunc, VisibilityType visibility = VIS_PUBLIC) = 0;
    virtual void addStaticMethod(const FunctionSignature& signature, UserFunction* userFunc, VisibilityType visibility = VIS_PUBLIC) = 0;
    
    // 方法查询
    virtual UserFunction* findUserMethod(const FunctionSignature& signature) const = 0;
    virtual UserFunction* findStaticMethod(const FunctionSignature& signature) const = 0;
    virtual UserFunction* findUserMethod(const string& name, const vector<Value*>& args) const = 0;
    virtual UserFunction* findStaticMethod(const string& name, const vector<Value*>& args) const = 0;
    
    // 方法重载支持
    virtual vector<FunctionSignature> getUserMethodOverloads(const string& name) const = 0;
    virtual vector<FunctionSignature> getStaticMethodOverloads(const string& name) const = 0;
    
    // 方法存在性检查
    virtual bool hasUserMethod(const string& name) const = 0;
    virtual bool hasStaticMethod(const string& name) const = 0;
    virtual bool hasStaticMethodName(const string& name) const = 0;
    
    // 方法映射获取
    virtual const map<FunctionSignature, UserFunction*>& getUserFunctionMethods() const = 0;
    virtual const map<FunctionSignature, UserFunction*>& getStaticMethods() const = 0;
    
    // 向后兼容的方法
    virtual map<string, UserFunction*> getUserFunctionMethodsByName() const = 0;
    virtual map<string, UserFunction*> getStaticMethodsByName() const = 0;
};
```

### 3. **子类接口实现**

#### ClassType实现两个接口：
```cpp
class ClassType : public ObjectType, public IMemberSupport, public IMethodSupport {
    // 实现所有成员和方法功能
};
```

#### Interface实现方法支持：
```cpp
class Interface : public ObjectType, public IMethodSupport {
    // 只实现方法功能，不实现成员功能
};
```

#### ContainerType实现成员支持：
```cpp
class ContainerType : public ObjectType, public IMemberSupport {
    // 只实现成员功能，不实现方法功能
};
```

#### StructType继承ClassType：
```cpp
class StructType : public ClassType {
    // 继承所有功能，但强制成员为公有
};
```

## 重构优势

### 1. **职责分离更清晰**
- **ObjectType**: 只负责基本类型信息和继承关系
- **IMemberSupport**: 专门负责成员管理
- **IMethodSupport**: 专门负责方法管理

### 2. **类型系统更灵活**
```cpp
// 可以根据需要选择实现的功能
if (type->supportsMembers()) {
    // 类型支持成员
    if (const IMemberSupport* memberSupport = dynamic_cast<const IMemberSupport*>(type)) {
        memberSupport->addMember("name", stringType);
    }
}

if (type->supportsMethods()) {
    // 类型支持方法
    if (const IMethodSupport* methodSupport = dynamic_cast<const IMethodSupport*>(type)) {
        methodSupport->addUserMethod(signature, userFunc);
    }
}
```

### 3. **代码更易维护**
- 每个接口职责单一
- 新增功能只需要实现相应接口
- 不需要修改ObjectType基类

### 4. **性能优化空间**
- 基础类型不需要实现所有接口
- 可以根据需要选择功能
- 减少不必要的虚函数调用

## 使用示例

### 1. **类型功能检查**
```cpp
ObjectType* type = getType("MyClass");

if (type->supportsMembers()) {
    cout << "类型支持成员" << endl;
}

if (type->supportsMethods()) {
    cout << "类型支持方法" << endl;
}

if (type->supportsInheritance()) {
    cout << "类型支持继承" << endl;
}
```

### 2. **接口功能使用**
```cpp
// 成员操作
if (IMemberSupport* memberSupport = dynamic_cast<IMemberSupport*>(type)) {
    memberSupport->addMember("name", stringType);
    memberSupport->setMemberInitialValue("name", new String("default"));
}

// 方法操作
if (IMethodSupport* methodSupport = dynamic_cast<IMethodSupport*>(type)) {
    methodSupport->addUserMethod(signature, userFunc);
    UserFunction* method = methodSupport->findUserMethod("getName", {});
}
```

### 3. **类型创建**
```cpp
// 基础类型 - 不实现任何接口
class IntegerType : public ObjectType {
    // 只提供基本类型信息
};

// 类类型 - 实现所有接口
class MyClassType : public ObjectType, public IMemberSupport, public IMethodSupport {
    // 提供完整的面向对象功能
};

// 接口类型 - 只实现方法接口
class MyInterfaceType : public ObjectType, public IMethodSupport {
    // 只提供方法原型，不提供实现
};
```

## 总结

通过这次重构，我们实现了：

1. **更清晰的职责分离** - ObjectType专注于基本类型信息
2. **更灵活的功能组合** - 通过接口组合实现不同功能
3. **更好的可维护性** - 新增功能不影响基类
4. **更高的性能** - 基础类型不需要实现所有功能

这种设计既保持了类型系统的统一性，又提供了足够的灵活性，是一个很好的平衡方案。
