#ifndef INTERPRETER_FUNCTION_CALL_H
#define INTERPRETER_FUNCTION_CALL_H

#include <string>
#include <vector>
#include "interpreter/values/value.h"

// using namespace std; // 已移除，使用显式std前缀

// 前向声明
class Value;
class Scope;
class ClassType;
class Interpreter;
class Function;

// 函数调用基类
class FunctionCall {
protected:
    Scope* currentScope;
    const std::vector<Value*>& args;
    
public:
    FunctionCall(Scope* scope, const std::vector<Value*>& arguments);
    virtual ~FunctionCall() = default;
    
    // 纯虚函数：执行函数调用
    virtual Value* execute() = 0;
    
    // 设置当前作用域
    void setCurrentScope(Scope* scope);
    
    // 获取当前作用域
    Scope* getCurrentScope() const;
    
protected:
    // 绑定参数到作用域（子类可以重写）
    virtual void bindParameters(const std::vector<Value*>& args) {}
};

// 普通函数调用（无特殊上下文）
class BasicFunctionCall : public FunctionCall {
protected:
    Function* function;
    
public:
    BasicFunctionCall(Scope* scope, Function* func, const std::vector<Value*>& arguments);
    
    // 实现基类的execute方法
    Value* execute() override;
    
    // 保护方法：获取函数对象
    Function* getFunction() const { return function; }
    
protected:
    // 重写参数绑定方法
    void bindParameters(const std::vector<Value*>& args) override;
};

// 静态方法调用（类上下文）- 继承BasicFunctionCall，增加静态成员注入
class StaticMethodCall : public BasicFunctionCall {
private:
    ObjectType* objectType;
    
public:
    StaticMethodCall(Scope* scope, ObjectType* cls, Function* method, const std::vector<Value*>& arguments);
    
    // 重写execute方法，先注入静态成员，再调用基类方法
    Value* execute() override;
    
private:
    // 注入静态成员到作用域
    void injectStaticMembers();
};

// 实例方法调用（实例上下文）- 继承BasicFunctionCall，增加this指针设置
class InstanceMethodCall : public BasicFunctionCall {
private:
    Value* instance;
    MethodReference* methodRef;
    
public:
    // 构造函数：接受MethodReference对象
    InstanceMethodCall(Scope* scope, Value* inst, MethodReference* methodRef, const std::vector<Value*>& arguments);
    
    // 重写execute方法，先绑定实例到作用域，再委托给方法引用执行
    Value* execute() override;
    
private:
    // 绑定实例到作用域变量
    void bindInstanceToScope();
};

// ==================== 函数执行器接口 ====================
// 用于执行用户定义的函数体，避免循环依赖
class FunctionExecutor {
public:
    virtual ~FunctionExecutor() = default;
    
    // 执行用户函数体
    // functionBody: 函数体语句
    // 返回: 函数执行结果
    // 注意：参数绑定由FunctionCall机制处理，不需要args参数
    virtual class Value* executeFunction(class Statement* functionBody) = 0;
    
    // 获取执行器名称（用于调试）
    virtual std::string getExecutorName() const = 0;
};

// ==================== Interpreter函数执行器 ====================
// 实现FunctionExecutor接口，包装Interpreter的功能
class InterpreterFunctionExecutor : public FunctionExecutor {
private:
    Interpreter* interpreterPtr;  // Interpreter指针

public:
    // 构造函数
    InterpreterFunctionExecutor(Interpreter* interp);
    
    // 实现FunctionExecutor接口
    class Value* executeFunction(class Statement* functionBody) override;
    
    // 获取执行器名称
    std::string getExecutorName() const override;
    
    // 设置Interpreter指针
    void setInterpreter(Interpreter* interp);
    
    // 获取Interpreter指针
    Interpreter* getInterpreter() const;
};

#endif // INTERPRETER_FUNCTION_CALL_H
