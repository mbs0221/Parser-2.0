
#include "interpreter/interpreter.h"
#include "parser/expression.h"
#include "parser/function.h"
#include "parser/inter.h"
#include "lexer/lexer.h"
#include "interpreter/logger.h"
#include "lexer/value.h"

#include <iostream>
#include <sstream>
#include <typeinfo>
#include <map>
#include <functional>

using namespace std;

// 求值表达式 - 使用访问者模式
Value* Interpreter::visit(Expression* expr) {
    if (!expr) return nullptr;
    
    // 使用动态分发调用对应的visit方法
    if (ConstantExpression* constExpr = dynamic_cast<ConstantExpression*>(expr)) {
        return visit(constExpr);
    } else if (VariableExpression* varExpr = dynamic_cast<VariableExpression*>(expr)) {
        return visit(varExpr);
    } else if (UnaryExpression* unaryExpr = dynamic_cast<UnaryExpression*>(expr)) {
        return visit(unaryExpr);
    } else if (AssignExpression* assignExpr = dynamic_cast<AssignExpression*>(expr)) {
        return visit(assignExpr);
    } else if (BinaryExpression* binaryExpr = dynamic_cast<BinaryExpression*>(expr)) {
        return visit(binaryExpr);
    } else if (CastExpression<Integer>* intCastExpr = dynamic_cast<CastExpression<Integer>*>(expr)) {
        return visit(intCastExpr);
    } else if (CastExpression<Double>* doubleCastExpr = dynamic_cast<CastExpression<Double>*>(expr)) {
        return visit(doubleCastExpr);
    } else if (CastExpression<Bool>* boolCastExpr = dynamic_cast<CastExpression<Bool>*>(expr)) {
        return visit(boolCastExpr);
    } else if (CastExpression<Char>* charCastExpr = dynamic_cast<CastExpression<Char>*>(expr)) {
        return visit(charCastExpr);
    } else if (CastExpression<String>* stringCastExpr = dynamic_cast<CastExpression<String>*>(expr)) {
        return visit(stringCastExpr);
    } else if (AccessExpression* accessExpr = dynamic_cast<AccessExpression*>(expr)) {
        return visit(accessExpr);
    } else if (CallExpression* callExpr = dynamic_cast<CallExpression*>(expr)) {
        return visit(callExpr);
    } else if (MethodCallExpression* methodExpr = dynamic_cast<MethodCallExpression*>(expr)) {
        return visit(methodExpr);
    }
    
    reportError("Unknown expression type");
    return nullptr;
}

// 添加缺失的visit方法实现
Value* Interpreter::visit(ConstantExpression* constExpr) {
    if (!constExpr || !constExpr->value) return nullptr;
    LOG_DEBUG("ConstantExpression: " + constExpr->getLocation());
    
    // 直接返回value，因为现在parseConstant已经创建了正确的Value类型
    return constExpr->value;
}

// 类型转换表达式求值

// 声明求值 - 语句类型，不需要返回值
void Interpreter::visit(VariableDeclaration* decl) {
    if (!decl) return;
    
    LOG_DEBUG("VariableDeclaration: processing " + to_string(decl->variables.size()) + " variables");
    
    // 处理多个变量声明
    for (size_t i = 0; i < decl->variables.size(); ++i) {
        const auto& var = decl->variables[i];
        string name = var.first;
        Expression* initialValue = var.second;
        Value* value = nullptr;
        
        LOG_DEBUG("VariableDeclaration: processing variable " + to_string(i) + ": " + name);
        
        if (initialValue) {
            LOG_DEBUG("VariableDeclaration: evaluating initial value for " + name);
            value = visit(initialValue);
            LOG_DEBUG("VariableDeclaration: initial value result: " + string(value ? "valid" : "null"));
        }
        
        // 如果变量没有初始值，设置为默认值（数字0）
        if (!value) {
            LOG_DEBUG("VariableDeclaration: creating default value for " + name);
            value = new Integer(0);
        }
        
        // 使用Variable类型存储
        LOG_DEBUG("VariableDeclaration: defining variable " + name);
        scopeManager.defineVariable(name, value);
        
        LOG_DEBUG(std::string("Declaring variable '") + name + "' with value " + (value == nullptr ? "null" : value->toString()));
    }
}

// 变量引用表达式求值 - 查询作用域中的变量定义
Value* Interpreter::visit(VariableExpression* varExpr) {
    if (!varExpr) {
        reportError("Null variable expression");
        return nullptr;
    }
    
    string name = varExpr->name;
    Variable* variable = scopeManager.lookupVariable(name);
    
    if (!variable) {
        reportError("Undefined variable: " + name);
        return nullptr;
    }
    
    Value* value = variable->getValue();
    LOG_DEBUG("VariableExpression: looking up variable '" + name + "', value pointer: " + (value ? "valid" : "null"));
    
    if (!value) {
        reportError("Variable has no value: " + name);
        return nullptr;
    }

    LOG_DEBUG("VariableExpression: about to call toString() on value");
    
    // 添加额外的安全检查
    if (value == nullptr) {
        reportError("Value became null after check: " + name);
        return nullptr;
    }
    
    try {
        string valueStr = value->toString();
        LOG_DEBUG("VariableExpression: " + name + " = " + valueStr);
        return value;
    } catch (...) {
        reportError("Exception in toString() for variable: " + name);
        return nullptr;
    }
}

// 一元表达式求值
Value* Interpreter::visit(UnaryExpression* unary) {
    if (!unary || !unary->operand || !unary->operator_) {
        reportError("Invalid unary expression");
        return nullptr;
    }
    
    // 处理操作数
    Value* operand = visit(unary->operand);
    if (!operand) {
        reportError("Invalid operand in unary expression");
        return nullptr;
    }

    return calculate_unary_compatible(operand, unary->operator_->Tag);
}

// 二元运算表达式求值 - 返回Value类型
Value* Interpreter::visit(BinaryExpression* binary) {
    if (!binary || !binary->left || !binary->right || !binary->operator_) {
        reportError("Invalid binary expression");
        return nullptr;
    }
    
    // 1. 计算左右表达式的值
    Value* left = visit(binary->left);
    Value* right = visit(binary->right);
    
    if (!left || !right) {
        reportError("Invalid operands in binary expression");
        return nullptr;
    }
    
    LOG_DEBUG("BinaryExpression left: " + left->toString());
    LOG_DEBUG("BinaryExpression right: " + right->toString());

    // 2. 获取操作符
    Operator* op = binary->operator_;
    if (!op) {
        reportError("Invalid operator in binary expression");
        return nullptr;
    }
    LOG_DEBUG("executeBinaryOperation called with operator: " + op->getSymbol());
 
    // 4. 确定兼容类型
    return calculate_binary_compatible(left, right, op->Tag);
}

// 赋值表达式求值 - 专门处理赋值操作
Value* Interpreter::visit(AssignExpression* assign) {
    if (!assign || !assign->left || !assign->right) {
        reportError("Invalid assignment expression");
        return nullptr;
    }
    
    // 检查左操作数是否为变量引用
    if (VariableExpression* varExpr = dynamic_cast<VariableExpression*>(assign->left)) {
        // 计算右边的值
        Value* rightValue = visit(assign->right);
        if (!rightValue) {
            reportError("Failed to evaluate right side of assignment");
            return nullptr;
        }
        
        // 获取变量的当前类型
        Variable* variable = scopeManager.lookupVariable(varExpr->name);
        // 如果变量未定义，则使用右边的值的类型
        Value* leftValue = nullptr;
        
        if (variable && variable->getValue()) {
            // 如果变量已定义，使用左边变量的类型
            leftValue = variable->getValue();
        } else {
            // 如果变量未定义，使用右边值的类型
            leftValue = rightValue;
        }
        
        Value* castedValue = calculate_assign_compatible(leftValue, rightValue, '=');
        if (!castedValue) {
            reportError("Failed to cast value to type " + leftValue->getTypeName() + " for assignment");
            delete rightValue;
            return nullptr;
        }
        
        // 更新变量值
        scopeManager.updateVariable(varExpr->name, castedValue);
        LOG_DEBUG("Assigned value " + castedValue->toString() + " to variable '" + varExpr->name + "'");
        return castedValue;  // 赋值表达式返回转换后的值
    } else {
        reportError("Left side of assignment must be a variable");
        return nullptr;
    }
}

// 访问表达式求值 - 统一处理数组/字典访问和成员访问
Value* Interpreter::visit(AccessExpression* access) {
    if (!access || !access->target || !access->key) return nullptr;
    
    // 求值目标
    Value* target = visit(access->target);
    if (!target) return nullptr;
    
    // 求值键
    Value* key = visit(access->key);
    if (!key) return nullptr;
    
    // 检查键是否是字符串常量（成员访问）
    if (String* strKey = dynamic_cast<String*>(key)) {
        string memberName = strKey->getValue();
        
        // 检查对象是否是结构体实例（Dict）
        if (Dict* dict = dynamic_cast<Dict*>(target)) {
            Value* member = dict->getEntry(memberName);
            if (member) {
                return member;
            } else {
                reportError("Member '" + memberName + "' not found in struct instance");
                return nullptr;
            }
        } else {
            reportError("Cannot access member '" + memberName + "' on non-struct object");
            return nullptr;
        }
    } else {
        // 数组/字典访问：使用表达式作为键
        // 检查目标是否是数组或字典
        if (Array* array = dynamic_cast<Array*>(target)) {
            if (Integer* index = dynamic_cast<Integer*>(key)) {
                int idx = index->getValue();
                if (idx >= 0 && idx < (int)array->size()) {
                    return array->getElement(idx);
                } else {
                    reportError("Array index out of bounds: " + to_string(idx));
                    return nullptr;
                }
            } else {
                reportError("Array index must be an integer");
                return nullptr;
            }
        } else if (Dict* dict = dynamic_cast<Dict*>(target)) {
            return dict->access(key);
        } else {
            reportError("Cannot access non-array/non-dict object with key");
            return nullptr;
        }
    }
}

// 函数调用表达式求值
Value* Interpreter::visit(CallExpression* call) {
    if (!call) return nullptr;
    
    // 求值所有参数
    vector<Value*> evaluatedArgs;
    for (Expression* arg : call->arguments) {
        Value* evaluatedArg = visit(arg);
        evaluatedArgs.push_back(evaluatedArg); // 即使为nullptr也要添加，保持索引对应关系
    }
    
    // 使用函数名调用
    string funcName = call->functionName;
    
    // 查找标识符（可能是函数、类或结构体）
    Identifier* identifier = scopeManager.lookupIdentifier(funcName);
    if (!identifier) {
        throw RuntimeException("Identifier not found: " + funcName);
    }
    
    // 检查是否为类或结构体定义（实例化）
    ClassDefinition* classDef = dynamic_cast<ClassDefinition*>(identifier);
    if (classDef) {
        LOG_DEBUG("Instantiating class '" + funcName + "' with " + to_string(evaluatedArgs.size()) + " arguments");
        return instantiateClass(classDef, evaluatedArgs);
    }
    
    StructDefinition* structDef = dynamic_cast<StructDefinition*>(identifier);
    if (structDef) {
        LOG_DEBUG("Instantiating struct '" + funcName + "' with " + to_string(evaluatedArgs.size()) + " arguments");
        return instantiateStruct(structDef, evaluatedArgs);
    }
    
    // 尝试转换为函数定义
    FunctionDefinition* funcDef = dynamic_cast<FunctionDefinition*>(identifier);
    if (!funcDef) {
        throw RuntimeException("Identifier '" + funcName + "' is not a function, class, or struct");
    }
    
    // 统一处理函数调用
    if (funcDef->isBuiltin()) {
        // 内置函数调用
        LOG_DEBUG("Calling builtin function '" + funcName + "' with " + to_string(evaluatedArgs.size()) + " arguments");
        
        // 将参数转换为Variable*，支持引用参数
        vector<Variable*> args;
        for (size_t i = 0; i < call->arguments.size(); ++i) {
            Expression* argExpr = call->arguments[i];
            Value* val = evaluatedArgs[i];
            
            // 检查是否为变量引用（用于 cin(number) 的情况）
            if (VariableExpression* varExpr = dynamic_cast<VariableExpression*>(argExpr)) {
                // 查找原始变量
                Identifier* originalVar = scopeManager.lookupIdentifier(varExpr->name);
                if (Variable* originalVariable = dynamic_cast<Variable*>(originalVar)) {
                    // 使用原始变量的引用
                    args.push_back(originalVariable);
                } else {
                    // 如果找不到原始变量，创建临时变量
                    Type* argType = val ? val->valueType : nullptr;
                    args.push_back(new Variable("", argType, val));
                }
            } else {
                // 非变量引用，创建临时变量
                Type* argType = val ? val->valueType : nullptr;
                args.push_back(new Variable("", argType, val));
            }
        }
        
        // 统一调用函数
        Value* result = funcDef->execute(args);
        
        // 清理临时变量（只清理非引用的变量）
        for (size_t i = 0; i < args.size(); ++i) {
            Expression* argExpr = call->arguments[i];
            if (!dynamic_cast<VariableExpression*>(argExpr)) {
                delete args[i];
            }
        }
        
        return result;
    } else {
        // 用户函数调用
        LOG_DEBUG("Calling user function '" + funcName + "' with " + to_string(evaluatedArgs.size()) + " arguments");
        
        // 进入新的作用域并执行函数体
        return withScope([&]() -> Value* {
            // 绑定参数到局部变量
            const vector<pair<string, Type*>>& params = funcDef->prototype->parameters;
            for (size_t i = 0; i < params.size() && i < evaluatedArgs.size(); ++i) {
                scopeManager.defineVariable(params[i].first, evaluatedArgs[i]);
                LOG_DEBUG("Bound parameter '" + params[i].first + "'");
            }
            
            // 执行函数体
            Value* result = nullptr;
            try {
                execute(funcDef->body);
            } catch (const ReturnException& e) {
                result = static_cast<Value*>(e.getValue());
            }
            
            return result;
        });
    }
}

Value* Interpreter::visit(MethodCallExpression* methodCall) {
    if (!methodCall) return nullptr;
    
    // 获取对象
    Value* object = visit(methodCall->object);
    if (!object) {
        reportError("Cannot call method on null object");
        return nullptr;
    }
    
    // 获取方法名
    string methodName = methodCall->methodName;
    
    // 获取参数
    vector<Value*> args;
    for (Expression* argExpr : methodCall->arguments) {
        Value* arg = visit(argExpr);
        if (arg) {
            args.push_back(arg);
        }
    }
    
    // 检查对象类型并调用相应的方法
    if (Dict* dict = dynamic_cast<Dict*>(object)) {
        // 对于字典对象，查找方法
        Value* method = dict->getEntry(methodName);
        if (method) {
            // 如果找到方法，调用它
            if (FunctionDefinition* func = dynamic_cast<FunctionDefinition*>(method)) {
                // 将对象作为第一个参数传递（类似this指针）
                vector<Value*> methodArgs;
                methodArgs.push_back(object);
                methodArgs.insert(methodArgs.end(), args.begin(), args.end());
                
                return func->executeWithInterpreter(methodArgs, this);
            } else {
                // 如果不是方法，返回该值
                return method;
            }
        } else {
            reportError("Method '" + methodName + "' not found in object");
            return nullptr;
        }
    } else {
        // 对于其他类型的对象，可以添加内置方法支持
        reportError("Method calls not supported for this object type");
        return nullptr;
    }
}
