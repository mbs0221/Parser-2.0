#ifndef AST_OPTIMIZER_H
#define AST_OPTIMIZER_H

#include "parser/inter.h"
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

// 类型转换优化器
class TypeCastOptimizer : public ASTOptimizer {
public:
    TypeCastOptimizer() = default;
    ~TypeCastOptimizer() = default;
    
    // 优化表达式，插入必要的类型转换
    Expression* optimize(Expression* expr) override;
    Statement* optimize(Statement* stmt) override;
    Program* optimize(Program* program) override;
    
    // 访问者模式实现
    void visit(Program* program) override;
    void visit(VariableDeclaration* decl) override;
    void visit(FunctionDefinition* func) override;
    void visit(ClassDefinition* cls) override;
    void visit(IfStatement* stmt) override;
    void visit(WhileStatement* stmt) override;
    void visit(ForStatement* stmt) override;
    void visit(DoWhileStatement* stmt) override;
    void visit(ReturnStatement* stmt) override;
    void visit(BreakStatement* stmt) override;
    void visit(ContinueStatement* stmt) override;
    void visit(ExpressionStatement* stmt) override;
    void visit(BlockStatement* stmt) override;
    void visit(ImportStatement* stmt) override;
    void visit(VariableExpression* expr) override;

    // StringLiteral的visit方法已移除，使用value.h中的String
    void visit(CharExpression* expr) override;
    void visit(BoolExpression* expr) override;
    // ArrayNode、DictNode的visit方法已移除，使用value.h中的Array、Dict
    void visit(AssignmentExpression* expr) override;
    void visit(BinaryExpression* expr) override;
    void visit(UnaryExpression* expr) override;
    void visit(CallExpression* expr) override;
    void visit(AccessExpression* expr) override;
    void visit(IntExpression* expr) override;
    void visit(DoubleExpression* expr) override;
    void visit(MethodCallExpression* expr) override;
    void visit(CastExpression<IntExpression>* expr) override;
    void visit(CastExpression<DoubleExpression>* expr) override;
    // CastExpression<StringLiteral>的visit方法已移除，使用value.h中的String
    void visit(CastExpression<CharExpression>* expr) override;
    void visit(CastExpression<BoolExpression>* expr) override;
    // StructInstantiationExpression和ClassInstantiationExpression的visit方法已移除，使用CallExpression
    void visit(MemberAccessExpression* expr) override;
    void visit(StructDefinition* stmt) override;

    void visit(SwitchStatement* stmt) override;
    // CaseStatement和DefaultStatement的visit方法已移除，合并到SwitchStatement中
    void visit(FunctionPrototype* stmt) override;

private:
    // 辅助方法
    bool shouldConvertLeftToRight(Expression* left, Expression* right, const std::string& operator_);
    int getTypePriority(Expression* expr);
    Expression* createCastExpressionForType(Expression* expr, Expression* targetTypeExpr);
    Expression* insertTypeCasts(Expression* expr);
};

#endif // AST_OPTIMIZER_H
