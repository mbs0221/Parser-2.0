#ifndef AST_VISITOR_H
#define AST_VISITOR_H

#include <string>

// 前向声明
class Expression;
class Statement;
class Program;

// 表达式类型
class IntExpression;
class DoubleExpression;
class BoolExpression;
class IdentifierExpression;
class UnaryExpression;
class BinaryExpression;
class AssignmentExpression;
class StringLiteral;
class CharExpression;
class ArrayNode;
class DictNode;
class AccessExpression;
class CallExpression;
class MethodCallExpression;

// 类型转换表达式模板前向声明
template<typename T> class CastExpression;

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

// 通用AST访问者接口 - 标准访问者模式，使用void返回类型
class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    
    // 具体的访问方法 - 标准访问者模式，void返回类型
    virtual void visit(IntExpression* expr) = 0;
    virtual void visit(DoubleExpression* expr) = 0;
    virtual void visit(BoolExpression* expr) = 0;
    virtual void visit(IdentifierExpression* expr) = 0;
    virtual void visit(UnaryExpression* expr) = 0;
    virtual void visit(BinaryExpression* expr) = 0;
    virtual void visit(AssignmentExpression* expr) = 0;
    virtual void visit(StringLiteral* expr) = 0;
    virtual void visit(CharExpression* expr) = 0;
    virtual void visit(ArrayNode* expr) = 0;
    virtual void visit(DictNode* expr) = 0;
    virtual void visit(AccessExpression* expr) = 0;
    virtual void visit(CallExpression* expr) = 0;
    virtual void visit(MethodCallExpression* expr) = 0;
    virtual void visit(CastExpression<IntExpression>* expr) = 0;
    virtual void visit(CastExpression<DoubleExpression>* expr) = 0;
    // virtual void visit(CastExpression<StringLiteral>* expr) = 0;
    virtual void visit(CastExpression<CharExpression>* expr) = 0;
    virtual void visit(CastExpression<BoolExpression>* expr) = 0;


    virtual void visit(StructInstantiationExpression* expr) = 0;
    virtual void visit(ClassInstantiationExpression* expr) = 0;
    virtual void visit(MemberAccessExpression* expr) = 0;
    
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
    
    // 程序访问方法
    virtual void visit(Program* program) = 0;
};

#endif // AST_VISITOR_H
