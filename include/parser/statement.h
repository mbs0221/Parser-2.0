#ifndef STATEMENT_H
#define STATEMENT_H

#include "expression.h"
#include "function.h"
#include <string>
#include <vector>
#include <map>

using namespace std;

// ==================== 语句基类 ====================
// 语句基类
struct Statement : public AST {
    virtual void accept(ASTVisitor* visitor) override = 0;
};

// ==================== 基本语句 ====================
// 导入语句
struct ImportStatement : public Statement {
    String *moduleName;
    
    ImportStatement(String *name) : moduleName(name) {}
    
    void accept(ASTVisitor* visitor) override;
};

// 表达式语句
struct ExpressionStatement : public Statement {
    Expression* expression;
    
    ExpressionStatement(Expression* expr) : expression(expr) {}
    
    void accept(ASTVisitor* visitor) override;
};

// 变量声明语句
struct VariableDeclaration : public Statement {
    string variableName;
    string variableType;
    Expression* initialValue;
    
    VariableDeclaration(const string& name, const string& type, Expression* value = nullptr)
        : variableName(name), variableType(type), initialValue(value) {}
    
    void accept(ASTVisitor* visitor) override;
};

// ==================== 控制流语句 ====================
// 条件语句
struct IfStatement : public Statement {
    Expression* condition;
    Statement* thenStatement;
    Statement* elseStatement;
    
    IfStatement(Expression* cond, Statement* thenStmt, Statement* elseStmt = nullptr)
        : condition(cond), thenStatement(thenStmt), elseStatement(elseStmt) {}
    
    void accept(ASTVisitor* visitor) override;
};

// 循环语句
struct WhileStatement : public Statement {
    Expression* condition;
    Statement* body;
    
    WhileStatement(Expression* cond, Statement* stmt) : condition(cond), body(stmt) {}
    
    void accept(ASTVisitor* visitor) override;
};

// For循环语句
struct ForStatement : public Statement {
    Statement* initializer;
    Expression* condition;
    Expression* increment;
    Statement* body;
    
    ForStatement(Statement* init, Expression* cond, Expression* inc, Statement* stmt)
        : initializer(init), condition(cond), increment(inc), body(stmt) {}
    
    void accept(ASTVisitor* visitor) override;
};

// Do-While循环语句
struct DoWhileStatement : public Statement {
    Statement* body;
    Expression* condition;
    
    DoWhileStatement(Statement* stmt, Expression* cond) : body(stmt), condition(cond) {}
    
    void accept(ASTVisitor* visitor) override;
};

// 跳转语句
struct BreakStatement : public Statement {
    BreakStatement() {}
    
    void accept(ASTVisitor* visitor) override;
};

struct ContinueStatement : public Statement {
    ContinueStatement() {}
    
    void accept(ASTVisitor* visitor) override;
};

// 返回语句
struct ReturnStatement : public Statement {
    Expression* returnValue;
    
    ReturnStatement(Expression* value = nullptr) : returnValue(value) {}
    
    void accept(ASTVisitor* visitor) override;
};

// ==================== 异常处理语句 ====================
// 抛出异常语句
struct ThrowStatement : public Statement {
    Expression* expression;
    
    ThrowStatement(Expression* expr) : expression(expr) {}
    
    void accept(ASTVisitor* visitor) override;
};

// Try-Catch语句
struct CatchStatement : public Statement {
    string exceptionType;
    string exceptionName;
    Statement* catchBlock;
    
    CatchStatement(const string& type, const string& name, Statement* stmt)
        : exceptionType(type), exceptionName(name), catchBlock(stmt) {}
    
    void accept(ASTVisitor* visitor) override;
};

struct FinallyStatement : public Statement {
    Statement* finallyBlock;
    
    FinallyStatement(Statement* stmt) : finallyBlock(stmt) {}
    
    void accept(ASTVisitor* visitor) override;
};

struct TryStatement : public Statement {
    Statement* tryBlock;
    vector<CatchStatement*> catchBlocks;
    FinallyStatement* finallyBlock;
    
    TryStatement(Statement* tryStmt, const vector<CatchStatement*>& catchStmts, FinallyStatement* finallyStmt = nullptr)
        : tryBlock(tryStmt), catchBlocks(catchStmts), finallyBlock(finallyStmt) {}
    
    void accept(ASTVisitor* visitor) override;
};

// ==================== Switch语句 ====================
struct CaseStatement : public Statement {
    Expression* value;
    vector<Statement*> statements;
    
    CaseStatement(Expression* val, const vector<Statement*>& stmts) : value(val), statements(stmts) {}
    
    void accept(ASTVisitor* visitor) override;
};

struct DefaultStatement : public Statement {
    vector<Statement*> statements;
    
    DefaultStatement(const vector<Statement*>& stmts) : statements(stmts) {}
    
    void accept(ASTVisitor* visitor) override;
};

// Switch语句
struct SwitchStatement : public Statement {
    Expression* expression;
    vector<CaseStatement*> cases;
    DefaultStatement* defaultCase;
    
    SwitchStatement(Expression* expr, const vector<CaseStatement*>& caseStmts, DefaultStatement* defaultStmt = nullptr)
        : expression(expr), cases(caseStmts), defaultCase(defaultStmt) {}
    
    void accept(ASTVisitor* visitor) override;
};

// ==================== 复合语句 ====================
// 块语句
struct BlockStatement : public Statement {
    vector<Statement*> statements;
    
    BlockStatement(const vector<Statement*>& stmts) : statements(stmts) {}
    
    void accept(ASTVisitor* visitor) override;
};

// ==================== 程序根节点 ====================
// 程序根节点
struct Program : public AST {
    vector<Statement*> statements;
    
    Program(const vector<Statement*>& stmts) : statements(stmts) {}
    
    void accept(ASTVisitor* visitor) override;
};

#endif // STATEMENT_H
