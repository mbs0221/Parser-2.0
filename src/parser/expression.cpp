#include "parser/expression.h"
#include "parser/ast_visitor.h"

// ==================== ConstantExpression实现 ====================
void ConstantExpression::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

// ==================== VariableExpression实现 ====================
void VariableExpression::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

// ==================== UnaryExpression实现 ====================
void UnaryExpression::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

// ==================== BinaryExpression实现 ====================
void BinaryExpression::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

// ==================== CastExpression实现 ====================
void CastExpression::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

// ==================== AccessExpression实现 ====================
void AccessExpression::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

// ==================== CallExpression实现 ====================
void CallExpression::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

// ==================== BuiltinFunctionExpression实现 ====================
void BuiltinFunctionExpression::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

// ==================== StructInstantiationExpression实现 ====================
void StructInstantiationExpression::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

// ==================== ClassInstantiationExpression实现 ====================
void ClassInstantiationExpression::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

// ==================== MemberAccessExpression实现 ====================
void MemberAccessExpression::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

// ==================== MethodCallExpression实现 ====================
void MethodCallExpression::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}
