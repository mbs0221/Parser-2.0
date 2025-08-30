#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "parser/inter.h"
#include "interpreter/control_flow.h"
#include "parser/ast_visitor.h"
#include "interpreter/scope.h"
#include "interpreter/builtin_plugin.h"
#include "interpreter/type_registry.h"
#include "interpreter/builtin_type.h"
#include "interpreter/type_converter.h"
#include "parser/function.h"
#include "interpreter/value.h"
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
class Interpreter : public ASTVisitor<Value*> {
public:
    // 作用域管理器
    ScopeManager scopeManager;
    
    // 插件管理器
    PluginManager pluginManager;
    
    // 类型注册表
    TypeRegistry* typeRegistry;

    // 对象工厂
    ObjectFactory* objectFactory;
    
    // 类型转换器
    TypeConverter* typeConverter;
    
    // 调用栈（用于调试和错误报告）
    vector<string> callStack;

public:
    Interpreter();
    Interpreter(bool loadPlugins); // 新增构造函数，可以选择是否加载插件
    ~Interpreter();
    
    // ASTVisitor接口实现 - 表达式访问方法
    Value* visit(Expression* expr) override;
    Value* visit(ConstantExpression<int>* expr) override;
    Value* visit(ConstantExpression<double>* expr) override;
    Value* visit(ConstantExpression<bool>* expr) override;
    Value* visit(ConstantExpression<char>* expr) override;
    Value* visit(ConstantExpression<string>* expr) override;
    Value* visit(VariableExpression* expr) override;
    Value* visit(UnaryExpression* expr) override;
    Value* visit(BinaryExpression* expr) override;
    Value* visit(AssignExpression* expr) override;
    // CastExpression的访问方法
    Value* visit(CastExpression* expr) override;
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
    void visit(FunctionPrototype* stmt) override;
    void visit(StructDefinition* stmt) override;
    void visit(ClassDefinition* stmt) override;
    void visit(BreakStatement* stmt) override;
    void visit(ContinueStatement* stmt) override;
    void visit(ReturnStatement* stmt) override;
    void visit(TryStatement* stmt) override;

    void visit(SwitchStatement* stmt) override;
    // CaseStatement和DefaultStatement的visit方法已移除，合并到SwitchStatement中
    void visit(FunctionDefinition* stmt) override;
    void visit(ClassMethod* method) override;

    // ASTVisitor接口实现 - function.h中定义的类的访问方法
    void visit(Identifier* id) override;
    void visit(Variable* var) override;

    // ASTVisitor接口实现 - 程序访问方法
    void visit(Program* program) override;
    
    // 插件管理
    void loadPlugin(const string& pluginPath);
    void unloadPlugin(const string& pluginName);
    vector<string> getLoadedPlugins() const;
    void loadDefaultPlugins();
    void loadDefaultPlugins(const string& pluginDir);
    
    // 类型系统相关方法
    string getValueTypeName(Value* value);
    Value* callTypeMethod(Value* instance, const string& methodName, vector<Value*>& args);
    
    // 函数调用辅助方法
    Value* executeBuiltinFunction(BuiltinFunctionWrapper* builtinFunc, vector<Value*>& evaluatedArgs);
    Value* executeUserFunction(UserFunctionWrapper* userFunc, vector<Value*>& evaluatedArgs);
    
    // 对象工厂访问方法
    ObjectFactory* getObjectFactory() const { return objectFactory; }
    
    // 类型转换器访问方法
    TypeConverter* getTypeConverter() const { return typeConverter; }
    
    // 辅助方法
    Expression* createExpressionFromValueImpl(Value* value);
    template<typename T>
    Expression* createExpressionFromValue(Value* value);
    
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
    
private:
    
    // 辅助函数
    string extractPluginName(const string& filePath);
    string join(const vector<string>& vec, const string& delimiter);

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
    
    // 错误处理
    void reportError(const string& message);
    void reportTypeError(const string& expected, const string& actual);
};

#endif // INTERPRETER_H
