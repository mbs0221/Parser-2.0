#ifndef AST_VISITOR_H
#define AST_VISITOR_H

#include <string>

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
class AssignmentExpression;
class StringLiteral;
class ArrayNode;
class DictNode;
class AccessExpression;
class CallExpression;
class MethodCallExpression;

// 类型转换表达式前向声明
class CastExpression;

class StructInstantiationExpression;
class ClassInstantiationExpression;
class MemberAccessExpression;

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
class CatchStatement;
class FinallyStatement;
class SwitchStatement;
class CaseStatement;
class DefaultStatement;
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
    
    // 表达式访问方法 - 返回Value类型
    virtual Value* visit(ConstantExpression* expr) = 0;
    virtual Value* visit(VariableExpression* expr) = 0;
    virtual Value* visit(UnaryExpression* expr) = 0;
    virtual Value* visit(BinaryExpression* expr) = 0;
    virtual Value* visit(CastExpression* expr) = 0;
    virtual Value* visit(StringLiteral* expr) = 0;
    virtual Value* visit(ArrayNode* expr) = 0;
    virtual Value* visit(DictNode* expr) = 0;
    virtual Value* visit(AccessExpression* expr) = 0;
    virtual Value* visit(CallExpression* expr) = 0;
    virtual Value* visit(MethodCallExpression* expr) = 0;


    virtual Value* visit(StructInstantiationExpression* expr) = 0;
    virtual Value* visit(ClassInstantiationExpression* expr) = 0;
    virtual Value* visit(MemberAccessExpression* expr) = 0;
    
    // 语句访问方法 - 标准访问者模式，void返回类型
    virtual void visit(ImportStatement* stmt) = 0;
    virtual void visit(ExpressionStatement* stmt) = 0;
    virtual void visit(VariableDeclaration* stmt) = 0;
    virtual void visit(IfStatement* stmt) = 0;
    virtual void visit(WhileStatement* stmt) = 0;
    virtual void visit(ForStatement* stmt) = 0;
    virtual void visit(DoWhileStatement* stmt) = 0;
    virtual void visit(BlockStatement* stmt) = 0;
    virtual void visit(FunctionDefinition* stmt) = 0;
    virtual void visit(StructDefinition* stmt) = 0;
    virtual void visit(ClassDefinition* stmt) = 0;
    virtual void visit(BreakStatement* stmt) = 0;
    virtual void visit(ContinueStatement* stmt) = 0;
    virtual void visit(ReturnStatement* stmt) = 0;
    virtual void visit(ThrowStatement* stmt) = 0;
    virtual void visit(TryStatement* stmt) = 0;
    virtual void visit(CatchStatement* stmt) = 0;
    virtual void visit(FinallyStatement* stmt) = 0;
    virtual void visit(SwitchStatement* stmt) = 0;
    virtual void visit(CaseStatement* stmt) = 0;
    virtual void visit(DefaultStatement* stmt) = 0;
    virtual void visit(FunctionPrototype* stmt) = 0;
    
    // function.h中定义的类的访问方法
    virtual void visit(Identifier* id) = 0;
    virtual void visit(Variable* var) = 0;
    virtual void visit(BuiltinFunction* func) = 0;
    virtual void visit(UserFunction* func) = 0;
    virtual void visit(ClassMethod* method) = 0;
    
    // 程序访问方法
    virtual void visit(Program* program) = 0;

    virtual void visit(AST* node) = 0;
};

#endif // AST_VISITOR_H
