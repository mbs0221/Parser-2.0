#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "inter.h"
#include "parser.h"
#include "control_flow.h"
#include "ast_visitor.h"
#include "scope.h"
#include "function.h"
#include "value.h"
#include "expression.h"
#include "statement.h"
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
    
    // 调用栈（用于调试和错误报告）
    vector<string> callStack;

public:
    Interpreter();
    ~Interpreter();
    
    // AST求值方法
    void visit(AST* node);
    void visit(Expression* expr);
    void execute(Statement* stmt);
    void execute(Program* program);

    // ASTVisitor接口实现 - 表达式访问方法
    Value* visit(ConstantExpression* expr) override;
    Value* visit(VariableExpression* expr) override;
    Value* visit(UnaryExpression* expr) override;
    Value* visit(BinaryExpression* expr) override;
    // 赋值表达式现在使用BinaryExpression处理
    Value* visit(CastExpression* expr) override;
    Value* visit(ArrayNode* expr) override;
    Value* visit(DictNode* expr) override;
    Value* visit(StringLiteral* expr) override;
    Value* visit(AccessExpression* expr) override;
    Value* visit(CallExpression* expr) override;
    Value* visit(StructInstantiationExpression* expr) override;
    Value* visit(ClassInstantiationExpression* expr) override;
    Value* visit(MemberAccessExpression* expr) override;
    Value* visit(MethodCallExpression* expr) override;

    // ASTVisitor接口实现 - 语句访问方法
    void visit(ImportStatement* stmt) override;
    void visit(ExpressionStatement* stmt) override;
    void visit(VariableDeclaration* stmt) override;
    void visit(IfStatement* stmt) override;
    void visit(WhileStatement* stmt) override;
    void visit(ForStatement* stmt) override;
    void visit(DoWhileStatement* stmt) override;
    void visit(BlockStatement* stmt) override;
    void visit(FunctionDefinition* stmt) override;
    void visit(StructDefinition* stmt) override;
    void visit(ClassDefinition* stmt) override;
    void visit(BreakStatement* stmt) override;
    void visit(ContinueStatement* stmt) override;
    void visit(ReturnStatement* stmt) override;
    void visit(ThrowStatement* stmt) override;
    void visit(TryStatement* stmt) override;
    void visit(CatchStatement* stmt) override;
    void visit(FinallyStatement* stmt) override;
    void visit(SwitchStatement* stmt) override;
    void visit(CaseStatement* stmt) override;
    void visit(DefaultStatement* stmt) override;
    void visit(FunctionPrototype* stmt) override;

    // ASTVisitor接口实现 - 程序访问方法
    void visit(Program* program) override;
    
    // 内置函数管理
    bool isBuiltinFunction(const string& funcName);
    Value* executeBuiltinFunction(const string& funcName, vector<Expression*>& args);
    void registerBuiltinFunctionsToScope();

    // 类型转换辅助方法
    string determineTargetType(Value* left, Value* right, Operator* op);
    int getTypePriority(Value* value);
    string getTypeName(Value* value);
    
    // 计算方法 - 使用函数重载，返回Value类型
    Value* calculate(Integer* left, Integer* right, int op);
    Value* calculate(Double* left, Double* right, int op);
    Value* calculate(Bool* left, Bool* right, int op);
    Value* calculate(Char* left, Char* right, int op);
    Value* calculate(String* left, String* right, int op);
    
    // 错误处理
    void reportError(const string& message);
    void reportTypeError(const string& expected, const string& actual);
};

#endif // INTERPRETER_H
