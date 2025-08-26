#include "ast_optimizer.h"
#include <iostream>
#include <typeinfo>

using namespace std;

// TypeCastOptimizer实现

Expression* TypeCastOptimizer::optimize(Expression* expr) {
    if (!expr) return nullptr;
    
    // 递归优化表达式
    return insertTypeCasts(expr);
}

Statement* TypeCastOptimizer::optimize(Statement* stmt) {
    if (!stmt) return nullptr;
    
    // 让语句自己决定如何被优化
    stmt->accept(this);
    return stmt;
}

Program* TypeCastOptimizer::optimize(Program* program) {
    if (!program) return nullptr;
    
    // 优化程序中的所有语句
    for (auto& stmt : program->statements) {
        if (stmt) {
            optimize(stmt);
        }
    }
    return program;
}

// 访问者模式实现 - 大部分只是传递，不需要特殊处理
void TypeCastOptimizer::visit(Program* program) {
    if (!program) return;
    
    for (auto& stmt : program->statements) {
        if (stmt) {
            stmt->accept(this);
        }
    }
}

void TypeCastOptimizer::visit(VariableDeclaration* decl) {
    if (!decl) return;
    
    for (auto& var : decl->variables) {
        if (var.initializer) {
            var.initializer = optimize(var.initializer);
        }
    }
}

void TypeCastOptimizer::visit(FunctionDefinition* func) {
    if (!func) return;
    
    if (func->body) {
        func->body->accept(this);
    }
}

void TypeCastOptimizer::visit(ClassDefinition* cls) {
    if (!cls) return;
    
    // ClassMethod不是AST节点，不需要优化
    // 如果需要优化方法体，应该在解析时处理
}

void TypeCastOptimizer::visit(IfStatement* stmt) {
    if (!stmt) return;
    
    if (stmt->condition) {
        stmt->condition = optimize(stmt->condition);
    }
    if (stmt->thenBranch) {
        stmt->thenBranch->accept(this);
    }
    if (stmt->elseBranch) {
        stmt->elseBranch->accept(this);
    }
}

void TypeCastOptimizer::visit(WhileStatement* stmt) {
    if (!stmt) return;
    
    if (stmt->condition) {
        stmt->condition = optimize(stmt->condition);
    }
    if (stmt->body) {
        stmt->body->accept(this);
    }
}

void TypeCastOptimizer::visit(ForStatement* stmt) {
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

void TypeCastOptimizer::visit(DoWhileStatement* stmt) {
    if (!stmt) return;
    
    if (stmt->body) {
        stmt->body->accept(this);
    }
    if (stmt->condition) {
        stmt->condition = optimize(stmt->condition);
    }
}

void TypeCastOptimizer::visit(ReturnStatement* stmt) {
    if (!stmt) return;
    
    if (stmt->returnValue) {
        stmt->returnValue = optimize(stmt->returnValue);
    }
}

void TypeCastOptimizer::visit(BreakStatement* stmt) {
    // 无需优化
}

void TypeCastOptimizer::visit(ContinueStatement* stmt) {
    // 无需优化
}

void TypeCastOptimizer::visit(ExpressionStatement* stmt) {
    if (!stmt) return;
    
    if (stmt->expression) {
        stmt->expression = optimize(stmt->expression);
    }
}

void TypeCastOptimizer::visit(BlockStatement* stmt) {
    if (!stmt) return;
    
    for (auto& statement : stmt->statements) {
        if (statement) {
            statement->accept(this);
        }
    }
}

void TypeCastOptimizer::visit(ImportStatement* stmt) {
    // 无需优化
}

// 表达式访问者实现
void TypeCastOptimizer::visit(IdentifierExpression* expr) {
    // 无需优化
}



void TypeCastOptimizer::visit(StringLiteral* expr) {
    // 无需优化
}

void TypeCastOptimizer::visit(CharExpression* expr) {
    // 无需优化
}

void TypeCastOptimizer::visit(BoolExpression* expr) {
    // 无需优化
}

void TypeCastOptimizer::visit(ArrayNode* expr) {
    if (!expr) return;
    
    for (auto& element : expr->elements) {
        if (element) {
            element = optimize(element);
        }
    }
}

void TypeCastOptimizer::visit(DictNode* expr) {
    if (!expr) return;
    
    // 获取所有键
    vector<string> keys = expr->getKeys();
    for (const string& key : keys) {
        Expression* value = expr->getEntry(key);
        if (value) {
            Expression* optimizedValue = optimize(value);
            expr->setEntry(key, optimizedValue);
        }
    }
}

void TypeCastOptimizer::visit(AssignmentExpression* expr) {
    if (!expr) return;
    
    if (expr->value) {
        expr->value = optimize(expr->value);
    }
}

void TypeCastOptimizer::visit(BinaryExpression* expr) {
    if (!expr) return;
    
    // 递归优化左右操作数
    if (expr->left) {
        expr->left = optimize(expr->left);
    }
    if (expr->right) {
        expr->right = optimize(expr->right);
    }
    
    // 根据操作符类型进行相应的类型转换
    if (expr->left && expr->right) {
        auto opType = expr->getOperatorType();
        
        switch (opType) {
            case BinaryExpression::OperatorType::LOGICAL: {
                // 逻辑运算：两个操作数都转换为布尔类型
                if (!dynamic_cast<BoolExpression*>(expr->left)) {
                    expr->left = createCastExpressionForType(expr->left, new BoolExpression(true));
                }
                if (!dynamic_cast<BoolExpression*>(expr->right)) {
                    expr->right = createCastExpressionForType(expr->right, new BoolExpression(true));
                }
                break;
            }
            case BinaryExpression::OperatorType::ARITHMETIC: {
                // 算术运算：转换为数值类型
                if (expr->operator_->Tag == '+' && 
                    (dynamic_cast<StringLiteral*>(expr->left) || dynamic_cast<StringLiteral*>(expr->right))) {
                    // 字符串拼接：转换为字符串类型
                    if (!dynamic_cast<StringLiteral*>(expr->left)) {
                        expr->left = createCastExpressionForType(expr->left, new StringLiteral(""));
                    }
                    if (!dynamic_cast<StringLiteral*>(expr->right)) {
                        expr->right = createCastExpressionForType(expr->right, new StringLiteral(""));
                    }
                } else {
                    // 数值运算：转换为数值类型
                    if (typeid(*expr->left) != typeid(*expr->right)) {
                        if (shouldConvertLeftToRight(expr->left, expr->right, std::string(1, expr->operator_->Tag))) {
                            expr->left = createCastExpressionForType(expr->left, expr->right);
                        } else {
                            expr->right = createCastExpressionForType(expr->right, expr->left);
                        }
                    }
                }
                break;
            }
            case BinaryExpression::OperatorType::COMPARISON: {
                // 比较运算：转换为相同类型
                if (typeid(*expr->left) != typeid(*expr->right)) {
                    if (shouldConvertLeftToRight(expr->left, expr->right, std::string(1, expr->operator_->Tag))) {
                        expr->left = createCastExpressionForType(expr->left, expr->right);
                    } else {
                        expr->right = createCastExpressionForType(expr->right, expr->left);
                    }
                }
                break;
            }
        }
    }
}

void TypeCastOptimizer::visit(UnaryExpression* expr) {
    if (!expr) return;
    
    if (expr->operand) {
        expr->operand = optimize(expr->operand);
    }
}

void TypeCastOptimizer::visit(CallExpression* expr) {
    if (!expr) return;
    
    for (auto& arg : expr->arguments) {
        if (arg) {
            arg = optimize(arg);
        }
    }
}

void TypeCastOptimizer::visit(AccessExpression* expr) {
    if (!expr) return;
    
    if (expr->target) {
        expr->target = optimize(expr->target);
    }
    if (expr->key) {
        expr->key = optimize(expr->key);
    }
}



void TypeCastOptimizer::visit(IntExpression* expr) {
    // 无需优化
}

void TypeCastOptimizer::visit(DoubleExpression* expr) {
    // 无需优化
}

// 辅助方法实现
bool TypeCastOptimizer::shouldConvertLeftToRight(Expression* left, Expression* right, const string& operator_) {
    // 字符串连接操作符
    if (operator_ == "+" || operator_ == ".") {
        if (dynamic_cast<StringLiteral*>(right)) {
            return true; // 将左操作数转换为字符串
        }
    }
    
    // 其他操作符使用类型优先级
    int leftPriority = getTypePriority(left);
    int rightPriority = getTypePriority(right);
    
    return rightPriority > leftPriority;
}

int TypeCastOptimizer::getTypePriority(Expression* expr) {
    if (dynamic_cast<StringLiteral*>(expr)) return 4;      // 字符串优先级最高
    if (dynamic_cast<DoubleExpression*>(expr)) return 3; // 浮点数
    if (dynamic_cast<IntExpression*>(expr)) return 2;    // 整数
    if (dynamic_cast<CharExpression*>(expr)) return 1;   // 字符优先级最低
    return 0; // 未知类型
}



Expression* TypeCastOptimizer::insertTypeCasts(Expression* expr) {
    if (!expr) return nullptr;
    
    // 递归处理表达式
    expr->accept(this);
    return expr;
}

// 根据目标类型创建相应的类型转换表达式
Expression* TypeCastOptimizer::createCastExpressionForType(Expression* expr, Expression* targetTypeExpr) {
    if (dynamic_cast<IntExpression*>(targetTypeExpr)) {
        return new CastExpression<IntExpression>(expr);
    } else if (dynamic_cast<DoubleExpression*>(targetTypeExpr)) {
        return new CastExpression<DoubleExpression>(expr);
    } else if (dynamic_cast<StringLiteral*>(targetTypeExpr)) {
        return new CastExpression<StringLiteral>(expr);
    } else if (dynamic_cast<CharExpression*>(targetTypeExpr)) {
        return new CastExpression<CharExpression>(expr);
    } else if (dynamic_cast<BoolExpression*>(targetTypeExpr)) {
        return new CastExpression<BoolExpression>(expr);
    }
    
    // 默认转换为整数
    return new CastExpression<IntExpression>(expr);
}

// 添加缺失的visit方法实现
void TypeCastOptimizer::visit(MethodCallExpression* expr) {
    if (!expr) return;
    
    if (expr->object) {
        expr->object = optimize(expr->object);
    }
    for (auto& arg : expr->arguments) {
        if (arg) {
            arg = optimize(arg);
        }
    }
}

void TypeCastOptimizer::visit(CastExpression<IntExpression>* expr) {
    if (!expr) return;
    
    if (expr->operand) {
        expr->operand = optimize(expr->operand);
    }
}

void TypeCastOptimizer::visit(CastExpression<DoubleExpression>* expr) {
    if (!expr) return;
    
    if (expr->operand) {
        expr->operand = optimize(expr->operand);
    }
}

void TypeCastOptimizer::visit(CastExpression<StringLiteral>* expr) {
    if (!expr) return;
    
    if (expr->operand) {
        expr->operand = optimize(expr->operand);
    }
}

void TypeCastOptimizer::visit(CastExpression<CharExpression>* expr) {
    if (!expr) return;
    
    if (expr->operand) {
        expr->operand = optimize(expr->operand);
    }
}

void TypeCastOptimizer::visit(CastExpression<BoolExpression>* expr) {
    if (!expr) return;
    
    if (expr->operand) {
        expr->operand = optimize(expr->operand);
    }
}

void TypeCastOptimizer::visit(StructInstantiationExpression* expr) {
    // 暂时简单实现
}

void TypeCastOptimizer::visit(ClassInstantiationExpression* expr) {
    // 暂时简单实现
}

void TypeCastOptimizer::visit(MemberAccessExpression* expr) {
    // 暂时简单实现
}

void TypeCastOptimizer::visit(StructDefinition* stmt) {
    if (!stmt) return;
    
    for (auto& member : stmt->members) {
        if (member.defaultValue) {
            member.defaultValue = optimize(member.defaultValue);
        }
    }
}

void TypeCastOptimizer::visit(ThrowStatement* stmt) {
    if (!stmt) return;
    
    if (stmt->expression) {
        stmt->expression = optimize(stmt->expression);
    }
}

void TypeCastOptimizer::visit(TryStatement* stmt) {
    if (!stmt) return;
    
    if (stmt->tryBlock) {
        stmt->tryBlock = optimize(stmt->tryBlock);
    }
    if (stmt->catchBlock) {
        stmt->catchBlock = optimize(stmt->catchBlock);
    }
    if (stmt->finallyBlock) {
        stmt->finallyBlock = optimize(stmt->finallyBlock);
    }
}

void TypeCastOptimizer::visit(CatchStatement* stmt) {
    if (!stmt) return;
    
    if (stmt->body) {
        stmt->body = optimize(stmt->body);
    }
}

void TypeCastOptimizer::visit(FinallyStatement* stmt) {
    if (!stmt) return;
    
    if (stmt->body) {
        stmt->body = optimize(stmt->body);
    }
}

void TypeCastOptimizer::visit(SwitchStatement* stmt) {
    if (!stmt) return;
    
    if (stmt->expression) {
        stmt->expression = optimize(stmt->expression);
    }
    for (auto& caseStmt : stmt->cases) {
        if (caseStmt) {
            caseStmt = optimize(caseStmt);
        }
    }
    if (stmt->defaultCase) {
        stmt->defaultCase = optimize(stmt->defaultCase);
    }
}

void TypeCastOptimizer::visit(CaseStatement* stmt) {
    if (!stmt) return;
    
    if (stmt->expression) {
        stmt->expression = optimize(stmt->expression);
    }
    if (stmt->body) {
        stmt->body = optimize(stmt->body);
    }
}

void TypeCastOptimizer::visit(DefaultStatement* stmt) {
    if (!stmt) return;
    
    if (stmt->body) {
        stmt->body = optimize(stmt->body);
    }
}

void TypeCastOptimizer::visit(FunctionPrototype* stmt) {
    // 函数原型不需要优化
}
