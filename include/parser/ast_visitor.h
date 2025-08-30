#ifndef AST_VISITOR_H
#define AST_VISITOR_H

#include <string>
#include "parser/statement.h"
#include "parser/expression.h"

// 前向声明
class AST;
class Expression;
class Statement;
class Program;
class Value;

// 表达式类型
template<typename T>
class ConstantExpression;
class VariableExpression;
class UnaryExpression;
class BinaryExpression;
class AssignExpression;
class AccessExpression;
class CallExpression;
class MethodCallExpression;

// 泛型CastExpression的前向声明
class CastExpression;

// 语句类型
class ImportStatement;
class ExpressionStatement;
class VariableDeclaration;
class IfStatement;
class WhileStatement;
class ForStatement;
class DoWhileStatement;
class BlockStatement;
class FunctionDefinition;
class StructDefinition;
class ClassDefinition;
class BreakStatement;
class ContinueStatement;
class ReturnStatement;
class TryStatement;
class SwitchStatement;
class FunctionPrototype;

// function.h中定义的类
class Identifier;
class Variable;
class ClassMethod;

// 语句访问者接口 - 不需要返回值
class StatementVisitor {
public:
    virtual ~StatementVisitor() = default;

    // 语句访问方法 - 无返回值
    virtual void visit(Statement* stmt) = 0;
    virtual void visit(ImportStatement* stmt) = 0;
    virtual void visit(ExpressionStatement* stmt) = 0;
    virtual void visit(VariableDeclaration* stmt) = 0;
    virtual void visit(IfStatement* stmt) = 0;
    virtual void visit(WhileStatement* stmt) = 0;
    virtual void visit(ForStatement* stmt) = 0;
    virtual void visit(DoWhileStatement* stmt) = 0;
    virtual void visit(BlockStatement* stmt) = 0;
    virtual void visit(StructDefinition* stmt) = 0;
    virtual void visit(ClassDefinition* stmt) = 0;
    virtual void visit(BreakStatement* stmt) = 0;
    virtual void visit(ContinueStatement* stmt) = 0;
    virtual void visit(ReturnStatement* stmt) = 0;
    virtual void visit(TryStatement* stmt) = 0;
    virtual void visit(SwitchStatement* stmt) = 0;
    virtual void visit(FunctionPrototype* stmt) = 0;
    
    // function.h中定义的类的访问方法
    virtual void visit(Identifier* id) = 0;
    virtual void visit(Variable* var) = 0;
    virtual void visit(FunctionDefinition* func) = 0;
    virtual void visit(ClassMethod* method) = 0;
    
    // 程序根节点访问方法
    virtual void visit(Program* program) = 0;
};

// 泛型表达式访问者接口 - 类似于Java的ExpressionVisitor<T>
template<typename ReturnType>
class ExpressionVisitor {
public:
    virtual ~ExpressionVisitor() = default;
    
    // 表达式访问方法 - 返回ReturnType
    virtual ReturnType visit(Expression* expr) = 0;
    virtual ReturnType visit(ConstantExpression<int>* expr) = 0;
    virtual ReturnType visit(ConstantExpression<double>* expr) = 0;
    virtual ReturnType visit(ConstantExpression<bool>* expr) = 0;
    virtual ReturnType visit(ConstantExpression<char>* expr) = 0;
    virtual ReturnType visit(ConstantExpression<std::string>* expr) = 0;
    virtual ReturnType visit(VariableExpression* expr) = 0;
    virtual ReturnType visit(UnaryExpression* expr) = 0;
    virtual ReturnType visit(BinaryExpression* expr) = 0;
    virtual ReturnType visit(AssignExpression* expr) = 0;
    virtual ReturnType visit(CastExpression* expr) = 0;
    virtual ReturnType visit(AccessExpression* expr) = 0;
    virtual ReturnType visit(CallExpression* expr) = 0;
    virtual ReturnType visit(MethodCallExpression* expr) = 0;
};

// 完整的AST访问者接口 - 继承自StatementVisitor和ExpressionVisitor
template<typename ReturnType>
class ASTVisitor : public StatementVisitor, public ExpressionVisitor<ReturnType> {
public:
    virtual ~ASTVisitor() = default;
};

#endif // AST_VISITOR_H