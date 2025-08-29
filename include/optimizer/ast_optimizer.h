#ifndef AST_OPTIMIZER_H
#define AST_OPTIMIZER_H

#include "parser/ast_visitor.h"
#include <string>

// AST优化器基类
class ASTOptimizer : public ASTVisitor {
public:
    virtual ~ASTOptimizer() = default;
    
    // 优化整个AST
    virtual Expression* optimize(Expression* expr) = 0;
    virtual Statement* optimize(Statement* stmt) = 0;
    virtual Program* optimize(Program* program) = 0;
};

// 简化的类型转换优化器
class SimpleOptimizer : public ASTOptimizer {
public:
    SimpleOptimizer() = default;
    ~SimpleOptimizer() = default;
    
    // 优化表达式，插入必要的类型转换
    Expression* optimize(Expression* expr) override;
    Statement* optimize(Statement* stmt) override;
    Program* optimize(Program* program) override;
    
    // 基本的访问者模式实现 - 只实现必要的
    Value* visit(Expression* expr) override;
    Value* visit(ConstantExpression* expr) override;
    Value* visit(VariableExpression* expr) override;
    Value* visit(UnaryExpression* expr) override;
    Value* visit(BinaryExpression* expr) override;
    Value* visit(AssignExpression* expr) override;
    Value* visit(AccessExpression* expr) override;
    Value* visit(CallExpression* expr) override;
    Value* visit(MethodCallExpression* expr) override;
    Value* visit(CastExpression<Integer>* expr) override;
    Value* visit(CastExpression<Double>* expr) override;
    Value* visit(CastExpression<Bool>* expr) override;
    Value* visit(CastExpression<Char>* expr) override;
    Value* visit(CastExpression<String>* expr) override;

    // 语句访问方法
    void visit(Program* program) override;
    void visit(Statement* stmt) override;
    void visit(ImportStatement* stmt) override;
    void visit(ExpressionStatement* stmt) override;
    void visit(VariableDeclaration* stmt) override;
    void visit(IfStatement* stmt) override;
    void visit(WhileStatement* stmt) override;
    void visit(ForStatement* stmt) override;
    void visit(DoWhileStatement* stmt) override;
    void visit(BlockStatement* stmt) override;
    void visit(StructDefinition* stmt) override;
    void visit(ClassDefinition* stmt) override;
    void visit(BreakStatement* stmt) override;
    void visit(ContinueStatement* stmt) override;
    void visit(ReturnStatement* stmt) override;
    void visit(TryStatement* stmt) override;
    void visit(SwitchStatement* stmt) override;
    void visit(FunctionPrototype* stmt) override;
    void visit(Identifier* id) override;
    void visit(Variable* var) override;
    void visit(UserFunction* func) override;

private:
    // 辅助方法
    Expression* insertTypeCasts(Expression* expr);
};

#endif // AST_OPTIMIZER_H
