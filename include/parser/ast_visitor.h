#ifndef AST_VISITOR_H
#define AST_VISITOR_H

#include <string>
#include "lexer/value.h"

// 前向声明
class AST;
class Expression;
class Statement;
class Program;
class Value;

// 表达式类型
class ConstantExpression;
class VariableExpression;
class UnaryExpression;
class BinaryExpression;
class AssignExpression;
class AccessExpression;
class CallExpression;
class MethodCallExpression;

// 泛型CastExpression的前向声明
template<typename T> class CastExpression;

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
class ThrowStatement;
class TryStatement;
// CatchStatement和FinallyStatement已合并到TryStatement中
class SwitchStatement;
// CaseStatement和DefaultStatement已合并到SwitchStatement中
class FunctionPrototype;

// function.h中定义的类
class Identifier;
class Variable;
class BuiltinFunction;
class UserFunction;
class ClassMethod;

// 通用AST访问者接口 - 表达式返回Value类型，语句保持void返回类型
class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    
    // 语句访问方法 - 无返回值
    virtual void visit(Statement* stmt) = 0;
    
    // 表达式访问方法 - 返回Value类型
    virtual Value* visit(Expression* expr) = 0;
    virtual Value* visit(ConstantExpression* expr) = 0;
    virtual Value* visit(VariableExpression* expr) = 0;
    virtual Value* visit(UnaryExpression* expr) = 0;
    virtual Value* visit(BinaryExpression* expr) = 0;
    virtual Value* visit(AssignExpression* expr) = 0;
    // 泛型CastExpression的访问方法
    virtual Value* visit(CastExpression<Integer>* expr) = 0;
    virtual Value* visit(CastExpression<Double>* expr) = 0;
    virtual Value* visit(CastExpression<Bool>* expr) = 0;
    virtual Value* visit(CastExpression<Char>* expr) = 0;
    virtual Value* visit(CastExpression<String>* expr) = 0;
    virtual Value* visit(AccessExpression* expr) = 0;
    virtual Value* visit(CallExpression* expr) = 0;
    virtual Value* visit(MethodCallExpression* expr) = 0;
    
    // 语句访问方法 - 标准访问者模式，void返回类型
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
    virtual void visit(ThrowStatement* stmt) = 0;
    virtual void visit(TryStatement* stmt) = 0;
    virtual void visit(SwitchStatement* stmt) = 0;
    virtual void visit(FunctionPrototype* stmt) = 0;
    
    // function.h中定义的类的访问方法
    virtual void visit(Identifier* id) = 0;
    virtual void visit(Variable* var) = 0;
    virtual void visit(UserFunction* func) = 0;
    virtual void visit(ClassMethod* method) = 0;
    
    // 程序访问方法
    virtual void visit(Program* program) = 0;
};

#endif // AST_VISITOR_H
