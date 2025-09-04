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

// ==================== 导入类型定义 ====================
// 导入类型枚举
enum class ImportType {
    DEFAULT,        // import "module";
    NAMESPACE,      // import "module" as ns;
    SELECTIVE,      // import { func1, func2 } from "module";
    WILDCARD        // import * from "module";
};

// 导入项信息
struct ImportItem {
    std::string name;           // 导入的名称
    std::string alias;          // 别名（可选）
    
    ImportItem(const std::string& n, const std::string& a = "") 
        : name(n), alias(a.empty() ? n : a) {}
};

// ==================== 基本语句 ====================
// 导入语句 - 增强版本
struct ImportStatement : public Statement {
    std::string modulePath;     // 模块路径
    ImportType type;            // 导入类型
    std::string namespaceAlias; // 命名空间别名
    std::vector<ImportItem> items; // 导入项列表（用于选择性导入）
    
    // 构造函数
    ImportStatement(const std::string& path, ImportType t = ImportType::DEFAULT)
        : modulePath(path), type(t) {}
    
    ImportStatement(const std::string& path, const std::string& alias)
        : modulePath(path), type(ImportType::NAMESPACE), namespaceAlias(alias) {}
    
    ImportStatement(const std::string& path, const std::vector<ImportItem>& importItems)
        : modulePath(path), type(ImportType::SELECTIVE), items(importItems) {}
    
    // 添加导入项
    void addImportItem(const std::string& name, const std::string& alias = "") {
        items.emplace_back(name, alias);
    }
    
    // 设置通配符导入
    void setWildcardImport() {
        type = ImportType::WILDCARD;
        items.clear();
    }
    
    ~ImportStatement() {
        // 不需要手动清理，容器会自动管理内存
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
        if (condition) delete condition;
        if (thenStatement) delete thenStatement;
        if (elseStatement) delete elseStatement;
    }
    
    void accept(StatementVisitor* visitor) override;
};

// 循环语句
struct WhileStatement : public Statement {
    Expression* condition;
    Statement* body;
    
    WhileStatement(Expression* cond, Statement* bodyStmt)
        : condition(cond), body(bodyStmt) {}
    
    ~WhileStatement() {
        if (condition) delete condition;
        if (body) delete body;
    }
    
    void accept(StatementVisitor* visitor) override;
};

struct ForStatement : public Statement {
    Statement* initialization;
    Expression* condition;
    Statement* increment;
    Statement* body;
    
    ForStatement(Statement* init, Expression* cond, Statement* inc, Statement* bodyStmt)
        : initialization(init), condition(cond), increment(inc), body(bodyStmt) {}
    
    ~ForStatement() {
        if (initialization) delete initialization;
        if (condition) delete condition;
        if (increment) delete increment;
        if (body) delete body;
    }
    
    void accept(StatementVisitor* visitor) override;
};

struct DoWhileStatement : public Statement {
    Statement* body;
    Expression* condition;
    
    DoWhileStatement(Statement* bodyStmt, Expression* cond)
        : body(bodyStmt), condition(cond) {}
    
    ~DoWhileStatement() {
        if (body) delete body;
        if (condition) delete condition;
    }
    
    void accept(StatementVisitor* visitor) override;
};

// 跳转语句
struct BreakStatement : public Statement {
    void accept(StatementVisitor* visitor) override;
};

struct ContinueStatement : public Statement {
    void accept(StatementVisitor* visitor) override;
};

struct ReturnStatement : public Statement {
    Expression* returnValue;
    
    ReturnStatement(Expression* value = nullptr) : returnValue(value) {}
    
    ~ReturnStatement() {
        if (returnValue) delete returnValue;
    }
    
    void accept(StatementVisitor* visitor) override;
};

// ==================== 异常处理语句 ====================
struct TryStatement : public Statement {
    Statement* tryBlock;
    std::string exceptionVariable;
    Statement* catchBlock;
    Statement* finallyBlock;
    
    TryStatement(Statement* tryBlk, const std::string& exVar, Statement* catchBlk, Statement* finallyBlk = nullptr)
        : tryBlock(tryBlk), exceptionVariable(exVar), catchBlock(catchBlk), finallyBlock(finallyBlk) {}
    
    ~TryStatement() {
        if (tryBlock) delete tryBlock;
        if (catchBlock) delete catchBlock;
        if (finallyBlock) delete finallyBlock;
    }
    
    void accept(StatementVisitor* visitor) override;
};

// ==================== Switch语句 ====================
struct SwitchStatement : public Statement {
    Expression* expression;
    std::vector<std::pair<Expression*, Statement*>> cases;
    Statement* defaultCase;
    
    SwitchStatement(Expression* expr) : expression(expr), defaultCase(nullptr) {}
    
    ~SwitchStatement() {
        if (expression) delete expression;
        for (auto& case_pair : cases) {
            if (case_pair.first) delete case_pair.first;
            if (case_pair.second) delete case_pair.second;
        }
        if (defaultCase) delete defaultCase;
    }
    
    void addCase(Expression* caseExpr, Statement* caseStmt) {
        cases.emplace_back(caseExpr, caseStmt);
    }
    
    void setDefaultCase(Statement* defaultStmt) {
        defaultCase = defaultStmt;
    }
    
    void accept(StatementVisitor* visitor) override;
};

// ==================== 复合语句 ====================
struct BlockStatement : public Statement {
    std::vector<Statement*> statements;
    
    BlockStatement() = default;
    
    ~BlockStatement() {
        for (Statement* stmt : statements) {
            if (stmt) delete stmt;
        }
    }
    
    void addStatement(Statement* stmt) {
        if (stmt) statements.push_back(stmt);
    }
    
    void accept(StatementVisitor* visitor) override;
};

// ==================== 程序根节点 ====================
struct Program : public Statement {
    std::vector<Statement*> statements;
    
    Program() = default;
    
    ~Program() {
        for (Statement* stmt : statements) {
            if (stmt) delete stmt;
        }
    }
    
    void addStatement(Statement* stmt) {
        if (stmt) statements.push_back(stmt);
    }
    
    void accept(StatementVisitor* visitor) override;
};

#endif // STATEMENT_H