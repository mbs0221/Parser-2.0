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

// ConstantExpression模板特化实现
template<>
string ConstantExpression<int>::getLocation() const {
    return to_string(value);
}

template<>
string ConstantExpression<double>::getLocation() const {
    return to_string(value);
}

template<>
string ConstantExpression<bool>::getLocation() const {
    return value ? "true" : "false";
}

template<>
string ConstantExpression<char>::getLocation() const {
    return string(1, value);
}

template<>
string ConstantExpression<string>::getLocation() const {
    return "\"" + value + "\"";
}

// 类型名称特化
template<>
string ConstantExpression<int>::getTypeName() const {
    return "int";
}

template<>
string ConstantExpression<double>::getTypeName() const {
    return "double";
}

template<>
string ConstantExpression<bool>::getTypeName() const {
    return "bool";
}

template<>
string ConstantExpression<char>::getTypeName() const {
    return "char";
}

template<>
string ConstantExpression<string>::getTypeName() const {
    return "string";
}

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

// ==================== IncrementDecrementExpression实现 ====================
template<typename ReturnType>
ReturnType IncrementDecrementExpression::accept(ExpressionVisitor<ReturnType>* visitor) {
    return visitor->visit(this);
}
