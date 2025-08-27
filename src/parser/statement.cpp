#include "parser/statement.h"
#include "parser/ast_visitor.h"

// ==================== 基本语句类型 ====================
void ImportStatement::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

void ExpressionStatement::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

void VariableDeclaration::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

// ==================== 控制流语句类型 ====================
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

void BreakStatement::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

void ContinueStatement::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

void ReturnStatement::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

// ==================== 异常处理语句类型 ====================
void ThrowStatement::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

void TryStatement::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

// ==================== Switch语句类型 ====================
void SwitchStatement::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

// ==================== 复合语句和函数相关语句类型 ====================
void BlockStatement::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}



// ==================== 程序根节点 ====================
void Program::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}
