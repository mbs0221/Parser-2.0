#ifndef ENHANCED_CONTROL_FLOW_H
#define ENHANCED_CONTROL_FLOW_H

#include "parser/statement.h"
#include "interpreter/values/value.h"
#include "interpreter/core/control_flow.h"
#include <vector>
#include <string>
#include <memory>

// 增强的控制流语句支持

// ==================== 增强的循环语句 ====================

// for-each循环语句 (for item in collection)
struct ForEachStatement : public Statement {
    std::string variableName;  // 循环变量名
    Expression* collection;    // 集合表达式
    Statement* body;          // 循环体
    
    ForEachStatement(const std::string& varName, Expression* coll, Statement* b)
        : variableName(varName), collection(coll), body(b) {}
    
    ~ForEachStatement() {
        if (collection) {
            delete collection;
            collection = nullptr;
        }
        if (body) {
            delete body;
            body = nullptr;
        }
    }
    
    std::string getLocation() const override {
        return "for-each statement: " + variableName;
    }
};

// for-range循环语句 (for i in 1..10)
struct ForRangeStatement : public Statement {
    std::string variableName;  // 循环变量名
    Expression* start;        // 起始值
    Expression* end;          // 结束值
    Expression* step;         // 步长（可选）
    Statement* body;          // 循环体
    
    ForRangeStatement(const std::string& varName, Expression* s, Expression* e, 
                     Expression* st = nullptr, Statement* b = nullptr)
        : variableName(varName), start(s), end(e), step(st), body(b) {}
    
    ~ForRangeStatement() {
        if (start) { delete start; start = nullptr; }
        if (end) { delete end; end = nullptr; }
        if (step) { delete step; step = nullptr; }
        if (body) { delete body; body = nullptr; }
    }
    
    std::string getLocation() const override {
        return "for-range statement: " + variableName;
    }
};

// ==================== 增强的条件语句 ====================

// else-if语句
struct ElseIfStatement : public Statement {
    Expression* condition;    // 条件表达式
    Statement* body;         // 语句体
    
    ElseIfStatement(Expression* cond, Statement* b) : condition(cond), body(b) {}
    
    ~ElseIfStatement() {
        if (condition) { delete condition; condition = nullptr; }
        if (body) { delete body; body = nullptr; }
    }
    
    std::string getLocation() const override {
        return "else-if statement";
    }
};

// 多重if语句 (if-elif-else)
struct MultiIfStatement : public Statement {
    Expression* condition;                    // 主条件
    Statement* thenBody;                     // then分支
    std::vector<ElseIfStatement*> elifBranches; // elif分支
    Statement* elseBody;                     // else分支
    
    MultiIfStatement(Expression* cond, Statement* then, 
                    const std::vector<ElseIfStatement*>& elifs, Statement* elseStmt = nullptr)
        : condition(cond), thenBody(then), elifBranches(elifs), elseBody(elseStmt) {}
    
    ~MultiIfStatement() {
        if (condition) { delete condition; condition = nullptr; }
        if (thenBody) { delete thenBody; thenBody = nullptr; }
        for (auto* elif : elifBranches) {
            if (elif) delete elif;
        }
        elifBranches.clear();
        if (elseBody) { delete elseBody; elseBody = nullptr; }
    }
    
    std::string getLocation() const override {
        return "multi-if statement";
    }
};

// ==================== 异常处理语句 ====================

// catch语句
struct CatchStatement : public Statement {
    std::string exceptionType;  // 异常类型
    std::string variableName;   // 异常变量名
    Statement* body;           // catch体
    
    CatchStatement(const std::string& exType, const std::string& varName, Statement* b)
        : exceptionType(exType), variableName(varName), body(b) {}
    
    ~CatchStatement() {
        if (body) {
            delete body;
            body = nullptr;
        }
    }
    
    std::string getLocation() const override {
        return "catch statement: " + exceptionType;
    }
};

// finally语句
struct FinallyStatement : public Statement {
    Statement* body;  // finally体
    
    FinallyStatement(Statement* b) : body(b) {}
    
    ~FinallyStatement() {
        if (body) {
            delete body;
            body = nullptr;
        }
    }
    
    std::string getLocation() const override {
        return "finally statement";
    }
};

// 增强的try-catch-finally语句
struct EnhancedTryStatement : public Statement {
    Statement* tryBody;                        // try体
    std::vector<CatchStatement*> catchBranches; // catch分支
    FinallyStatement* finallyBody;             // finally分支
    
    EnhancedTryStatement(Statement* tryStmt, 
                        const std::vector<CatchStatement*>& catches, 
                        FinallyStatement* finally = nullptr)
        : tryBody(tryStmt), catchBranches(catches), finallyBody(finally) {}
    
    ~EnhancedTryStatement() {
        if (tryBody) { delete tryBody; tryBody = nullptr; }
        for (auto* catchStmt : catchBranches) {
            if (catchStmt) delete catchStmt;
        }
        catchBranches.clear();
        if (finallyBody) { delete finallyBody; finallyBody = nullptr; }
    }
    
    std::string getLocation() const override {
        return "enhanced try statement";
    }
};

// ==================== 跳转语句增强 ====================

// 带标签的break语句
struct LabeledBreakStatement : public BreakStatement {
    std::string label;  // 标签名
    
    LabeledBreakStatement(const std::string& lbl) : label(lbl) {}
    
    std::string getLocation() const override {
        return "labeled break statement: " + label;
    }
};

// 带标签的continue语句
struct LabeledContinueStatement : public ContinueStatement {
    std::string label;  // 标签名
    
    LabeledContinueStatement(const std::string& lbl) : label(lbl) {}
    
    std::string getLocation() const override {
        return "labeled continue statement: " + label;
    }
};

// 标签语句
struct LabelStatement : public Statement {
    std::string label;  // 标签名
    Statement* body;    // 标签后的语句
    
    LabelStatement(const std::string& lbl, Statement* b) : label(lbl), body(b) {}
    
    ~LabelStatement() {
        if (body) {
            delete body;
            body = nullptr;
        }
    }
    
    std::string getLocation() const override {
        return "label statement: " + label;
    }
};

// ==================== 协程支持 ====================

// yield语句
struct YieldStatement : public Statement {
    Expression* expression;  // yield的值
    
    YieldStatement(Expression* expr = nullptr) : expression(expr) {}
    
    ~YieldStatement() {
        if (expression) {
            delete expression;
            expression = nullptr;
        }
    }
    
    std::string getLocation() const override {
        return "yield statement";
    }
};

// 协程函数定义
struct CoroutineDefinition : public Statement {
    std::string name;                    // 协程名
    std::vector<std::string> parameters; // 参数列表
    Statement* body;                    // 协程体
    
    CoroutineDefinition(const std::string& n, 
                       const std::vector<std::string>& params, 
                       Statement* b)
        : name(n), parameters(params), body(b) {}
    
    ~CoroutineDefinition() {
        if (body) {
            delete body;
            body = nullptr;
        }
    }
    
    std::string getLocation() const override {
        return "coroutine definition: " + name;
    }
};

// ==================== 并行控制流 ====================

// 并行语句块
struct ParallelStatement : public Statement {
    std::vector<Statement*> statements;  // 并行执行的语句
    
    ParallelStatement(const std::vector<Statement*>& stmts) : statements(stmts) {}
    
    ~ParallelStatement() {
        for (Statement* stmt : statements) {
            if (stmt) delete stmt;
        }
        statements.clear();
    }
    
    std::string getLocation() const override {
        return "parallel statement with " + std::to_string(statements.size()) + " branches";
    }
};

// 同步语句
struct SynchronizeStatement : public Statement {
    std::string lockName;  // 锁名称
    Statement* body;       // 同步体
    
    SynchronizeStatement(const std::string& lock, Statement* b) 
        : lockName(lock), body(b) {}
    
    ~SynchronizeStatement() {
        if (body) {
            delete body;
            body = nullptr;
        }
    }
    
    std::string getLocation() const override {
        return "synchronize statement: " + lockName;
    }
};

// ==================== 条件编译 ====================

// 条件编译语句
struct ConditionalCompilationStatement : public Statement {
    std::string condition;  // 编译条件
    Statement* trueBody;    // 条件为真时的语句
    Statement* falseBody;   // 条件为假时的语句
    
    ConditionalCompilationStatement(const std::string& cond, 
                                   Statement* trueStmt, 
                                   Statement* falseStmt = nullptr)
        : condition(cond), trueBody(trueStmt), falseBody(falseStmt) {}
    
    ~ConditionalCompilationStatement() {
        if (trueBody) { delete trueBody; trueBody = nullptr; }
        if (falseBody) { delete falseBody; falseBody = nullptr; }
    }
    
    std::string getLocation() const override {
        return "conditional compilation statement: " + condition;
    }
};

#endif // ENHANCED_CONTROL_FLOW_H
