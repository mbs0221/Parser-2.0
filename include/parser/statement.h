#ifndef STATEMENT_H
#define STATEMENT_H

#include "parser/expression.h"
#include "parser/inter.h"
#include "parser/ast_visitor.h"
#include <vector>
#include <utility>
#include <string>
#include <vector>
#include <map>

// using namespace std; // 已移除，使用显式std前缀

// 前向声明
class Expression;

// ==================== 语句基类 ====================
// 语句节点基类
struct Statement : public AST {
    virtual ~Statement() = default;
    
    // 接受语句访问者 - 不需要返回值
    virtual void accept(StatementVisitor* visitor) = 0;
};



// ==================== 基本语句 ====================
// 导入语句
struct ImportStatement : public Statement {
    std::string moduleName;
    
    ImportStatement(const std::string& name) : moduleName(name) {}
    
    ~ImportStatement() {
        // 不需要手动清理，string会自动管理内存
    }
    
    void accept(StatementVisitor* visitor) override;
};

// 表达式语句
struct ExpressionStatement : public Statement {
    Expression* expression;
    
    ExpressionStatement(Expression* expr) : expression(expr) {}
    
    ~ExpressionStatement() {
        if (expression) {
            delete expression;
            expression = nullptr;
        }
    }
    
    void accept(StatementVisitor* visitor) override;
};



// ==================== 控制流语句 ====================
// 条件语句
struct IfStatement : public Statement {
    Expression* condition;
    Statement* thenStatement;
    Statement* elseStatement;
    
    IfStatement(Expression* cond, Statement* thenStmt, Statement* elseStmt = nullptr)
        : condition(cond), thenStatement(thenStmt), elseStatement(elseStmt) {}
    
    ~IfStatement() {
        if (condition) {
            delete condition;
            condition = nullptr;
        }
        if (thenStatement) {
            delete thenStatement;
            thenStatement = nullptr;
        }
        if (elseStatement) {
            delete elseStatement;
            elseStatement = nullptr;
        }
    }
    
    void accept(StatementVisitor* visitor) override;
};

// 循环语句
struct WhileStatement : public Statement {
    Expression* condition;
    Statement* body;
    
    WhileStatement(Expression* cond, Statement* stmt) : condition(cond), body(stmt) {}
    
    ~WhileStatement() {
        if (condition) {
            delete condition;
            condition = nullptr;
        }
        if (body) {
            delete body;
            body = nullptr;
        }
    }
    
    void accept(StatementVisitor* visitor) override;
};

// For循环语句
struct ForStatement : public Statement {
    Statement* initializer;
    Expression* condition;
    Expression* increment;
    Statement* body;
    
    ForStatement(Statement* init, Expression* cond, Expression* inc, Statement* stmt)
        : initializer(init), condition(cond), increment(inc), body(stmt) {}
    
    ~ForStatement() {
        if (initializer) {
            delete initializer;
            initializer = nullptr;
        }
        if (condition) {
            delete condition;
            condition = nullptr;
        }
        if (increment) {
            delete increment;
            increment = nullptr;
        }
        if (body) {
            delete body;
            body = nullptr;
        }
    }
    
    void accept(StatementVisitor* visitor) override;
};

// Do-While循环语句
struct DoWhileStatement : public Statement {
    Statement* body;
    Expression* condition;
    
    DoWhileStatement(Statement* stmt, Expression* cond) : body(stmt), condition(cond) {}
    
    ~DoWhileStatement() {
        if (body) {
            delete body;
            body = nullptr;
        }
        if (condition) {
            delete condition;
            condition = nullptr;
        }
    }
    
    void accept(StatementVisitor* visitor) override;
};

// 跳转语句
struct BreakStatement : public Statement {
    BreakStatement() {}
    
    void accept(StatementVisitor* visitor) override;
};

struct ContinueStatement : public Statement {
    ContinueStatement() {}
    
    void accept(StatementVisitor* visitor) override;
};

// 返回语句
struct ReturnStatement : public Statement {
    Expression* returnValue;
    
    ReturnStatement(Expression* value = nullptr) : returnValue(value) {}
    
    ~ReturnStatement() {
        if (returnValue) {
            delete returnValue;
            returnValue = nullptr;
        }
    }
    
    void accept(StatementVisitor* visitor) override;
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
    
    void accept(StatementVisitor* visitor) override;
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
    
    void accept(StatementVisitor* visitor) override;
};

// ==================== 复合语句 ====================
// 块语句
struct BlockStatement : public Statement {
    vector<Statement*> statements;
    
    BlockStatement() : statements() {}
    BlockStatement(const vector<Statement*>& stmts) : statements(stmts) {}
    
    ~BlockStatement() {
        for (Statement* stmt : statements) {
            if (stmt) {
                delete stmt;
            }
        }
        statements.clear();
    }
    
    void addStatement(Statement* stmt) {
        statements.push_back(stmt);
    }
    
    void accept(StatementVisitor* visitor) override;
};

// ==================== 程序根节点 ====================
// 程序根节点 - 继承自Statement
struct Program : public Statement {
    vector<Statement*> statements;
    
    Program() : statements() {}
    Program(const vector<Statement*>& stmts) : statements(stmts) {}
    
    ~Program() {
        for (Statement* stmt : statements) {
            if (stmt) {
                delete stmt;
            }
        }
        statements.clear();
    }
    
    void addStatement(Statement* stmt) {
        statements.push_back(stmt);
    }
    
    void accept(StatementVisitor* visitor) override;
};

#endif // STATEMENT_H
