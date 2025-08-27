#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "parser/inter.h"
#include "parser/parser.h"
#include "interpreter/control_flow.h"
#include "parser/ast_visitor.h"
#include "interpreter/scope.h"
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
    // ArrayNode、DictNode、StringLiteral的visit方法已移除，使用value.h中的Array、Dict、String
    Value* visit(AccessExpression* expr) override;
    Value* visit(CallExpression* expr) override;
    // StructInstantiationExpression和ClassInstantiationExpression的visit方法已移除，使用CallExpression
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
    // CatchStatement和FinallyStatement的visit方法已移除，合并到TryStatement中
    void visit(SwitchStatement* stmt) override;
    // CaseStatement和DefaultStatement的visit方法已移除，合并到SwitchStatement中
    void visit(FunctionPrototype* stmt) override;

    // ASTVisitor接口实现 - function.h中定义的类的访问方法
    void visit(Identifier* id) override;
    void visit(Variable* var) override;
    void visit(BuiltinFunction* func) override;
    void visit(UserFunction* func) override;
    void visit(ClassMethod* method) override;

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
