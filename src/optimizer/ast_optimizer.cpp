#include "optimizer/ast_optimizer.h"
#include "parser/expression.h"
#include "parser/statement.h"
#include "parser/definition.h"
#include "lexer/token.h"
#include <iostream>

using namespace std;

// SimpleOptimizer实现

Expression* SimpleOptimizer::optimize(Expression* expr) {
    if (!expr) return nullptr;
    
    // 递归优化表达式
    return insertTypeCasts(expr);
}

Statement* SimpleOptimizer::optimize(Statement* stmt) {
    if (!stmt) return nullptr;
    
    // 让语句自己决定如何被优化
    stmt->accept(this);
    return stmt;
}

Program* SimpleOptimizer::optimize(Program* program) {
    if (!program) return nullptr;
    
    // 优化程序中的所有语句
    for (auto& stmt : program->statements) {
        if (stmt) {
            optimize(stmt);
        }
    }
    return program;
}

// 表达式访问方法实现
Value* SimpleOptimizer::visit(Expression* expr) {
    if (!expr) return nullptr;
    // 基础表达式不需要优化
    return nullptr;
}

Value* SimpleOptimizer::visit(ConstantExpression* expr) {
    if (!expr) return nullptr;
    // 常量表达式不需要优化
    return nullptr;
}

Value* SimpleOptimizer::visit(VariableExpression* expr) {
    if (!expr) return nullptr;
    // 变量表达式不需要优化
    return nullptr;
}

Value* SimpleOptimizer::visit(UnaryExpression* expr) {
    if (!expr) return nullptr;
    
    if (expr->operand) {
        expr->operand = optimize(expr->operand);
    }
    return nullptr;
}

Value* SimpleOptimizer::visit(BinaryExpression* expr) {
    if (!expr) return nullptr;
    
    // 递归优化左右操作数
    if (expr->left) {
        expr->left = optimize(expr->left);
    }
    if (expr->right) {
        expr->right = optimize(expr->right);
    }
    
    return nullptr;
}

Value* SimpleOptimizer::visit(AssignExpression* expr) {
    if (!expr) return nullptr;
    
    // AssignExpression继承自BinaryExpression，使用right作为赋值值
    if (expr->right) {
        expr->right = optimize(expr->right);
    }
    return nullptr;
}

Value* SimpleOptimizer::visit(AccessExpression* expr) {
    if (!expr) return nullptr;
    
    if (expr->target) {
        expr->target = optimize(expr->target);
    }
    if (expr->key) {
        expr->key = optimize(expr->key);
    }
    return nullptr;
}

Value* SimpleOptimizer::visit(CallExpression* expr) {
    if (!expr) return nullptr;
    
    for (auto& arg : expr->arguments) {
        if (arg) {
            arg = optimize(arg);
        }
    }
    return nullptr;
}



Value* SimpleOptimizer::visit(CastExpression* expr) {
    if (!expr) return nullptr;
    
    if (expr->operand) {
        expr->operand = optimize(expr->operand);
    }
    return nullptr;
}

// 语句访问方法实现
void SimpleOptimizer::visit(Program* program) {
    if (!program) return;
    
    for (auto& stmt : program->statements) {
        if (stmt) {
            stmt->accept(this);
        }
    }
}

void SimpleOptimizer::visit(Statement* stmt) {
    if (!stmt) return;
    // 基础语句不需要优化
}

void SimpleOptimizer::visit(ImportStatement* stmt) {
    if (!stmt) return;
    // 导入语句不需要优化
}

void SimpleOptimizer::visit(ExpressionStatement* stmt) {
    if (!stmt) return;
    
    if (stmt->expression) {
        stmt->expression = optimize(stmt->expression);
    }
}

void SimpleOptimizer::visit(VariableDefinition* stmt) {
    if (!stmt) return;
    
    // 变量声明暂时不优化
}

void SimpleOptimizer::visit(IfStatement* stmt) {
    if (!stmt) return;
    
    if (stmt->condition) {
        stmt->condition = optimize(stmt->condition);
    }
    if (stmt->thenStatement) {
        stmt->thenStatement->accept(this);
    }
    if (stmt->elseStatement) {
        stmt->elseStatement->accept(this);
    }
}

void SimpleOptimizer::visit(WhileStatement* stmt) {
    if (!stmt) return;
    
    if (stmt->condition) {
        stmt->condition = optimize(stmt->condition);
    }
    if (stmt->body) {
        stmt->body->accept(this);
    }
}

void SimpleOptimizer::visit(ForStatement* stmt) {
    if (!stmt) return;
    
    if (stmt->initializer) {
        stmt->initializer->accept(this);
    }
    if (stmt->condition) {
        stmt->condition = optimize(stmt->condition);
    }
    if (stmt->increment) {
        stmt->increment = optimize(stmt->increment);
    }
    if (stmt->body) {
        stmt->body->accept(this);
    }
}

void SimpleOptimizer::visit(DoWhileStatement* stmt) {
    if (!stmt) return;
    
    if (stmt->body) {
        stmt->body->accept(this);
    }
    if (stmt->condition) {
        stmt->condition = optimize(stmt->condition);
    }
}

void SimpleOptimizer::visit(BlockStatement* stmt) {
    if (!stmt) return;
    
    for (auto& statement : stmt->statements) {
        if (statement) {
            statement->accept(this);
        }
    }
}

void SimpleOptimizer::visit(StructDefinition* stmt) {
    if (!stmt) return;
    // 结构体定义不需要优化
}

void SimpleOptimizer::visit(ClassDefinition* stmt) {
    if (!stmt) return;
    // 类定义不需要优化
}

void SimpleOptimizer::visit(BreakStatement* stmt) {
    if (!stmt) return;
    // 无需优化
}

void SimpleOptimizer::visit(ContinueStatement* stmt) {
    if (!stmt) return;
    // 无需优化
}

void SimpleOptimizer::visit(ReturnStatement* stmt) {
    if (!stmt) return;
    
    if (stmt->returnValue) {
        stmt->returnValue = optimize(stmt->returnValue);
    }
}

void SimpleOptimizer::visit(TryStatement* stmt) {
    if (!stmt) return;
    // Try语句暂时不优化
}

void SimpleOptimizer::visit(SwitchStatement* stmt) {
    if (!stmt) return;
    // Switch语句暂时不优化
}

void SimpleOptimizer::visit(FunctionPrototype* stmt) {
    if (!stmt) return;
    // 函数原型不需要优化
}

void SimpleOptimizer::visit(Identifier* id) {
    if (!id) return;
    // 标识符不需要优化
}

void SimpleOptimizer::visit(Variable* var) {
    if (!var) return;
    // 变量不需要优化
}



// 辅助方法实现
Expression* SimpleOptimizer::insertTypeCasts(Expression* expr) {
    if (!expr) return nullptr;
    
    // 递归处理表达式
    expr->accept(this);
    return expr;
}
