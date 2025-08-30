#include "parser/expression.h"
#include "parser/ast_visitor.h"

// ==================== Expression基类实现 ====================
template<typename ReturnType>
ReturnType Expression::accept(ExpressionVisitor<ReturnType>* visitor) {
    return visitor->visit(this);
}

// ==================== ConstantExpression实现 ====================
template<typename T>
template<typename ReturnType>
ReturnType ConstantExpression<T>::accept(ExpressionVisitor<ReturnType>* visitor) {
    return visitor->visit(this);
}

// 模板特化实现
template int ConstantExpression<int>::accept<int>(ExpressionVisitor<int>* visitor);
template double ConstantExpression<double>::accept<double>(ExpressionVisitor<double>* visitor);
template bool ConstantExpression<bool>::accept<bool>(ExpressionVisitor<bool>* visitor);
template char ConstantExpression<char>::accept<char>(ExpressionVisitor<char>* visitor);
template string ConstantExpression<string>::accept<string>(ExpressionVisitor<string>* visitor);

// ==================== VariableExpression实现 ====================
template<typename ReturnType>
ReturnType VariableExpression::accept(ExpressionVisitor<ReturnType>* visitor) {
    return visitor->visit(this);
}

// ==================== UnaryExpression实现 ====================
template<typename ReturnType>
ReturnType UnaryExpression::accept(ExpressionVisitor<ReturnType>* visitor) {
    return visitor->visit(this);
}

// ==================== BinaryExpression实现 ====================
template<typename ReturnType>
ReturnType BinaryExpression::accept(ExpressionVisitor<ReturnType>* visitor) {
    return visitor->visit(this);
}

// ==================== AssignExpression实现 ====================
template<typename ReturnType>
ReturnType AssignExpression::accept(ExpressionVisitor<ReturnType>* visitor) {
    return visitor->visit(this);
}

// ==================== CastExpression实现 ====================
template<typename ReturnType>
ReturnType CastExpression::accept(ExpressionVisitor<ReturnType>* visitor) {
    return visitor->visit(this);
}

// ==================== AccessExpression实现 ====================
template<typename ReturnType>
ReturnType AccessExpression::accept(ExpressionVisitor<ReturnType>* visitor) {
    return visitor->visit(this);
}

// ==================== CallExpression实现 ====================
template<typename ReturnType>
ReturnType CallExpression::accept(ExpressionVisitor<ReturnType>* visitor) {
    return visitor->visit(this);
}

// ==================== MethodCallExpression实现 ====================
template<typename ReturnType>
ReturnType MethodCallExpression::accept(ExpressionVisitor<ReturnType>* visitor) {
    return visitor->visit(this);
}




