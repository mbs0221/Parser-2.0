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
    
    // this指针 - 指向当前实例对象（用于类方法中）
    Value* thisPointer;
    
    // 当前类上下文 - 指向当前类定义（用于类方法定义中）
    ClassType* currentClassContext;
    
    // 构造函数
    Scope();
    
    // 析构函数
    ~Scope();
    
    // 清理作用域中的资源
    void cleanup();
    
    // 打印作用域内容（用于调试）
    void print() const;
    
    // 获取类型管理器
    TypeRegistry* getTypeRegistry() const;
    
    // 获取对象管理器
    ObjectRegistry* getObjectRegistry() const;
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
    
    // 变量定义
    void defineVariable(const std::string& name, Value* value);
    void defineVariable(const std::string& name, ObjectType* type, Value* value = nullptr);
    void defineVariable(const std::string& name, const std::string& type, Value* value = nullptr);
    
    // 函数定义 - 统一接口
    void defineFunction(const std::string& name, Value* function);
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
    
    // this指针管理
    void setThisPointer(Value* thisPtr);
    Value* getThisPointer() const;
    void clearThisPointer();
    
    // 类上下文管理
    void setCurrentClassContext(ClassType* classType);
    ClassType* getCurrentClassContext() const;
    void clearCurrentClassContext();
    
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