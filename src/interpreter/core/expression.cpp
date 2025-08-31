
#include "interpreter/interpreter.h"
#include "interpreter/calculate.h"
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
    } else if (IncrementDecrementExpression* incDecExpr = dynamic_cast<IncrementDecrementExpression*>(expr)) {
        // IncrementDecrementExpression现在继承自UnaryExpression，可以利用基类特性
        return visit(incDecExpr);
    } else if (MemberAssignExpression* memberAssignExpr = dynamic_cast<MemberAssignExpression*>(expr)) {
        return visit(memberAssignExpr);
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
Value* Interpreter::visit(CastExpression* castExpr) {
    if (!castExpr) return nullptr;
    
    // 求值操作数
    Value* operandValue = visit(castExpr->operand);
    if (!operandValue) return nullptr;
    
    // 使用Calculator的智能类型转换
    Value* result = Calculator::smartTypeConversion(
        operandValue, 
        castExpr->getTargetTypeName()
    );
    
    if (!result) {
        reportError("Type conversion failed from " + getValueTypeName(operandValue) + 
                   " to " + castExpr->getTargetTypeName());
        return nullptr;
    }
    
    return result;
}

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

    return Calculator::executeUnaryOperation(operand, unary->operator_->Tag);
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
 
    // 4. 使用Calculator进行二元运算
    return Calculator::executeBinaryOperation(left, right, op->Tag);
}

// 赋值表达式求值 - 支持变量赋值和一般表达式赋值
Value* Interpreter::visit(AssignExpression* assign) {
    if (!assign || !assign->left || !assign->right) {
        reportError("Invalid assignment expression");
        return nullptr;
    }
    
    // 计算右边的值
    Value* rightValue = visit(assign->right);
    if (!rightValue) {
        reportError("Failed to evaluate right side of assignment");
        return nullptr;
    }
    
    // 检查左操作数的类型并处理赋值
    if (VariableExpression* varExpr = dynamic_cast<VariableExpression*>(assign->left)) {
        // 变量赋值 - 先转换类型再赋值
        return handleVariableAssignment(varExpr, rightValue);
    } else if (AccessExpression* accessExpr = dynamic_cast<AccessExpression*>(assign->left)) {
        // 成员访问或数组访问赋值 - 先转换类型再赋值
        return handleMemberAssignment(accessExpr, rightValue);
    } else {
        // 其他表达式类型，先转换类型再赋值
        return handleGeneralAssignment(assign->left, rightValue);
    }
}

// 成员赋值表达式求值 - 处理成员赋值操作
Value* Interpreter::visit(MemberAssignExpression* memberAssign) {
    if (!memberAssign || !memberAssign->target || !memberAssign->member || !memberAssign->value) {
        reportError("Invalid member assignment expression");
        return nullptr;
    }
    
    // 求值目标对象
    Value* target = visit(memberAssign->target);
    if (!target) {
        reportError("Failed to evaluate target object in member assignment");
        return nullptr;
    }
    
    // 求值成员名称或索引
    Value* member = visit(memberAssign->member);
    if (!member) {
        reportError("Failed to evaluate member name or index in member assignment");
        delete target;
        return nullptr;
    }
    
    // 求值要赋值的值
    Value* value = visit(memberAssign->value);
    if (!value) {
        reportError("Failed to evaluate value in member assignment");
        delete target;
        delete member;
        return nullptr;
    }
    
    // 通过类型系统处理成员赋值
    ObjectType* targetType = target->getValueType();
    if (!targetType) {
        reportError("Cannot assign to member of object without runtime type");
        delete target;
        delete member;
        delete value;
        return nullptr;
    }
    
    // 成员赋值可以通过成员访问和赋值运算符的组合来实现
    // 首先获取成员引用，然后进行赋值操作
    
    // 检查类型是否支持成员访问操作
    if (!targetType->hasMethod("member_access")) {
        reportError("Type '" + targetType->getTypeName() + "' does not support member access");
        delete target;
        delete member;
        delete value;
        return nullptr;
    }
    
    // 通过成员访问获取成员引用，然后进行赋值
    // 这里需要实现成员引用的逻辑，暂时使用直接赋值
    // TODO: 实现成员引用的完整逻辑
    reportError("Member assignment not yet implemented - requires member reference support");
    delete target;
    delete member;
    delete value;
    return nullptr;
    
    if (!result) {
        reportError("Failed to assign value to member of type '" + targetType->getTypeName() + "'");
    }
    
    // 清理临时值
    delete target;
    delete member;
    delete value;
    
    return result;
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
    for (size_t i = 0; i < call->arguments.size(); ++i) {
        Expression* arg = call->arguments[i];
        Value* value = visit(arg);
        if (value) {
            evaluatedArgs.push_back(value);
        } else {
            reportError("Failed to evaluate argument " + to_string(i + 1) + " in function call to " + call->functionName);
            return nullptr;
        }
    }
    
    // 特殊处理内置函数
    string funcName = call->functionName;
    
    // 处理数组字面量
    if (funcName == "new_array") {
        Array* newArray = new Array();
        for (Value* arg : evaluatedArgs) {
            newArray->addElement(arg);
        }
        return newArray;
    }
    
    // 处理字典字面量
    if (funcName == "new_dict") {
        Dict* newDict = new Dict();
        // 字典字面量参数是键值对交替出现
        for (size_t i = 0; i < evaluatedArgs.size(); i += 2) {
            if (i + 1 < evaluatedArgs.size()) {
                if (String* key = dynamic_cast<String*>(evaluatedArgs[i])) {
                    newDict->setEntry(key->getValue(), evaluatedArgs[i + 1]);
                } else {
                    reportError("Dictionary key must be a string");
                    delete newDict;
                    return nullptr;
                }
            }
        }
        return newDict;
    }
    
    // 普通函数调用
    RuntimeFunction* runtimeFunc = scopeManager.lookupFunction(funcName);
    if (!runtimeFunc) {
        reportError("Function not found: " + funcName);
        return nullptr;
    }
    
    // 执行运行时函数调用
    Value* result = runtimeFunc->call(evaluatedArgs);
    // 对于像println这样的函数，返回nullptr是正常的，不应该被视为错误
    // 只有在函数调用过程中出现异常时才应该报告错误
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

// ==================== IncrementDecrementExpression访问方法 ====================

// 自增自减表达式求值 - 继承自UnaryExpression，利用基类特性
Value* Interpreter::visit(IncrementDecrementExpression* expr) {
    if (!expr) return nullptr;
    
    // 利用基类UnaryExpression的operand字段
    Expression* operand = expr->operand;
    if (!operand) {
        reportError("Increment/decrement expression has no operand");
        return nullptr;
    }
    
    // 求值操作数
    Value* operandValue = visit(operand);
    if (!operandValue) {
        reportError("Failed to evaluate increment/decrement operand");
        return nullptr;
    }
    
    // 检查操作数是否为变量表达式（只有变量才能被修改）
    VariableExpression* varExpr = dynamic_cast<VariableExpression*>(operand);
    if (!varExpr) {
        reportError("Increment/decrement operand must be a variable");
        return nullptr;
    }
    
    string varName = varExpr->name;
    
    // 获取变量的当前值
    RuntimeVariable* runtimeVar = scopeManager.lookupVariable(varName);
    if (!runtimeVar) {
        reportError("Variable '" + varName + "' not found for increment/decrement");
        return nullptr;
    }
    Value* currentValue = runtimeVar->getValue();
    if (!currentValue) {
        reportError("Variable '" + varName + "' has no value for increment/decrement");
        return nullptr;
    }
    
    // 计算新值
    Value* newValue = nullptr;
    if (expr->isIncrement()) {
        // 自增操作
        if (Integer* intVal = dynamic_cast<Integer*>(currentValue)) {
            newValue = new Integer(intVal->getValue() + 1);
        } else if (Double* doubleVal = dynamic_cast<Double*>(currentValue)) {
            newValue = new Double(doubleVal->getValue() + 1.0);
        } else {
            reportError("Cannot increment non-numeric value");
            return nullptr;
        }
    } else {
        // 自减操作
        if (Integer* intVal = dynamic_cast<Integer*>(currentValue)) {
            newValue = new Integer(intVal->getValue() - 1);
        } else if (Double* doubleVal = dynamic_cast<Double*>(currentValue)) {
            newValue = new Double(doubleVal->getValue() - 1.0);
        } else {
            reportError("Cannot increment non-numeric value");
            return nullptr;
        }
    }
    
    // 根据前缀/后缀返回不同的值
    Value* result = nullptr;
    if (expr->isPrefixOperation()) {
        // 前缀操作：先更新变量值，然后返回新值
        scopeManager.updateVariable(varName, newValue);
        result = newValue->clone();
    } else {
        // 后缀操作：先保存旧值，然后更新变量值
        result = currentValue->clone();
        scopeManager.updateVariable(varName, newValue);
    }
    
    return result;
}

// ==================== 赋值辅助方法 ====================

// 处理变量赋值
Value* Interpreter::handleVariableAssignment(VariableExpression* varExpr, Value* rightValue) {
    if (!varExpr || !rightValue) {
        reportError("Invalid variable assignment");
        return nullptr;
    }
    
    // 获取变量的当前类型
    RuntimeVariable* variable = scopeManager.lookupVariable(varExpr->name);
    Value* leftValue = nullptr;
    
    if (variable && variable->getValue()) {
        // 如果变量已定义，使用其当前类型
        leftValue = variable->getValue();
    } else {
        // 如果变量未定义，创建新变量
        leftValue = rightValue->clone();
        scopeManager.defineVariable(varExpr->name, leftValue);
        return rightValue;
    }
    
    // 先进行类型转换，再进行赋值
    if (leftValue->getValueType() && leftValue->getValueType()->hasMethod("convertTo")) {
        string targetTypeName = leftValue->getTypeName();
        if (targetTypeName != rightValue->getTypeName()) {
            // 尝试将右值转换为左值的类型
            vector<Value*> convertArgs = {new String(targetTypeName)};
            Value* convertedValue = rightValue->getValueType()->callMethod(rightValue, "convertTo", convertArgs);
            if (convertedValue) {
                // 转换成功，进行赋值
                if (leftValue->getValueType()->hasMethod("assign")) {
                    vector<Value*> assignArgs = {convertedValue};
                    Value* result = leftValue->getValueType()->callMethod(leftValue, "assign", assignArgs);
                    if (result) {
                        // 更新变量值
                        scopeManager.updateVariable(varExpr->name, result);
                        LOG_DEBUG("Successfully converted and assigned value through type system");
                        delete convertedValue;
                        return result;
                    }
                }
                // 如果assign方法失败，直接更新变量值
                scopeManager.updateVariable(varExpr->name, convertedValue);
                LOG_DEBUG("Successfully converted value and updated variable through type system");
                return convertedValue;
            }
        }
    }
    
    // 如果类型转换失败，尝试直接赋值
    if (leftValue->getValueType() && leftValue->getValueType()->hasMethod("assign")) {
        vector<Value*> assignArgs = {rightValue};
        Value* result = leftValue->getValueType()->callMethod(leftValue, "assign", assignArgs);
        if (result) {
            // 更新变量值
            scopeManager.updateVariable(varExpr->name, result);
            LOG_DEBUG("Successfully assigned value through type system assign method");
            return result;
        }
    }
    
    // 如果类型系统方法都失败，回退到Calculator的赋值操作
    Value* castedValue = Calculator::executeAssignmentOperation(leftValue, rightValue, '=');
    if (!castedValue) {
        reportError("Failed to cast value to type " + leftValue->getTypeName() + " for assignment");
        return nullptr;
    }
    
    // 更新变量值
    scopeManager.updateVariable(varExpr->name, castedValue);
    LOG_DEBUG("Assigned value " + castedValue->toString() + " to variable " + varExpr->name);
    return castedValue;
}

// 处理成员赋值
Value* Interpreter::handleMemberAssignment(AccessExpression* accessExpr, Value* rightValue) {
    if (!accessExpr || !rightValue) {
        reportError("Invalid member assignment");
        return nullptr;
    }
    
    // 求值目标对象
    Value* target = visit(accessExpr->target);
    if (!target) {
        reportError("Failed to evaluate target object in member assignment");
        return nullptr;
    }
    
    // 求值成员名称或索引
    Value* member = visit(accessExpr->key);
    if (!member) {
        reportError("Failed to evaluate member name or index in member assignment");
        delete target;
        return nullptr;
    }
    
    // 通过类型系统处理成员赋值
    ObjectType* targetType = target->getValueType();
    if (!targetType) {
        reportError("Cannot assign to member of object without runtime type");
        delete target;
        delete member;
        return nullptr;
    }
    
    // 成员赋值可以通过成员访问和赋值运算符的组合来实现
    // 首先获取成员引用，然后进行赋值操作
    
    // 检查类型是否支持成员访问操作
    if (!targetType->hasMethod("member_access")) {
        reportError("Type '" + targetType->getTypeName() + "' does not support member access");
        delete target;
        delete member;
        return nullptr;
    }
    
    // 通过成员访问获取成员引用，然后进行赋值
    // 这里需要实现成员引用的逻辑，暂时使用直接赋值
    // TODO: 实现成员引用的完整逻辑
    reportError("Member assignment not yet implemented - requires member reference support");
    delete target;
    delete member;
    return nullptr;
    
    // 成员赋值功能已暂时禁用，等待成员引用支持
    return nullptr;
}

// 处理一般表达式赋值
Value* Interpreter::handleGeneralAssignment(Expression* leftExpr, Value* rightValue) {
    if (!leftExpr || !rightValue) {
        reportError("Invalid general assignment");
        return nullptr;
    }
    
    // 尝试对左边表达式求值
    Value* leftValue = visit(leftExpr);
    if (!leftValue) {
        reportError("Failed to evaluate left side of assignment");
        return nullptr;
    }
    
    // 先进行类型转换，再进行赋值
    ObjectType* leftType = leftValue->getValueType();
    if (leftType && leftType->hasMethod("convertTo")) {
        string targetTypeName = leftValue->getTypeName();
        if (targetTypeName != rightValue->getTypeName()) {
            // 尝试将右值转换为左值的类型
            vector<Value*> convertArgs = {new String(targetTypeName)};
            Value* convertedValue = rightValue->getValueType()->callMethod(rightValue, "convertTo", convertArgs);
            if (convertedValue) {
                // 转换成功，进行赋值
                if (leftType->hasMethod("assign")) {
                    vector<Value*> assignArgs = {convertedValue};
                    Value* result = leftType->callMethod(leftValue, "assign", assignArgs);
                    if (result) {
                        LOG_DEBUG("Successfully converted and assigned value through type system");
                        delete convertedValue;
                        return result;
                    }
                }
                // 如果assign方法失败，返回转换后的值
                LOG_DEBUG("Successfully converted value through type system");
                return convertedValue;
            }
        }
    }
    
    // 如果类型转换失败或不需要转换，尝试直接赋值
    if (leftType && leftType->hasMethod("assign")) {
        vector<Value*> args = {rightValue};
        Value* result = leftType->callMethod(leftValue, "assign", args);
        if (result) {
            LOG_DEBUG("Successfully assigned value through type system assign method");
            return result;
        }
    }
    
    // 如果不支持赋值，报告错误
    reportError("Left side of assignment does not support assignment operation");
    return nullptr;
}
