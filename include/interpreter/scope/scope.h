#ifndef SCOPE_H
#define SCOPE_H

#include <string>
#include <map>
// #include <list>
#include <vector>
#include <iostream>
#include <functional>
#include <memory>
#include <tuple>
#include "interpreter/values/value.h"
#include "interpreter/types/types.h"

using namespace std;

// 前向声明
class TypeRegistry;
class ObjectRegistry;
class ObjectType;
class ClassType;



// ==================== Scope结构 ====================
// 作用域结构 - 包含类型管理器和对象管理器
struct Scope {
    // 类型管理器 - 管理当前作用域中的类型定义
    std::unique_ptr<TypeRegistry> typeRegistry;
    
    // 对象管理器 - 管理当前作用域中的对象（变量、函数、实例等）
    std::unique_ptr<ObjectRegistry> objectRegistry;
    
    // 参数作用域标识 - 标识当前作用域是否专门用于参数绑定
    bool isParameterScope;
    

    
    // 构造函数
    Scope();
    
    // 析构函数
    ~Scope();
    

    
    // 打印作用域内容（用于调试）
    void print() const;
    
    // 获取类型管理器
    TypeRegistry* getTypeRegistry() const;
    
    // 获取对象管理器
    ObjectRegistry* getObjectRegistry() const;
    
    // ==================== 变量访问接口 ====================
    // 获取变量值（基础版本）
    Value* getVariable(const std::string& name) const;
    
    // 获取变量值并转换为指定类型（泛型版本）
    template<typename T>
    T* getVariable(const std::string& name) const {
        Value* value = getVariable(name);
        if (value) {
            return dynamic_cast<T*>(value);
        }
        return nullptr;
    }
    
    // 获取变量值并转换为指定类型，带默认值
    template<typename T>
    T* getVariable(const std::string& name, T* defaultValue) const {
        T* result = getVariable<T>(name);
        return result ? result : defaultValue;
    }
    
    // 设置变量值到当前作用域
    void setVariable(const std::string& name, Value* value);
    
    // 设置变量值到当前作用域（类型化版本）
    template<typename T>
    void setVariable(const std::string& name, T* value) {
        setVariable(name, static_cast<Value*>(value));
    }
    
    // ==================== 统一参数访问接口 ====================
    // 获取函数参数并转换为指定类型（语义化方法）
    template<typename T>
    T* getArgument(const std::string& name) const {
        return getVariable<T>(name);
    }
    
    // 获取函数参数并转换为指定类型，带默认值
    template<typename T>
    T* getArgument(const std::string& name, T* defaultValue) const {
        T* result = getArgument<T>(name);
        return result ? result : defaultValue;
    }
    
    // 设置函数参数到当前作用域（统一绑定方式）
    void setArgument(const std::string& name, Value* value);
    
    // 设置函数参数到当前作用域（类型化版本）
    template<typename T>
    void setArgument(const std::string& name, T* value) {
        setArgument(name, static_cast<Value*>(value));
    }
    
    // ==================== 位置参数访问 ====================
    // 按索引获取参数（基于名称查找）
    Value* getArgumentByIndex(size_t index) const;
    
    // 按索引获取参数（类型化版本）
    template<typename T>
    T* getArgumentByIndex(size_t index) const {
        Value* value = getArgumentByIndex(index);
        if (value) {
            return dynamic_cast<T*>(value);
        }
        return nullptr;
    }
    
    // 获取所有参数（按顺序）
    std::vector<Value*> getAllArguments() const;
    
    // 获取参数数量
    size_t getArgumentCount() const;
    
    // 获取参数名称列表（用于函数原型匹配）
    std::vector<std::string> getParameterNames() const;
    
    // 获取有名字的函数实际参数（过滤系统变量）
    std::map<std::string, Value*> getNamedArgs() const;
    
    // 获取kwargs字典（可变参数支持）
    Dict* getKwargs() const;
    
    // 检查是否有kwargs
    bool hasKwargs() const;
    
    // 获取args数组（位置可变参数支持）
    Array* getArgs() const;
    
    // 检查是否有args
    bool hasArgs() const;
    
    // ==================== 可变参数支持 ====================
    // 注意：旧的setVarArgs机制已被新的*args和**kwargs机制替代
    // 新的机制提供更灵活和Python风格的可变参数支持
    
    // 检查是否有可变参数（兼容性方法）
    bool hasVarArgs() const;
    
    // ==================== 实例访问接口 ====================
    // 获取当前实例（this指针）
    template<typename T>
    T* getThis() const {
        return getVariable<T>("this");
    }
    
    // 获取当前实例（instance别名）
    template<typename T>
    T* getInstance() const {
        return getVariable<T>("instance");
    }
    
    // 获取当前实例，带类型检查
    template<typename T>
    T* requireThis() const {
        T* instance = getThis<T>();
        if (!instance) {
            // 可以在这里添加错误处理逻辑
            // 比如抛出异常或记录错误日志
        }
        return instance;
    }
};

// ==================== ScopeManager类 ====================
class ScopeManager {
private:
    std::vector<Scope*> scopes;
    Scope* currentScope;
    Scope* globalScope;

    // ==================== 私有辅助方法 ====================
    
    // 在作用域中直接查找可调用对象
    Value* findCallableInScope(const std::string& name);
    
    // 通过函数名在作用域中查找可调用对象
    Value* findCallableByName(const std::string& name);
    
    // 在单个作用域中通过函数名查找可调用对象
    Value* findCallableByNameInScope(Scope* scope, const std::string& name);
    
    // 在类型系统中查找构造函数或静态方法
    Value* findTypeConstructor(const std::string& name);
    
    // 在作用域中查找实例对象
    Value* findInstanceInScopes(const std::string& name);
    
    // ==================== 自动上下文管理辅助方法 ====================
    
    // 检查是否在类定义上下文中
    bool isInClassContext() const;
    
    // 检查是否在结构体定义上下文中
    bool isInStructContext() const;
    
    // 获取当前可见性
    VisibilityType getCurrentVisibility() const;
    
    // 自动注册为类方法
    void registerAsClassMethod(const std::string& name, Function* function);
    
    // 自动注册为类成员
    void registerAsClassMember(const std::string& name, Value* value);
    
    // 自动注册为结构体成员
    void registerAsStructMember(const std::string& name, Value* value);

public:
    // 构造函数和析构函数
    ScopeManager();
    ~ScopeManager();
    
    // 作用域管理
    void enterScope();
    void exitScope();
    
    // ==================== 统一查询接口 ====================
    
    // 主要查询方法 - 按优先级查找标识符
    Value* lookup(const std::string& name);
    
    // 类型化查询方法 - 明确指定要查找的类型
    ObjectType* lookupType(const std::string& name);
    Value* lookupVariable(const std::string& name);
    Value* lookupFunction(const std::string& name);
    Value* lookupMethod(const std::string& name);
    
    // 批量查询方法 - 一次性获取所有匹配的标识符
    struct LookupResult {
        ObjectType* type = nullptr;
        Value* variable = nullptr;
        Value* function = nullptr;
        Value* method = nullptr;
        Value* instance = nullptr;
        
        bool hasAny() const {
            return type || variable || function || method || instance;
        }
        
        string toString() const;
    };
    
    LookupResult lookupAll(const std::string& name);
    
    // ==================== 定义接口 ====================
    
    // 类型定义
    void defineType(const std::string& name, ObjectType* type);
    
    // 变量定义 - 自动处理类成员和普通变量
    void defineVariable(const std::string& name, Value* value);
    void defineVariable(const std::string& name, ObjectType* type, Value* value = nullptr);
    void defineVariable(const std::string& name, const std::string& type, Value* value = nullptr);
    
    // 函数定义 - 统一接口，自动处理类方法和普通函数
    void defineFunction(const std::string& name, Function* function);
    void defineFunction(const std::string& name, const std::vector<std::string>& parameterTypes, Value* function);
    
    // 方法定义
    void defineMethod(const std::string& name, Value* method);
    
    // 实例定义
    void defineInstance(const std::string& name, Value* instance);
    
    // ==================== 更新接口 ====================
    
    void updateVariable(const std::string& name, Value* value);
    
    // ==================== 检查接口 ====================
    
    bool has(const std::string& name) const;
    bool hasVariable(const std::string& name) const;
    bool hasFunction(const std::string& name) const;
    bool hasType(const std::string& name) const;
    
    // ==================== 上下文管理 ====================
    

    

    
    // ==================== 作用域信息 ====================
    
    Scope* getCurrentScope() const;
    Scope* getGlobalScope() const;
    size_t getScopeDepth() const;
    
    // ==================== 调试接口 ====================
    
    void printCurrentScope() const;
    void printAllScopes() const;
    
    // ==================== 兼容性接口（保留但标记为过时） ====================
    
    // @deprecated 使用 lookup() 替代
    Value* lookupAny(const std::string& name);
    
    // @deprecated 使用 lookupAll() 替代
    std::tuple<ObjectType*, Value*, Value*, Value*> lookupIdentifier(const std::string& name);
    
    // @deprecated 使用 defineFunction() 替代
    void defineCallable(const std::string& name, Value* callable);
    Value* lookupCallable(const std::string& name);
    bool isCallableDefinedInCurrentScope(const std::string& name) const;
    
    // @deprecated 使用 lookupFunction() 替代
    Value* lookupFunction(const std::string& name, const std::vector<Value*>& args);
    Value* lookupFunction(const FunctionSignature& signature);
    
    // @deprecated 使用 lookupMethod() 替代
    Value* lookupTypeMethod(const std::string& typeName, const std::string& methodName);
    Value* lookupTypeStaticMethod(const std::string& typeName, const std::string& methodName);
    Value* lookupTypeInstanceMethod(const std::string& typeName, const std::string& methodName);
    
    // @deprecated 使用 defineFunction() 替代
    void defineUserFunction(const std::string& name, UserFunction* userFunc);
    void defineUserFunction(const std::string& name, const std::vector<std::string>& parameterTypes, UserFunction* userFunc);
    
    // @deprecated 使用 has() 替代
    bool isIdentifier(const std::string& name) const;
    bool isVariableDefinedInCurrentScope(const std::string& name) const;
    bool isFunctionDefinedInCurrentScope(const std::string& name) const;
    bool isTypeDefinedInCurrentScope(const std::string& name) const;
    bool isInstanceDefinedInCurrentScope(const std::string& name) const;
};



#endif // SCOPE_H