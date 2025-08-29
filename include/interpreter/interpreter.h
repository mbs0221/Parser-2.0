#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "parser/inter.h"
#include "parser/parser.h"
#include "interpreter/control_flow.h"
#include "parser/ast_visitor.h"
#include "interpreter/scope.h"
#include "interpreter/builtin_plugin.h"
#include "parser/function.h"
#include "lexer/value.h"
#include "parser/expression.h"
#include "parser/statement.h"
#include <string>
#include <list>
#include <map>
#include <vector>
#include <functional>
#include <iostream>
#include <sstream>
#include <typeinfo>

using namespace std;

// 解释器类 - 负责AST的求值和作用域管理
class Interpreter : public ASTVisitor {
private:
    // 作用域管理器
    ScopeManager scopeManager;
    
    // 插件管理器
    PluginManager pluginManager;
    
    // 调用栈（用于调试和错误报告）
    vector<string> callStack;

public:
    Interpreter();
    ~Interpreter();
    
    // AST求值方法
    void execute(Statement* stmt);
    void execute(Program* program);
    
    // ASTVisitor接口实现 - 表达式访问方法
    Value* visit(Expression* expr) override;
    Value* visit(ConstantExpression* expr) override;
    Value* visit(VariableExpression* expr) override;
    Value* visit(UnaryExpression* expr) override;
    Value* visit(BinaryExpression* expr) override;
    Value* visit(AssignExpression* expr) override;
    // 泛型CastExpression的访问方法
    Value* visit(CastExpression<Integer>* expr) override;
    Value* visit(CastExpression<Double>* expr) override;
    Value* visit(CastExpression<Bool>* expr) override;
    Value* visit(CastExpression<Char>* expr) override;
    Value* visit(CastExpression<String>* expr) override;
    Value* visit(AccessExpression* expr) override;
    Value* visit(CallExpression* expr) override;
    Value* visit(MethodCallExpression* expr) override;

    // ASTVisitor接口实现 - 语句访问方法
    void visit(Statement* stmt) override;
    void visit(ImportStatement* stmt) override;
    void visit(ExpressionStatement* stmt) override;
    void visit(VariableDeclaration* stmt) override;
    void visit(IfStatement* stmt) override;
    void visit(WhileStatement* stmt) override;
    void visit(ForStatement* stmt) override;
    void visit(DoWhileStatement* stmt) override;
    void visit(BlockStatement* stmt) override;
    void visit(UserFunction* stmt) override;
    void visit(StructDefinition* stmt) override;
    void visit(ClassDefinition* stmt) override;
    void visit(BreakStatement* stmt) override;
    void visit(ContinueStatement* stmt) override;
    void visit(ReturnStatement* stmt) override;
    void visit(TryStatement* stmt) override;

    void visit(SwitchStatement* stmt) override;
    // CaseStatement和DefaultStatement的visit方法已移除，合并到SwitchStatement中
    void visit(FunctionPrototype* stmt) override;

    // ASTVisitor接口实现 - function.h中定义的类的访问方法
    void visit(Identifier* id) override;
    void visit(Variable* var) override;

    // ASTVisitor接口实现 - 程序访问方法
    void visit(Program* program) override;
    
    // 插件管理
    void loadPlugin(const string& pluginPath);
    void unloadPlugin(const string& pluginName);
    vector<string> getLoadedPlugins() const;
    
private:
    // 自动加载默认插件
    void loadDefaultPlugins();
    void loadDefaultPlugins(const string& pluginDir);
    
    // 辅助函数
    string extractPluginName(const string& filePath);
    string join(const vector<string>& vec, const string& delimiter);
    
    // 类/结构体实例化
    Value* instantiateClass(ClassDefinition* classDef, vector<Value*>& args);
    Value* instantiateStruct(StructDefinition* structDef, vector<Value*>& args);
    Value* createDefaultValue(Type* type);

    // 类型转换辅助方法
    template<typename T>
    Value* calculate_unary_casted(Value* value, int opTag);
    Value* calculate_unary_compatible(Value* value, int opTag);
    
    // 一元操作的类型转换辅助方法
    template<typename T>
    Value* calculate_binary_casted(Value* left, Value* right, int opTag);
    Value* calculate_binary_compatible(Value* left, Value* right, int opTag);
    
    // 赋值操作的类型转换辅助方法
    template<typename T>
    Value* calculate_assign_casted(Value* left, Value* right, int opTag);
    Value* calculate_assign_compatible(Value* left, Value* right, int opTag);
    
    // 泛型计算方法 - 返回Value*类型
    template<typename T>
    Value* calculate(Value* left, Value* right, int op);
    template<typename T>
    Value* calculate(Value* value, int op);
    
    // 作用域管理辅助函数 - 处理有返回值的函数
    template<typename Func>
    auto withScope(Func func) -> decltype(func()) {
        scopeManager.enterScope();
        try {
            auto result = func();
            scopeManager.exitScope();
            return result;
        } catch (...) {
            scopeManager.exitScope();
            throw;
        }
    }
    
    // 作用域管理辅助函数 - 处理无返回值的函数
    template<typename Func>
    void withScopeVoid(Func func) {
        scopeManager.enterScope();
        try {
            func();
            scopeManager.exitScope();
        } catch (...) {
            scopeManager.exitScope();
            throw;
        }
    }
    
    // 错误处理
    void reportError(const string& message);
    void reportTypeError(const string& expected, const string& actual);
};

#endif // INTERPRETER_H
