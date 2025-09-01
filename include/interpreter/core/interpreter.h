#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "parser/inter.h"
#include "parser/definition.h"
#include "parser/expression.h"
#include "parser/statement.h"
#include "parser/ast_visitor.h"
#include "interpreter/core/control_flow.h"
#include "interpreter/core/function_call.h"
#include "interpreter/scope/scope.h"
#include "interpreter/plugins/builtin_plugin.h"
#include "interpreter/types/types.h"
#include "interpreter/values/calculate.h"
#include "interpreter/values/value.h"

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
    
    // 计算器实例
    Calculator* calculator;

    // 调用栈（用于调试和错误报告）
    std::vector<std::string> callStack;

public:
    // 构造函数
    Interpreter();
    Interpreter(bool loadPlugins); // 新增构造函数，可以选择是否加载插件
    // 析构函数
    ~Interpreter();
    
    // ASTVisitor接口实现 - 表达式访问方法
    Value* visit(Expression* expr) override;
    Value* visit(ConstantExpression<int>* expr) override;
    Value* visit(ConstantExpression<double>* expr) override;
    Value* visit(ConstantExpression<bool>* expr) override;
    Value* visit(ConstantExpression<char>* expr) override;
    Value* visit(ConstantExpression<std::string>* expr) override;
    Value* visit(VariableExpression* expr) override;
    Value* visit(UnaryExpression* expr) override;
    Value* visit(BinaryExpression* expr) override;
    // IncDecExpression已合并到UnaryExpression中
    // CastExpression的访问方法
    Value* visit(CastExpression* expr) override;
    Value* visit(AccessExpression* expr) override;
    Value* visit(CallExpression* expr) override;
    Value* visit(TernaryExpression* expr) override;
    
    // ASTVisitor接口实现 - 语句访问方法
    void visit(Statement* stmt) override;
    void visit(ImportStatement* stmt) override;
    void visit(ExpressionStatement* stmt) override;
    void visit(IfStatement* stmt) override;
    void visit(WhileStatement* stmt) override;
    void visit(ForStatement* stmt) override;
    void visit(DoWhileStatement* stmt) override;
    void visit(BlockStatement* stmt) override;
    void visit(BreakStatement* stmt) override;
    void visit(ContinueStatement* stmt) override;
    void visit(ReturnStatement* stmt) override;
    void visit(TryStatement* stmt) override;
    void visit(SwitchStatement* stmt) override;

    void visit(VariableDefinition* stmt) override;
    void visit(FunctionPrototype* stmt) override;
    void visit(FunctionDefinition* stmt) override;
    void visit(StructDefinition* stmt) override;
    void visit(ClassMember* member) override;
    void visit(ClassMethod* method) override;
    void visit(ClassDefinition* stmt) override;
    

    // ASTVisitor接口实现 - 程序访问方法
    void visit(Program* program) override;
    
    // ==================== AccessExpression辅助方法 ====================
    
    // 提取成员名称
    string extractMemberName(Value* key);
    
    // 插件管理
    void loadPlugin(const std::string& pluginPath);
    void unloadPlugin(const std::string& pluginName);
    std::vector<std::string> getLoadedPlugins() const;
    void loadDefaultPlugins();
    void loadDefaultPlugins(const std::string& pluginDir);
    
    // 类型系统相关方法
    std::string getValueTypeName(Value* value);
    
    // 对象工厂访问方法
    ObjectFactory* getObjectFactory() const { return objectFactory; }
    
    // 类型工厂访问方法
    
    // 辅助方法声明
    void processMemberPrototypes(ObjectType* type, const std::vector<ClassMember*>& members);
    void processStructDefinition(StructDefinition* structDef);
    void processStructDefinition(ClassDefinition* classDef);
    
    // 自增自减操作处理
    Value* handleIncrementDecrement(UnaryExpression* unary);
    
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
    std::string extractPluginName(const std::string& filePath);

    // 通用计算方法 - 通过类型系统调用运算符方法
    Value* calculate_binary(Value* left, Value* right, int op);
    Value* calculate_unary(Value* operand, int op);
    
    // 实例错误报告函数
    void reportError(const std::string& message);
    void reportTypeError(const std::string& expected, const std::string& actual);
};

#endif // INTERPRETER_H
