#include "parser/statement.h"
#include "parser/ast_visitor.h"

// ==================== 基本语句类型 ====================
void ImportStatement::accept(StatementVisitor* visitor) {
    visitor->visit(this);
}

void ExpressionStatement::accept(StatementVisitor* visitor) {
    visitor->visit(this);
}



// ==================== 控制流语句类型 ====================
void IfStatement::accept(StatementVisitor* visitor) {
    visitor->visit(this);
}

void WhileStatement::accept(StatementVisitor* visitor) {
    visitor->visit(this);
}

void ForStatement::accept(StatementVisitor* visitor) {
    visitor->visit(this);
}

void DoWhileStatement::accept(StatementVisitor* visitor) {
    visitor->visit(this);
}

void BreakStatement::accept(StatementVisitor* visitor) {
    visitor->visit(this);
}

void ContinueStatement::accept(StatementVisitor* visitor) {
    visitor->visit(this);
}

void ReturnStatement::accept(StatementVisitor* visitor) {
    visitor->visit(this);
}

// ==================== 异常处理语句类型 ====================
void TryStatement::accept(StatementVisitor* visitor) {
    visitor->visit(this);
}

// ==================== Switch语句类型 ====================
void SwitchStatement::accept(StatementVisitor* visitor) {
    visitor->visit(this);
}

// ==================== 复合语句和函数相关语句类型 ====================
void BlockStatement::accept(StatementVisitor* visitor) {
    visitor->visit(this);
}

// ==================== 程序根节点 ====================
void Program::accept(StatementVisitor* visitor) {
    visitor->visit(this);
}
