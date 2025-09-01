#include "compiler/ssa_converter.h"
#include "lexer/value.h"
#include <iostream>

using namespace std;

// ==================== 表达式转换实现 ====================

void SSAConverter::convertExpression(Expression* expr) {
    if (!expr) return;
    
    expr->accept(this);
}

void SSAConverter::convertBinaryExpression(BinaryExpression* expr) {
    if (!expr) return;
    
    // 先转换左操作数
    convertExpression(expr->left);
    
    // 再转换右操作数
    convertExpression(expr->right);
    
    // 根据操作符生成相应的指令
    if (expr->operator_) {
        string op = expr->operator_->lexeme;
        if (op == "+") {
            addInstruction(VMInstructionType::ADD);
        } else if (op == "-") {
            addInstruction(VMInstructionType::SUB);
        } else if (op == "*") {
            addInstruction(VMInstructionType::MUL);
        } else if (op == "/") {
            addInstruction(VMInstructionType::DIV);
        } else if (op == "%") {
            addInstruction(VMInstructionType::MOD);
        } else if (op == "==") {
            addInstruction(VMInstructionType::EQ);
        } else if (op == "!=") {
            addInstruction(VMInstructionType::NE);
        } else if (op == "<") {
            addInstruction(VMInstructionType::LT);
        } else if (op == "<=") {
            addInstruction(VMInstructionType::LE);
        } else if (op == ">") {
            addInstruction(VMInstructionType::GT);
        } else if (op == ">=") {
            addInstruction(VMInstructionType::GE);
        } else if (op == "&&") {
            addInstruction(VMInstructionType::AND);
        } else if (op == "||") {
            addInstruction(VMInstructionType::OR);
        }
    }
}

void SSAConverter::convertUnaryExpression(UnaryExpression* expr) {
    if (!expr) return;
    
    // 先转换操作数
    convertExpression(expr->operand);
    
    // 根据操作符生成指令
    if (expr->operator_) {
        string op = expr->operator_->lexeme;
        if (op == "!") {
            addInstruction(VMInstructionType::NOT);
        } else if (op == "-") {
            addInstruction(VMInstructionType::SUB);
        }
    }
}

void SSAConverter::convertCallExpression(CallExpression* expr) {
    if (!expr) return;
    
    // 转换所有参数
    for (auto arg : expr->arguments) {
        convertExpression(arg);
    }
    
    // 生成函数调用指令
    if (expr->function) {
        addInstruction(VMInstructionType::CALL, 0, expr->function->name);
    }
}

void SSAConverter::convertAssignExpression(AssignExpression* expr) {
    if (!expr) return;
    
    // 转换右值
    convertExpression(expr->value);
    
    // 根据左值类型生成存储指令
    if (expr->target) {
        string targetName = expr->target->name;
        int offset = getVariableOffset(targetName);
        if (offset >= 0) {
            addInstruction(VMInstructionType::STORE, offset);
        } else {
            // 全局变量
            program->addGlobal(targetName);
            addInstruction(VMInstructionType::STORE_GLOBAL, globals.size() - 1);
        }
    }
}

void SSAConverter::convertVariableExpression(VariableExpression* expr) {
    if (!expr) return;
    
    string varName = expr->name;
    int offset = getVariableOffset(varName);
    if (offset >= 0) {
        addInstruction(VMInstructionType::LOAD, offset);
    } else {
        // 全局变量
        program->addGlobal(varName);
        addInstruction(VMInstructionType::LOAD_GLOBAL, globals.size() - 1);
    }
}

void SSAConverter::convertConstantExpression(ConstantExpression* expr) {
    if (!expr) return;
    
    // 根据常量类型生成PUSH指令
    switch (expr->type) {
        case ConstantExpression::INT:
            addInstruction(VMInstructionType::PUSH, expr->getIntValue());
            break;
        case ConstantExpression::DOUBLE:
            // 简化处理，将double转为int
            addInstruction(VMInstructionType::PUSH, (int)expr->getDoubleValue());
            break;
        case ConstantExpression::BOOL:
            addInstruction(VMInstructionType::PUSH, expr->getBoolValue() ? 1 : 0);
            break;
        case ConstantExpression::CHAR:
            addInstruction(VMInstructionType::PUSH, (int)expr->getCharValue());
            break;
        case ConstantExpression::STRING:
            // 字符串常量需要特殊处理，这里简化
            addInstruction(VMInstructionType::PUSH, 0); // 字符串ID
            break;
        default:
            break;
    }
}

// ==================== 表达式访问者模式实现 ====================
Value* SSAConverter::visit(Expression* expr) {
    convertExpression(expr);
    return nullptr;
}

Value* SSAConverter::visit(ConstantExpression* expr) {
    convertConstantExpression(expr);
    return nullptr;
}

Value* SSAConverter::visit(VariableExpression* expr) {
    convertVariableExpression(expr);
    return nullptr;
}

Value* SSAConverter::visit(UnaryExpression* expr) {
    convertUnaryExpression(expr);
    return nullptr;
}

Value* SSAConverter::visit(BinaryExpression* expr) {
    convertBinaryExpression(expr);
    return nullptr;
}

Value* SSAConverter::visit(AssignExpression* expr) {
    convertAssignExpression(expr);
    return nullptr;
}

Value* SSAConverter::visit(AccessExpression* expr) {
    // 数组访问，简化处理
    if (expr->array) {
        convertExpression(expr->array);
    }
    if (expr->index) {
        convertExpression(expr->index);
    }
    addInstruction(VMInstructionType::ARRAY_LOAD);
    return nullptr;
}

Value* SSAConverter::visit(CallExpression* expr) {
    convertCallExpression(expr);
    return nullptr;
}



Value* SSAConverter::visit(CastExpression* expr) {
    convertExpression(expr->operand);
    return nullptr;
}
