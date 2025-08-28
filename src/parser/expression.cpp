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

// ==================== AssignExpression实现 ====================
void AssignExpression::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

// ==================== CastExpression实现 ====================
template<typename T>
void CastExpression<T>::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

// 显式实例化
template void CastExpression<Integer>::accept(ASTVisitor* visitor);
template void CastExpression<Double>::accept(ASTVisitor* visitor);
template void CastExpression<Bool>::accept(ASTVisitor* visitor);
template void CastExpression<Char>::accept(ASTVisitor* visitor);
template void CastExpression<String>::accept(ASTVisitor* visitor);

// ==================== AccessExpression实现 ====================
void AccessExpression::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

// ==================== CallExpression实现 ====================
void CallExpression::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

// ==================== MemberAccessExpression实现 ====================
// MemberAccessExpression已合并到AccessExpression中

// ==================== MethodCallExpression实现 ====================
void MethodCallExpression::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}
