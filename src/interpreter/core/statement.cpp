#include "interpreter/core/interpreter.h"
#include "parser/expression.h"
#include "parser/definition.h"
#include "parser/inter.h"
#include "parser/parser.h"
#include "lexer/lexer.h"
#include "interpreter/utils/logger.h"
#include "interpreter/values/value.h"


#include <iostream>
#include <sstream>
#include <typeinfo>
#include <map>
#include <functional>

using namespace std;

template<>
Integer* ReturnException::getValue<Integer*>() const {
    return static_cast<Integer*>(returnValue);
}

// 语句访问方法 - 无返回值
void Interpreter::visit(Statement* stmt) {
    if (!stmt) return;
    
    // 使用动态分发调用对应的visit方法
    if (ImportStatement* importStmt = dynamic_cast<ImportStatement*>(stmt)) {
        visit(importStmt);
    } else if (ExpressionStatement* exprStmt = dynamic_cast<ExpressionStatement*>(stmt)) {
        visit(exprStmt);
    } else if (VariableDefinition* varDecl = dynamic_cast<VariableDefinition*>(stmt)) {
        visit(varDecl);
    } else if (IfStatement* ifStmt = dynamic_cast<IfStatement*>(stmt)) {
        visit(ifStmt);
    } else if (WhileStatement* whileStmt = dynamic_cast<WhileStatement*>(stmt)) {
        visit(whileStmt);
    } else if (ForStatement* forStmt = dynamic_cast<ForStatement*>(stmt)) {
        visit(forStmt);
    } else if (DoWhileStatement* doWhileStmt = dynamic_cast<DoWhileStatement*>(stmt)) {
        visit(doWhileStmt);
    } else if (BlockStatement* blockStmt = dynamic_cast<BlockStatement*>(stmt)) {
        visit(blockStmt);
    } else if (BreakStatement* breakStmt = dynamic_cast<BreakStatement*>(stmt)) {
        visit(breakStmt);
    } else if (ContinueStatement* continueStmt = dynamic_cast<ContinueStatement*>(stmt)) {
        visit(continueStmt);
    } else if (ReturnStatement* returnStmt = dynamic_cast<ReturnStatement*>(stmt)) {
        visit(returnStmt);
    } else if (TryStatement* tryStmt = dynamic_cast<TryStatement*>(stmt)) {
        visit(tryStmt);
    } else if (SwitchStatement* switchStmt = dynamic_cast<SwitchStatement*>(stmt)) {
        visit(switchStmt);
    } else if (FunctionDefinition* funcDef = dynamic_cast<FunctionDefinition*>(stmt)) {
        visit(funcDef);
    } else {
        reportError("Unknown statement type");
    }
}

void Interpreter::visit(ReturnStatement* returnStmt) {
    if (!returnStmt) return;
    
    Value* result = nullptr;
    if (returnStmt->returnValue) {
        LOG_DEBUG("Executing return statement with value");
        result = visit(returnStmt->returnValue);
    } else {
        LOG_DEBUG("Executing return statement without value");
    }
    
    // 抛出带有返回值的ReturnException
    throw ReturnException(result);
}



// 导入语句执行
void Interpreter::visit(ImportStatement* importStmt) {
    if (!importStmt) return;
    
    string moduleName = importStmt->moduleName;
    LOG_DEBUG("Importing module: " + moduleName);
    
    // 检查文件是否存在
    ifstream file(moduleName);
    if (!file.is_open()) {
        LOG_ERROR("Error: Cannot open module file '" + moduleName + "'");
        return;
    }
    file.close();
    
    // 创建新的解析器来解析导入的模块
    Parser parser;
    Program* importedProgram = parser.parse(moduleName);
    
    if (!importedProgram) {
        cout << "Error: Failed to parse module '" + moduleName + "'" << endl;
        return;
    }
    
    // 执行导入的模块（在当前作用域中）
    LOG_DEBUG("Executing imported module: " + moduleName);
    visit(importedProgram);  // 直接调用visit方法，消除execute函数依赖
    
    // 清理导入的程序
    delete importedProgram;
    
    LOG_DEBUG("Module import completed: " + moduleName);
}

// 表达式语句执行
void Interpreter::visit(ExpressionStatement* stmt) {
    if (!stmt || !stmt->expression) return;
    
    visit(stmt->expression);         
}

// 条件语句执行
void Interpreter::visit(IfStatement* ifStmt) {
    if (!ifStmt || !ifStmt->condition) return;
    
    Value* conditionValue = visit(ifStmt->condition);
    
    // 检查条件是否为真（非零值）
    bool conditionBool = false;
    if (Integer* intVal = dynamic_cast<Integer*>(conditionValue)) {
        conditionBool = (intVal->getValue() != 0);
        LOG_DEBUG(std::string("Condition value: ") + to_string(intVal->getValue()) + " (bool: " + (conditionBool ? "true" : "false") + ")");
    } else if (Bool* boolVal = dynamic_cast<Bool*>(conditionValue)) {
        conditionBool = boolVal->getValue();
        LOG_DEBUG(std::string("Condition value: ") + (conditionBool ? "true" : "false"));
    }
    
    if (conditionBool && ifStmt->thenStatement) {
        LOG_DEBUG("Executing then branch");
        // 为then分支创建独立作用域
        withScopeVoid([&]() {
            visit(ifStmt->thenStatement);  // 直接调用visit方法，消除execute函数依赖
        });
    } else if (!conditionBool && ifStmt->elseStatement) {
        LOG_DEBUG("Executing else branch");
        // 为else分支创建独立作用域
        withScopeVoid([&]() {
            visit(ifStmt->elseStatement);  // 直接调用visit方法，消除execute函数依赖
        });
    }
}

// 循环语句执行
void Interpreter::visit(WhileStatement* whileStmt) {
    if (!whileStmt || !whileStmt->condition || !whileStmt->body) return;
    
    while (true) {
        // 评估循环条件
        Value* conditionValue = visit(whileStmt->condition);
        
        // 检查条件是否为真（非零值）
        bool conditionBool = false;
        if (Integer* intVal = dynamic_cast<Integer*>(conditionValue)) {
            conditionBool = (intVal->getValue() != 0);
            LOG_DEBUG(std::string("While condition value: ") + to_string(intVal->getValue()) + " (bool: " + (conditionBool ? "true" : "false") + ")");
        } else if (Bool* boolVal = dynamic_cast<Bool*>(conditionValue)) {
            conditionBool = boolVal->getValue();
            LOG_DEBUG(std::string("While condition value: ") + (conditionBool ? "true" : "false"));
        }
        
        // 如果条件为假，退出循环
        if (!conditionBool) {
            LOG_DEBUG("While condition is false, exiting loop");
            break;
        }
        
        // 执行循环体（无论是单个语句还是语句块，都会抛出相同的异常）
        // 注意：不创建新作用域，让循环体内的变量赋值影响外层作用域
        try {
            if (BlockStatement* block = dynamic_cast<BlockStatement*>(whileStmt->body)) {
            // 如果是块语句，直接执行其中的语句而不创建新作用域
            for (Statement* stmt : block->statements) {
                visit(stmt);  // 直接调用visit方法，消除execute函数依赖
            }
            } else {
                // 如果是单个语句，正常执行
                visit(whileStmt->body);  // 直接调用visit方法，消除execute函数依赖
            }
        } catch (const BreakException&) {
            return;  // 退出循环
        } catch (const ContinueException&) {
            continue;  // 跳过当前迭代，继续下一次循环
        } catch (const ReturnException&) {
            throw;  // 重新抛出return异常
        }
    }
}

// for循环语句执行
void Interpreter::visit(ForStatement* forStmt) {
    if (!forStmt || !forStmt->condition || !forStmt->body) return;
    
    // 执行初始化表达式
    if (forStmt->initializer) {
        visit(forStmt->initializer);
    }
    
    while (true) {
        // 评估循环条件
        Value* conditionValue = visit(forStmt->condition);
        
        // 检查条件是否为真（非零值）
        bool conditionBool = false;
        if (Integer* intVal = dynamic_cast<Integer*>(conditionValue)) {
            conditionBool = (intVal->getValue() != 0);
            LOG_DEBUG(std::string("For condition value: ") + to_string(intVal->getValue()) + " (bool: " + (conditionBool ? "true" : "false") + ")");
        } else if (Bool* boolVal = dynamic_cast<Bool*>(conditionValue)) {
            conditionBool = boolVal->getValue();
            LOG_DEBUG(std::string("For condition value: ") + (conditionBool ? "true" : "false"));
        }
        
        // 如果条件为假，退出循环
        if (!conditionBool) {
            LOG_DEBUG("For condition is false, exiting loop");
            break;
        }
        
        // 执行循环体
        LOG_DEBUG("Executing for loop body");
        try {
            visit(forStmt->body);  // 直接调用visit方法，消除execute函数依赖
        } catch (const BreakException&) {
            return;  // 退出循环
        } catch (const ContinueException&) {
            // 执行增量表达式，然后继续下一次循环
            if (forStmt->increment) {
                visit(forStmt->increment);
            }
            continue;  // 跳过当前迭代，继续下一次循环
        } catch (const ReturnException&) {
            throw;  // 重新抛出return异常
        }
        
        // 执行增量表达式
        if (forStmt->increment) {
            visit(forStmt->increment);
        }
    }
}

// do-while循环语句执行
void Interpreter::visit(DoWhileStatement* doWhileStmt) {
    if (!doWhileStmt || !doWhileStmt->condition || !doWhileStmt->body) return;
    
    do {
        // 执行循环体
        LOG_DEBUG("Executing do-while loop body");
        try {
            visit(doWhileStmt->body);  // 直接调用visit方法，消除execute函数依赖
        } catch (const BreakException&) {
            return;  // 退出循环
        } catch (const ContinueException&) {
            continue;  // 跳过当前迭代，继续下一次循环
        } catch (const ReturnException&) {
            throw;  // 重新抛出return异常
        }
        
        // 评估循环条件
        Value* conditionValue = visit(doWhileStmt->condition);
        
        // 检查条件是否为真（非零值）
        bool conditionBool = false;
        if (Integer* intVal = dynamic_cast<Integer*>(conditionValue)) {
            conditionBool = (intVal->getValue() != 0);
            LOG_DEBUG(std::string("Do-while condition value: ") + to_string(intVal->getValue()) + " (bool: " + (conditionBool ? "true" : "false") + ")");
        } else if (Bool* boolVal = dynamic_cast<Bool*>(conditionValue)) {
            conditionBool = boolVal->getValue();
            LOG_DEBUG(std::string("Do-while condition value: ") + (conditionBool ? "true" : "false"));
        }
        
        // 如果条件为假，退出循环
        if (!conditionBool) {
            LOG_DEBUG("Do-while condition is false, exiting loop");
            break;
        }
    } while (true);
}

// switch语句执行
void Interpreter::visit(SwitchStatement* switchStmt) {
    if (!switchStmt || !switchStmt->expression) return;
    
    // 求值switch表达式
    Value* switchValue = visit(switchStmt->expression);
    if (!switchValue) {
        reportError("Switch expression evaluation failed");
        return;
    }
    
    LOG_DEBUG("Switch expression value: " + switchValue->toString());
    
    // 查找匹配的case
    bool foundMatch = false;
    bool executedDefault = false;
    
    for (const auto& switchCase : switchStmt->cases) {
        // 检查是否是default分支
        if (!switchCase.value) {
            // default分支 - 只有在没有找到匹配的case时才执行
            if (!foundMatch && !executedDefault) {
                LOG_DEBUG("Executing default case");
                executedDefault = true;
                
                // 执行default分支的语句
                for (Statement* stmt : switchCase.statements) {
                    try {
                        visit(stmt);  // 直接调用visit方法，消除execute函数依赖
                    } catch (const BreakException&) {
                        return;  // 退出switch语句
                    } catch (const ContinueException&) {
                        // continue在switch中应该跳出switch，继续外层循环
                        throw;  // 重新抛出continue异常
                    } catch (const ReturnException&) {
                        throw;  // 重新抛出return异常
                    }
                }
            }
        } else {
            // 普通case分支 - 比较值
            Value* caseValue = visit(switchCase.value);
            if (!caseValue) {
                reportError("Case expression evaluation failed");
                continue;
            }
            
            // 比较switch值和case值
            // 直接调用操作符来判断是否相等
            Value* matches = calculator->executeBinaryOperation(switchValue, caseValue, lexer::Operator::EQ);
            if (dynamic_cast<Bool*>(matches)->getValue()) {
                LOG_DEBUG("Executing matching case");
                foundMatch = true;
                
                // 执行匹配的case分支的语句
                for (Statement* stmt : switchCase.statements) {
                    try {
                        visit(stmt);  // 直接调用visit方法，消除execute函数依赖
                    } catch (const BreakException&) {
                        return;  // 退出switch语句
                    } catch (const ContinueException&) {
                        // continue在switch中应该跳出switch，继续外层循环
                        throw;  // 重新抛出continue异常
                    } catch (const ReturnException&) {
                        throw;  // 重新抛出return异常
                    }
                }
                
                // 找到匹配的case后，继续执行后续的case（fall-through）
                // 除非遇到break语句
            } else if (foundMatch) {
                // 如果已经找到了匹配的case，继续执行后续case（fall-through）
                LOG_DEBUG("Executing fall-through case");
                
                // 执行当前case分支的语句
                for (Statement* stmt : switchCase.statements) {
                    try {
                        visit(stmt);  // 直接调用visit方法，消除execute函数依赖
                    } catch (const BreakException&) {
                        return;  // 退出switch语句
                    } catch (const ContinueException&) {
                        // continue在switch中应该跳出switch，继续外层循环
                        throw;  // 重新抛出continue异常
                    } catch (const ReturnException&) {
                        throw;  // 重新抛出return异常
                    }
                }
            }
        }
    }
    
    LOG_DEBUG("Switch statement execution completed");
}

void Interpreter::visit(TryStatement* tryStmt) {
    if (!tryStmt) return;
    
    LOG_DEBUG("Executing try statement");
    
    try {
        // 执行try块
        if (tryStmt->tryBlock) {
            visit(tryStmt->tryBlock);  // 直接调用visit方法，消除execute函数依赖
        }
    } catch (...) {
        LOG_DEBUG("Exception caught");
        
        // 查找匹配的catch块
        bool caught = false;
        for (const auto& catchBlock : tryStmt->catchBlocks) {
            // 简化处理：如果有catch块，就执行它
            if (!catchBlock.exceptionName.empty()) {
                // 创建一个简单的异常值对象
                String* exceptionValue = new String("Exception occurred");
                scopeManager.defineVariable(catchBlock.exceptionName, exceptionValue);
            }
            
            // 执行catch块
            if (catchBlock.catchBlock) {
                visit(catchBlock.catchBlock);  // 直接调用visit方法，消除execute函数依赖
            }
            
            caught = true;
            break;  
        }
        
        // 如果没有找到匹配的catch块，重新抛出异常
        if (!caught) {
            LOG_DEBUG("No matching catch block found, re-throwing exception");
            throw;
        }
    }
    
            // 执行finally块（无论是否发生异常）
        if (tryStmt->finallyBlock) {
            LOG_DEBUG("Executing finally block");
            try {
                visit(tryStmt->finallyBlock);  // 直接调用visit方法，消除execute函数依赖
            } catch (...) {
            // finally块中的异常不应该被try-catch捕获
            LOG_DEBUG("Exception in finally block, re-throwing");
            throw;
        }
    }
    
    LOG_DEBUG("Try statement execution completed");
}

// break语句执行
void Interpreter::visit(BreakStatement* breakStmt) {
    if (!breakStmt) return;
    LOG_DEBUG("Executing break statement");
    throw BreakException();
}

// continue语句执行
void Interpreter::visit(ContinueStatement* continueStmt) {
    if (!continueStmt) return;
    LOG_DEBUG("Executing continue statement");
    throw ContinueException();
}

// 语句块执行
void Interpreter::visit(BlockStatement* block) {
    if (!block) return;
    
    withScopeVoid([&]() {
        for (Statement* stmt : block->statements) {
            visit(stmt);  // 直接调用visit方法，消除execute函数依赖
        }
    });
}

// 执行用户函数的回调函数已移除 - 不再需要

// 函数定义访问已移动到 definition_visits.cpp

// 类方法访问已移动到 definition_visits.cpp
