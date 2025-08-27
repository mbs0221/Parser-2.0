#include "parser/expression.h"
#include "parser/ast_visitor.h"

// ==================== 基本表达式类型 ====================
void ConstantExpression::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

void VariableExpression::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

void UnaryExpression::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

void BinaryExpression::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

void CastExpression::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

// ==================== 访问和调用表达式类型 ====================
void AccessExpression::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

void CallExpression::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

void BuiltinFunctionExpression::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

// ==================== 结构体和类相关表达式类型 ====================
void StructInstantiationExpression::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

void ClassInstantiationExpression::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

void MemberAccessExpression::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

void MethodCallExpression::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}
