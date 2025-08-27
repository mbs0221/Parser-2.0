#include "parser/inter.h"

// 添加缺失的accept方法实现
void IntExpression::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

void DoubleExpression::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

void BoolExpression::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

void CharExpression::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

void ArrayNode::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

void DictNode::accept(ASTVisitor* visitor) {
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

void AssignmentExpression::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

void AccessExpression::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

void CallExpression::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

void StructInstantiationExpression::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

void ClassInstantiationExpression::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

void MemberAccessExpression::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

void ImportStatement::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

void ExpressionStatement::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

void VariableDeclaration::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

void IfStatement::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

void WhileStatement::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

void ForStatement::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

void DoWhileStatement::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

void BlockStatement::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

void FunctionDefinition::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

void StructDefinition::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

void ClassDefinition::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

void BreakStatement::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

void ContinueStatement::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

void ReturnStatement::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

void ThrowStatement::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

void TryStatement::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

void CatchStatement::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

void FinallyStatement::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

void SwitchStatement::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

void CaseStatement::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

void DefaultStatement::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

void FunctionPrototype::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

void Program::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

// CastExpression模板实例的accept方法实现
template<>
void CastExpression<IntExpression>::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

template<>
void CastExpression<DoubleExpression>::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

template<>
void CastExpression<CharExpression>::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

template<>
void CastExpression<BoolExpression>::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

void MethodCallExpression::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}
