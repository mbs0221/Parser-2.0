#ifndef STATEMENT_H
#define STATEMENT_H

#include "parser/inter.h"
#include <vector>
#include <utility>
#include <string>
#include <vector>
#include <map>

using namespace std;

// 前向声明
class Expression;

// ==================== 语句基类 ====================
// 语句基类 - 继承自AST
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
    vector<pair<string, Expression*>> variables;  // 支持多个变量声明
    
    VariableDeclaration() {}
    VariableDeclaration(const string& name, const string& type, Expression* value = nullptr) {
        addVariable(name, value);
    }
    
    void addVariable(const string& name, Expression* value) {
        variables.push_back(make_pair(name, value));
    }
    
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
// Try-Catch-Finally语句 - 合并了CatchStatement和FinallyStatement
struct TryStatement : public Statement {
    Statement* tryBlock;
    
    // 统一的catch结构
    struct CatchBlock {
        string exceptionType;
        string exceptionName;
        Statement* catchBlock;
        
        CatchBlock(const string& type, const string& name, Statement* stmt)
            : exceptionType(type), exceptionName(name), catchBlock(stmt) {}
    };
    
    vector<CatchBlock> catchBlocks;
    Statement* finallyBlock;  // nullptr表示没有finally块
    
    TryStatement(Statement* tryStmt, const vector<CatchBlock>& catchStmts, Statement* finallyStmt = nullptr)
        : tryBlock(tryStmt), catchBlocks(catchStmts), finallyBlock(finallyStmt) {}
    
    void accept(ASTVisitor* visitor) override;
};

// ==================== Switch语句 ====================
// Switch语句 - 合并了CaseStatement和DefaultStatement
struct SwitchStatement : public Statement {
    Expression* expression;
    
    // 统一的case结构，value为nullptr表示default分支
    struct SwitchCase {
        Expression* value;  // nullptr表示default分支
        vector<Statement*> statements;
        
        SwitchCase(Expression* val, const vector<Statement*>& stmts) 
            : value(val), statements(stmts) {}
    };
    
    vector<SwitchCase> cases;
    
    SwitchStatement(Expression* expr, const vector<SwitchCase>& caseStmts)
        : expression(expr), cases(caseStmts) {}
    
    void accept(ASTVisitor* visitor) override;
};

// ==================== 复合语句 ====================
// 块语句
struct BlockStatement : public Statement {
    vector<Statement*> statements;
    
    BlockStatement() : statements() {}
    BlockStatement(const vector<Statement*>& stmts) : statements(stmts) {}
    
    void addStatement(Statement* stmt) {
        statements.push_back(stmt);
    }
    
    void accept(ASTVisitor* visitor) override;
};

// ==================== 程序根节点 ====================
// 程序根节点
struct Program : public AST {
    vector<Statement*> statements;
    
    Program() : statements() {}
    Program(const vector<Statement*>& stmts) : statements(stmts) {}
    
    void addStatement(Statement* stmt) {
        statements.push_back(stmt);
    }
    
    void accept(ASTVisitor* visitor) override;
};

#endif // STATEMENT_H
