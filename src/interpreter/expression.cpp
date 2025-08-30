
#include "interpreter/interpreter.h"
#include "parser/expression.h"
#include "parser/function.h"
#include "parser/inter.h"
#include "lexer/token.h"
#include "interpreter/logger.h"
#include "interpreter/value.h"

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
    if (ConstantExpression<int>* intExpr = dynamic_cast<ConstantExpression<int>*>(expr)) {
        return visit(intExpr);
    } else if (ConstantExpression<double>* doubleExpr = dynamic_cast<ConstantExpression<double>*>(expr)) {
        return visit(doubleExpr);
    } else if (ConstantExpression<bool>* boolExpr = dynamic_cast<ConstantExpression<bool>*>(expr)) {
        return visit(boolExpr);
    } else if (ConstantExpression<char>* charExpr = dynamic_cast<ConstantExpression<char>*>(expr)) {
        return visit(charExpr);
    } else if (ConstantExpression<string>* stringExpr = dynamic_cast<ConstantExpression<string>*>(expr)) {
        return visit(stringExpr);
    } else if (VariableExpression* varExpr = dynamic_cast<VariableExpression*>(expr)) {
        return visit(varExpr);
    } else if (UnaryExpression* unaryExpr = dynamic_cast<UnaryExpression*>(expr)) {
        return visit(unaryExpr);
    } else if (AssignExpression* assignExpr = dynamic_cast<AssignExpression*>(expr)) {
        return visit(assignExpr);
    } else if (BinaryExpression* binaryExpr = dynamic_cast<BinaryExpression*>(expr)) {
        return visit(binaryExpr);
    } else if (CastExpression* castExpr = dynamic_cast<CastExpression*>(expr)) {
        return visit(castExpr);
    } else if (AccessExpression* accessExpr = dynamic_cast<AccessExpression*>(expr)) {
        return visit(accessExpr);
    } else if (CallExpression* callExpr = dynamic_cast<CallExpression*>(expr)) {
        return visit(callExpr);
    } else if (MethodCallExpression* methodCallExpr = dynamic_cast<MethodCallExpression*>(expr)) {
        return visit(methodCallExpr);
    }
    
    reportError("Unknown expression type");
    return nullptr;
}

// ==================== 常量表达式访问方法实现 ====================

// 整数常量表达式求值
Value* Interpreter::visit(ConstantExpression<int>* expr) {
    if (!expr) {
        reportError("Null integer constant expression");
        return nullptr;
    }
    
    LOG_DEBUG("Evaluating integer constant: " + to_string(expr->value));
    return new Integer(expr->value);
}

// 浮点数常量表达式求值
Value* Interpreter::visit(ConstantExpression<double>* expr) {
    if (!expr) {
        reportError("Null double constant expression");
        return nullptr;
    }
    
    LOG_DEBUG("Evaluating double constant: " + to_string(expr->value));
    return new Double(expr->value);
}

// 布尔常量表达式求值
Value* Interpreter::visit(ConstantExpression<bool>* expr) {
    if (!expr) {
        reportError("Null boolean constant expression");
        return nullptr;
    }
    
    LOG_DEBUG("Evaluating boolean constant: " + string(expr->value ? "true" : "false"));
    return new Bool(expr->value);
}

// 字符常量表达式求值
Value* Interpreter::visit(ConstantExpression<char>* expr) {
    if (!expr) {
        reportError("Null character constant expression");
        return nullptr;
    }
    
    LOG_DEBUG("Evaluating character constant: " + string(1, expr->value));
    return new Char(expr->value);
}

// 字符串常量表达式求值
Value* Interpreter::visit(ConstantExpression<string>* expr) {
    if (!expr) {
        reportError("Null string constant expression");
        return nullptr;
    }
    
    LOG_DEBUG("Evaluating string constant: " + expr->value);
    return new String(expr->value);
}

// 类型转换表达式求值

// 声明求值 - 语句类型，不需要返回值
void Interpreter::visit(VariableDeclaration* decl) {
    if (!decl) return;
    
    LOG_DEBUG("Interpreter::visit(VariableDeclaration): processing variable declarations");
    
    // 处理每个变量声明
    for (const auto& varPair : decl->variables) {
        string varName = varPair.first;
        Expression* initExpr = varPair.second;
        
        LOG_DEBUG("Processing variable declaration: " + varName);
        
        // 创建变量值
        Value* varValue = nullptr;
        
        if (initExpr) {
            // 如果有初始化表达式，求值它
            varValue = visit(initExpr);
            if (!varValue) {
                LOG_ERROR("Failed to evaluate initialization expression for variable: " + varName);
                continue;
            }
        } else {
            // 如果没有初始化表达式，使用ObjectFactory创建默认值
            // 这里我们假设所有变量都是auto类型，或者可以从初始化表达式推断类型
            varValue = ObjectFactory::createDefaultValueStatic("auto");
        }
        
        // 在作用域中定义变量
        scopeManager.defineVariable(varName, varValue);
        
        LOG_DEBUG("Successfully declared variable: " + varName + " with value: " + varValue->toString());
    }
}

// 变量引用表达式求值 - 使用作用域管理器
Value* Interpreter::visit(VariableExpression* varExpr) {
    if (!varExpr) {
        reportError("Null variable expression");
        return nullptr;
    }
    
    string name = varExpr->name;
    
    // 从作用域中查找变量
    RuntimeVariable* variable = scopeManager.lookupVariable(name);
    
    if (!variable) {
        reportError("Undefined variable: " + name);
        return nullptr;
    }
    
    Value* value = variable->getValue();
    if (!value) {
        reportError("Variable has no value: " + name);
        return nullptr;
    }
    
    LOG_DEBUG("VariableExpression: " + name + " = " + value->toString());
    return value;
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
    if (!dynamic_cast<VariableExpression*>(assign->left)) {
        reportError("Left side of assignment must be a variable");
        return nullptr;
    }
    
    VariableExpression* varExpr = static_cast<VariableExpression*>(assign->left);
    
    // 计算右边的值
    Value* rightValue = visit(assign->right);
    if (!rightValue) {
        reportError("Failed to evaluate right side of assignment");
        return nullptr;
    }
    
    // 获取变量的当前类型
    RuntimeVariable* variable = scopeManager.lookupVariable(varExpr->name);
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
}

// 访问表达式求值 - 通过类型系统统一处理
Value* Interpreter::visit(AccessExpression* access) {
    if (!access || !access->target || !access->key) return nullptr;
    
    // 求值目标
    Value* target = visit(access->target);
    if (!target) return nullptr;
    
    // 求值键
    Value* key = visit(access->key);
    if (!key) return nullptr;
    
    // 通过类型系统处理访问操作
            ObjectType* targetType = target->getValueType();
    if (!targetType) {
        reportError("Cannot access object without runtime type");
        return nullptr;
    }
    
    // 检查类型是否支持访问操作
    if (!targetType->hasMethod("access")) {
        reportError("Type '" + targetType->getTypeName() + "' does not support access operations");
        return nullptr;
    }
    
    // 调用类型系统的访问方法
    vector<Value*> args = {key};
    return targetType->callMethod(target, "access", args);
}

// 函数调用表达式求值 - 通过类型系统统一处理
Value* Interpreter::visit(CallExpression* call) {
    if (!call) return nullptr;
    
    // 求值所有参数
    vector<Value*> evaluatedArgs;
    for (Expression* arg : call->arguments) {
        Value* value = visit(arg);
        if (value) {
            evaluatedArgs.push_back(value);
        } else {
            reportError("Failed to evaluate argument in function call");
            return nullptr;
        }
    }
    

    
    // 普通函数调用
    string funcName = call->functionName;
    
    // 查找运行时函数
    RuntimeFunction* runtimeFunc = scopeManager.lookupFunction(funcName);
    if (!runtimeFunc) {
        reportError("Function not found: " + funcName);
        return nullptr;
    }
    
    // 执行运行时函数调用
    Value* result = runtimeFunc->call(evaluatedArgs);
    if (!result) {
        reportError("Function call failed: " + funcName);
        return nullptr;
    }
    
    return result;
}

// ==================== MethodCallExpression访问方法 ====================

// 方法调用表达式求值 - 专门处理方法调用
Value* Interpreter::visit(MethodCallExpression* methodCall) {
    if (!methodCall) return nullptr;
    
    // 求值对象
    Value* object = visit(methodCall->object);
    if (!object) {
        reportError("Failed to evaluate method call object");
        return nullptr;
    }
    
    // 求值方法参数
    vector<Value*> methodArgs;
    for (Expression* arg : methodCall->arguments) {
        Value* value = visit(arg);
        if (value) {
            methodArgs.push_back(value);
        } else {
            reportError("Failed to evaluate method argument");
            return nullptr;
        }
    }
    
    // 获取对象的运行时类型
            ObjectType* objectType = object->getValueType();
    if (!objectType) {
        reportError("Method call object has no runtime type");
        return nullptr;
    }
    
    // 检查方法是否存在
    if (!objectType->hasMethod(methodCall->functionName)) {
        reportError("Method '" + methodCall->functionName + "' not found in type '" + objectType->getTypeName() + "'");
        return nullptr;
    }
    
    // 执行方法调用
    Value* result = objectType->callMethod(object, methodCall->functionName, methodArgs);
    if (!result) {
        reportError("Method '" + methodCall->functionName + "' call failed on type '" + objectType->getTypeName() + "'");
        return nullptr;
    }
    
    return result;
}



