#ifndef SCOPE_H
#define SCOPE_H

#include "parser/inter.h"
#include "lexer/value.h"
#include "parser/function.h"
#include <string>
#include <map>
#include <list>
#include <vector>
#include <iostream>
#include <functional>

using namespace std;

// 前向声明
class StructDefinition;
class ClassDefinition;

// ==================== 作用域结构 ====================
// 作用域结构 - 统一管理所有标识符
struct Scope {
    map<string, Identifier*> identifiers;  // 统一管理所有标识符（变量、函数、结构体、类）
    
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
    void updateVariable(const string& name, Value* value);
    Variable* lookupVariable(const string& name);
    
    // 标识符管理
    void defineIdentifier(const string& name, Identifier* identifier);
    Identifier* lookupIdentifier(const string& name);
    bool isIdentifier(const string& name) const;
    
    // 便捷方法
    void defineVariable(const string& name, const string& type, Value* value = nullptr);
    void defineFunction(const string& name, UserFunction* func);
    void defineStruct(const string& name, StructDefinition* structDef);
    void defineClass(const string& name, ClassDefinition* classDef);
    
    // 查找方法
    StructDefinition* lookupStruct(const string& name);
    ClassDefinition* lookupClass(const string& name);
    
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
