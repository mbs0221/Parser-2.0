#include "compiler/ssa_converter.h"
#include "parser/statement.h"
#include <iostream>

using namespace std;

// ==================== 语句转换实现 ====================

void SSAConverter::convertStatement(Statement* stmt) {
    if (!stmt) return;
    
    stmt->accept(this);
}

void SSAConverter::convertIfStatement(IfStatement* stmt) {
    if (!stmt) return;
    
    string elseLabel = createLabel("else");
    string endLabel = createLabel("endif");
    
    // 转换条件表达式
    convertExpression(stmt->condition);
    
    // 条件跳转到else分支
    addInstruction(VMInstructionType::JZ, 0, elseLabel);
    
    // 转换then分支
    convertStatement(stmt->thenStatement);
    
    // 跳转到结束
    addInstruction(VMInstructionType::JMP, 0, endLabel);
    
    // else标签
    currentFunction->addLabel(elseLabel);
    
    // 转换else分支
    if (stmt->elseStatement) {
        convertStatement(stmt->elseStatement);
    }
    
    // 结束标签
    currentFunction->addLabel(endLabel);
}

void SSAConverter::convertWhileStatement(WhileStatement* stmt) {
    if (!stmt) return;
    
    string loopLabel = createLabel("loop");
    string endLabel = createLabel("endloop");
    
    // 循环开始标签
    currentFunction->addLabel(loopLabel);
    
    // 转换条件表达式
    convertExpression(stmt->condition);
    
    // 条件跳转到结束
    addInstruction(VMInstructionType::JZ, 0, endLabel);
    
    // 转换循环体
    convertStatement(stmt->body);
    
    // 跳回循环开始
    addInstruction(VMInstructionType::JMP, 0, loopLabel);
    
    // 循环结束标签
    currentFunction->addLabel(endLabel);
}

void SSAConverter::convertForStatement(ForStatement* stmt) {
    if (!stmt) return;
    
    string loopLabel = createLabel("forloop");
    string endLabel = createLabel("endfor");
    
    // 初始化
    if (stmt->initialization) {
        convertStatement(stmt->initialization);
    }
    
    // 循环开始标签
    currentFunction->addLabel(loopLabel);
    
    // 条件检查
    if (stmt->condition) {
        convertExpression(stmt->condition);
        addInstruction(VMInstructionType::JZ, 0, endLabel);
    }
    
    // 循环体
    convertStatement(stmt->body);
    
    // 更新表达式
    if (stmt->update) {
        convertExpression(stmt->update);
    }
    
    // 跳回循环开始
    addInstruction(VMInstructionType::JMP, 0, loopLabel);
    
    // 循环结束标签
    currentFunction->addLabel(endLabel);
}

void SSAConverter::convertReturnStatement(ReturnStatement* stmt) {
    if (!stmt) return;
    
    // 转换返回值表达式
    if (stmt->value) {
        convertExpression(stmt->value);
    }
    
    // 生成返回指令
    addInstruction(VMInstructionType::RET);
}

void SSAConverter::convertVariableDefinition(VariableDefinition* stmt) {
    if (!stmt) return;
    
    for (const auto& var : stmt->variables) {
        string varName = var.first;
        Expression* initValue = var.second;
        
        // 声明变量
        declareVariable(varName);
        
        // 如果有初始值，转换并存储
        if (initValue) {
            convertExpression(initValue);
            int offset = getVariableOffset(varName);
            addInstruction(VMInstructionType::STORE, offset);
        }
    }
}

void SSAConverter::convertExpressionStatement(ExpressionStatement* stmt) {
    if (!stmt) return;
    
    convertExpression(stmt->expression);
    // 弹出栈顶值（表达式的结果）
    addInstruction(VMInstructionType::POP);
}

void SSAConverter::convertBlockStatement(BlockStatement* stmt) {
    if (!stmt) return;
    
    for (auto statement : stmt->statements) {
        convertStatement(statement);
    }
}

// ==================== 语句访问者模式实现 ====================
void SSAConverter::visit(Program* program) {
    if (!program) return;
    
    for (auto stmt : program->statements) {
        convertStatement(stmt);
    }
}

void SSAConverter::visit(Statement* stmt) {
    convertStatement(stmt);
}

void SSAConverter::visit(ImportStatement* stmt) {
    // 导入语句在编译时处理，不需要生成代码
}

void SSAConverter::visit(ExpressionStatement* stmt) {
    convertExpressionStatement(stmt);
}

void SSAConverter::visit(VariableDefinition* stmt) {
    convertVariableDefinition(stmt);
}

void SSAConverter::visit(IfStatement* stmt) {
    convertIfStatement(stmt);
}

void SSAConverter::visit(WhileStatement* stmt) {
    convertWhileStatement(stmt);
}

void SSAConverter::visit(ForStatement* stmt) {
    convertForStatement(stmt);
}

void SSAConverter::visit(DoWhileStatement* stmt) {
    // do-while循环转换
    if (!stmt) return;
    
    string loopLabel = createLabel("doloop");
    string endLabel = createLabel("enddoloop");
    
    // 循环开始标签
    currentFunction->addLabel(loopLabel);
    
    // 转换循环体
    convertStatement(stmt->body);
    
    // 转换条件表达式
    convertExpression(stmt->condition);
    
    // 条件为真时跳回循环开始
    addInstruction(VMInstructionType::JNZ, 0, loopLabel);
    
    // 循环结束标签
    currentFunction->addLabel(endLabel);
}

void SSAConverter::visit(BlockStatement* stmt) {
    convertBlockStatement(stmt);
}

void SSAConverter::visit(StructDefinition* stmt) {
    // 结构体定义在编译时处理，不需要生成代码
}

void SSAConverter::visit(ClassDefinition* stmt) {
    // 类定义在编译时处理，不需要生成代码
}

void SSAConverter::visit(BreakStatement* stmt) {
    // break语句需要跳转到循环结束，这里简化处理
    addInstruction(VMInstructionType::JMP, 0, "break_target");
}

void SSAConverter::visit(ContinueStatement* stmt) {
    // continue语句需要跳转到循环开始，这里简化处理
    addInstruction(VMInstructionType::JMP, 0, "continue_target");
}

void SSAConverter::visit(ReturnStatement* stmt) {
    convertReturnStatement(stmt);
}

void SSAConverter::visit(TryStatement* stmt) {
    // try-catch语句，简化处理
    if (stmt->tryBlock) {
        convertStatement(stmt->tryBlock);
    }
    if (stmt->catchBlock) {
        convertStatement(stmt->catchBlock);
    }
}

void SSAConverter::visit(SwitchStatement* stmt) {
    // switch语句，简化处理为if-else链
    if (stmt->expression) {
        convertExpression(stmt->expression);
    }
}

void SSAConverter::visit(FunctionPrototype* stmt) {
    // 函数原型，在编译时处理
}

void SSAConverter::visit(Identifier* id) {
    // 标识符访问
}

void SSAConverter::visit(Variable* var) {
    // 变量访问
}


