#ifndef SCOPE_H
#define SCOPE_H

#include "interpreter/value.h"
#include "interpreter/builtin_type.h"
#include <string>
#include <map>
#include <list>
#include <vector>
#include <iostream>
#include <functional>

using namespace std;

// 前向声明
struct FunctionDefinition;
class ReturnException;

// 避免循环依赖的函数指针类型定义
using ScopeManagerPtr = void*;
using ExecuteFunctionPtr = Value* (*)(ScopeManagerPtr, FunctionDefinition*, vector<Value*>&);



// 内置函数类型定义
typedef Value* (*BuiltinFunctionPtr)(vector<Value*>&);

// 内置函数类
class BuiltinFunction {
public:
    string name;
    BuiltinFunctionPtr func;
    
    BuiltinFunction(const string& funcName, BuiltinFunctionPtr funcPtr)
        : name(funcName), func(funcPtr) {}
    
    ~BuiltinFunction() = default;
    
    // 执行函数
    Value* call(vector<Value*>& args) {
        return func(args);
    }
    
    // 获取函数名
    string getName() const {
        return name;
    }
};

// ==================== Interpreter自己的标识符类型 ====================
// 运行时标识符基类
class RuntimeIdentifier {
public:
    string name;
    
    RuntimeIdentifier(const string& identifierName) : name(identifierName) {}
    virtual ~RuntimeIdentifier() = default;
    
    // 获取标识符类型
    virtual string getIdentifierType() const = 0;
};

// 运行时变量 - 存储实际的Value对象
class RuntimeVariable : public RuntimeIdentifier {
public:
    Value* value;
    ObjectType* type;  // 使用interpreter的类型系统
    
    RuntimeVariable(const string& varName, Value* val = nullptr, ObjectType* varType = nullptr)
        : RuntimeIdentifier(varName), value(val), type(varType) {}
    
    string getIdentifierType() const override {
        return "RuntimeVariable";
    }
    
    // 设置值
    void setValue(Value* val) {
        if (value) {
            delete value;  // 清理旧值
        }
        value = val;
    }
    
    // 获取值
    Value* getValue() const {
        return value;
    }
    
    // 获取类型
    ObjectType* getType() const {
        return type;
    }
    
    ~RuntimeVariable() {
        if (value) {
            delete value;
        }
    }
};

// 运行时函数基类 - 统一内置函数和用户函数
class RuntimeFunction : public RuntimeIdentifier {
public:
    RuntimeFunction(const string& funcName) : RuntimeIdentifier(funcName) {}
    virtual ~RuntimeFunction() = default;
    
    string getIdentifierType() const override {
        return "RuntimeFunction";
    }
    
    // 函数调用接口
    // 参数列表本身不会被修改，但参数指向的值可以被修改
    virtual Value* call(vector<Value*>& args) = 0;
    
    // 获取参数数量（-1表示可变参数）
    virtual int getParameterCount() const = 0;
    
    // 获取函数类型
    virtual string getFunctionType() const = 0;
};

// 内置函数包装器
class BuiltinFunctionWrapper : public RuntimeFunction {
private:
    BuiltinFunction* function;
    
public:
    BuiltinFunctionWrapper(const string& funcName, BuiltinFunction* func)
        : RuntimeFunction(funcName), function(func) {}
    
    Value* call(vector<Value*>& args) override {
        return function->call(args);
    }
    
    int getParameterCount() const override {
        return -1; // 内置函数通常是可变参数
    }
    
    string getFunctionType() const override {
        return "BuiltinFunction";
    }
    
    BuiltinFunction* getFunction() const {
        return function;
    }
};

// 用户函数包装器
class UserFunctionWrapper : public RuntimeFunction {
private:
    FunctionDefinition* astDefinition;  // 直接存储AST定义
    int paramCount;
    
    // 使用函数指针替代直接依赖
    ScopeManagerPtr scopeManagerPtr;
    ExecuteFunctionPtr executeFunction;
    
public:
    UserFunctionWrapper(const string& funcName, FunctionDefinition* def, int params, 
                       ScopeManagerPtr scopeMgr, ExecuteFunctionPtr execFunc)
        : RuntimeFunction(funcName), astDefinition(def), paramCount(params), 
          scopeManagerPtr(scopeMgr), executeFunction(execFunc) {}
    
    Value* call(vector<Value*>& args) override {
        if (!executeFunction || !astDefinition) {
            return nullptr;
        }
        
        // 通过函数指针执行，避免直接依赖
        return executeFunction(scopeManagerPtr, astDefinition, args);
    }
    
    int getParameterCount() const override {
        return paramCount;
    }
    
    string getFunctionType() const override {
        return "UserFunction";
    }
};

// 运行时类型定义 - 已移除，类型管理由TypeRegistry负责
// 类型信息应该通过TypeRegistry统一管理，而不是在scope中重复存储

// ==================== 作用域结构 ====================
// 作用域结构 - 统一管理所有运行时标识符
struct Scope {
    map<string, RuntimeIdentifier*> identifiers;  // 统一管理所有运行时标识符
    
    Scope() {}
    
    // 清理作用域中的资源
    void cleanup();
    
    // 打印作用域内容（用于调试）
    void print() const;
};

// ==================== 作用域管理器 ====================
// 作用域管理器类 - 负责作用域的创建、销毁和查找
class ScopeManager {
private:
    // 作用域栈 - 管理变量和函数的作用域
    list<Scope*> scopes;
    
    // 当前作用域
    Scope* currentScope;
    
    // 全局作用域（程序级别的变量和函数）
    Scope* globalScope;

public:
    // 构造函数和析构函数
    ScopeManager();
    ~ScopeManager();
    
    // 作用域管理
    void enterScope();
    void exitScope();
    
    // 变量管理
    void defineVariable(const string& name, Value* value);
    void defineVariable(const string& name, ObjectType* type, Value* value = nullptr);
    void updateVariable(const string& name, Value* value);
    RuntimeVariable* lookupVariable(const string& name);
    
    // 函数管理 - 统一内置函数和用户函数
    void defineFunction(const string& name, BuiltinFunction* function);
    void defineUserFunction(const string& name, FunctionDefinition* funcDef, int paramCount, 
                           ScopeManagerPtr scopeMgr, ExecuteFunctionPtr execFunc);
    RuntimeFunction* lookupFunction(const string& name);
    
    // 通用标识符管理
    void defineIdentifier(const string& name, RuntimeIdentifier* identifier);
    RuntimeIdentifier* lookupIdentifier(const string& name);
    bool isIdentifier(const string& name) const;
    
    // 便捷方法
    void defineVariable(const string& name, const string& type, Value* value = nullptr);
    
    // 获取当前作用域
    Scope* getCurrentScope() const { return currentScope; }
    
    // 获取全局作用域
    Scope* getGlobalScope() const { return globalScope; }
    
    // 获取作用域深度
    size_t getScopeDepth() const { return scopes.size(); }
    
    // 调试方法
    void printCurrentScope() const;
    void printAllScopes() const;
    
    // 检查变量是否在当前作用域中定义
    bool isVariableDefinedInCurrentScope(const string& name) const;
    
    // 检查函数是否在当前作用域中定义
    bool isFunctionDefinedInCurrentScope(const string& name) const;
};

#endif // SCOPE_H