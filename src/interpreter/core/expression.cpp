// ==================== 标准库头文件 ====================
#include <iostream>
#include <sstream>
#include <typeinfo>
#include <map>
#include <functional>

// ==================== 项目头文件 ====================
// 1. 当前源文件对应的头文件（如果有的话）
// 2. 项目核心头文件
#include "interpreter/core/interpreter.h"
#include "interpreter/values/value.h"
#include "interpreter/types/types.h"

// 3. 项目功能头文件
#include "interpreter/values/calculate.h"
#include "interpreter/core/function_call.h"
#include "interpreter/utils/logger.h"

// 4. 解析器头文件
#include "parser/expression.h"
#include "parser/definition.h"
#include "parser/inter.h"
#include "parser/operator_mapping.h"

// 5. 词法分析器头文件
#include "lexer/token.h"

// 注意：已移除 using namespace std;，使用显式std前缀

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
    } else if (ConstantExpression<std::string>* stringExpr = dynamic_cast<ConstantExpression<std::string>*>(expr)) {
        return visit(stringExpr);
    } else if (VariableExpression* varExpr = dynamic_cast<VariableExpression*>(expr)) {
        return visit(varExpr);
    } else if (UnaryExpression* unaryExpr = dynamic_cast<UnaryExpression*>(expr)) {
        return visit(unaryExpr);
    } else if (BinaryExpression* binaryExpr = dynamic_cast<BinaryExpression*>(expr)) {
        return visit(binaryExpr);
    } else if (CastExpression* castExpr = dynamic_cast<CastExpression*>(expr)) {
        return visit(castExpr);
    } else if (AccessExpression* accessExpr = dynamic_cast<AccessExpression*>(expr)) {
        return visit(accessExpr);
    } else if (CallExpression* callExpr = dynamic_cast<CallExpression*>(expr)) {
        return visit(callExpr);
    // IncDecExpression已合并到UnaryExpression中，不需要单独处理
    } else if (TernaryExpression* ternaryExpr = dynamic_cast<TernaryExpression*>(expr)) {
        return visit(ternaryExpr);
    }
    
    Interpreter::reportError("Unknown expression type");
    return nullptr;
}

// ==================== 常量表达式访问方法实现 ====================

// 整数常量表达式求值
Value* Interpreter::visit(ConstantExpression<int>* expr) {
    if (!expr) {
        Interpreter::reportError("Null integer constant expression");
        return nullptr;
    }
    
    LOG_DEBUG("Evaluating integer constant: " + to_string(expr->value));
    return new Integer(expr->value);
}

// 浮点数常量表达式求值
Value* Interpreter::visit(ConstantExpression<double>* expr) {
    if (!expr) {
        Interpreter::reportError("Null double constant expression");
        return nullptr;
    }
    
    LOG_DEBUG("Evaluating double constant: " + to_string(expr->value));
    return new Double(expr->value);
}

// 布尔常量表达式求值
Value* Interpreter::visit(ConstantExpression<bool>* expr) {
    if (!expr) {
        Interpreter::reportError("Null boolean constant expression");
        return nullptr;
    }
    
            LOG_DEBUG("Evaluating boolean constant: " + std::string(expr->value ? "true" : "false"));
    return new Bool(expr->value);
}

// 字符常量表达式求值
Value* Interpreter::visit(ConstantExpression<char>* expr) {
    if (!expr) {
        Interpreter::reportError("Null character constant expression");
        return nullptr;
    }
    
    LOG_DEBUG("Evaluating character constant: " + std::string(1, expr->value));
    return new Char(expr->value);
}

// 字符串常量表达式求值
Value* Interpreter::visit(ConstantExpression<std::string>* expr) {
    if (!expr) {
        Interpreter::reportError("Null string constant expression");
        return nullptr;
    }
    
    LOG_DEBUG("Evaluating string constant: " + expr->value);
    String* strValue = new String(expr->value);
    
    // 设置字符串的类型
    ObjectType* stringType = TypeRegistry::getGlobalInstance()->getType("string");
    if (stringType) {
        strValue->setValueType(stringType);
        LOG_DEBUG("String constant type set to: " + stringType->getTypeName());
    } else {
        LOG_DEBUG("Could not find string type in registry");
    }
    
    return strValue;
}

// 类型转换表达式求值
Value* Interpreter::visit(CastExpression* castExpr) {
    if (!castExpr) return nullptr;
    
    // 求值操作数
    Value* operandValue = visit(castExpr->operand);
    if (!operandValue) return nullptr;
    
    // 直接进行类型转换
    std::string targetTypeName = castExpr->getTargetTypeName();
    std::string currentTypeName = getValueTypeName(operandValue);
    
    // 如果已经是目标类型，直接克隆
    if (currentTypeName == targetTypeName) {
        return operandValue->clone();
    }
    
    // 使用类型系统进行转换
    ObjectType* valueType = operandValue->getValueType();
    if (!valueType) return nullptr;
    
    // 根据目标类型名称构造转换方法名
    std::string methodName = "to" + targetTypeName;
    // 首字母大写
    if (!methodName.empty() && methodName.length() > 2) {
        methodName[2] = toupper(methodName[2]);
    }
    
    // 检查是否有对应的转换方法
    if (valueType->hasMethod(methodName)) {
        // 调用类型系统注册的转换方法
        std::vector<Value*> convertArgs;
        Value* result = valueType->callMethod(operandValue, methodName, convertArgs);
        if (result) {
            return result;
        }
    }
    
    // 如果转换失败，报告错误
    Interpreter::reportError("Type conversion failed from " + getValueTypeName(operandValue) + 
               " to " + castExpr->getTargetTypeName());
    return nullptr;
}

// 变量引用表达式求值 - 使用新的统一接口
Value* Interpreter::visit(VariableExpression* varExpr) {
    if (!varExpr) {
        Interpreter::reportError("Null variable expression");
        return nullptr;
    }
    
    string name = varExpr->name;
    
    // 使用新的统一接口查找标识符
    Value* value = scopeManager.lookup(name);
    
    if (!value) {
        Interpreter::reportError("Undefined variable: " + name);
        return nullptr;
    }
    
    // 检查变量的类型
    if (!value->getValueType()) {
        // 如果这是一个类型名称对象（没有运行时类型），不要为其设置类型
        if (String* typeNameStr = dynamic_cast<String*>(value)) {
            // 检查是否是类型名称
            ObjectType* type = scopeManager.lookupType(typeNameStr->getValue());
            if (type) {
                return value; // 不设置运行时类型，保持为类型名称对象
            }
        }
        
        // 尝试推断类型
        string typeName = value->getBuiltinTypeName();
        ObjectType* inferredType = TypeRegistry::getGlobalInstance()->getType(typeName);
        if (inferredType) {
            value->setValueType(inferredType);
        }
    }
    
    return value;
}

// 一元表达式求值
Value* Interpreter::visit(UnaryExpression* unary) {
    if (!unary || !unary->operand || !unary->operator_) {
        Interpreter::reportError("Invalid unary expression");
        return nullptr;
    }
    
    // 特殊处理自增自减操作
    if (unary->operator_->Tag == INCREMENT || unary->operator_->Tag == DECREMENT) {
        return handleIncrementDecrement(unary);
    }
    
    // 处理操作数
    Value* operand = visit(unary->operand);
    if (!operand) {
        Interpreter::reportError("Invalid operand in unary expression");
        return nullptr;
    }

    // 使用Calculator进行一元运算
    try {
        // 使用getOperationType()获取标准化的操作类型
        return calculator->executeUnaryOperation(operand, unary->operator_);
    } catch (const std::runtime_error& e) {
        Interpreter::reportError("Unary operation failed: " + std::string(e.what()));
        return nullptr;
    }
}

// 二元运算表达式求值 - 返回Value类型
Value* Interpreter::visit(BinaryExpression* binary) {
    if (!binary || !binary->left || !binary->right || !binary->operator_) {
        Interpreter::reportError("Invalid binary expression");
        return nullptr;
    }
    
    // 1. 计算左右表达式的值
    Value* left = visit(binary->left);
    Value* right = visit(binary->right);
    
    if (!left || !right) {
        Interpreter::reportError("Invalid operands in binary expression");
        return nullptr;
    }
    
    LOG_DEBUG("BinaryExpression left: " + left->toString());
    LOG_DEBUG("BinaryExpression right: " + right->toString());

    // 2. 获取操作符
    Operator* op = binary->operator_;
    if (!op) {
        Interpreter::reportError("Invalid operator in binary expression");
        return nullptr;
    }
    LOG_DEBUG("executeBinaryOperation called with operator: " + op->getSymbol());
 
    // 使用Calculator进行二元运算，自动处理类型转换
    try {
        // 所有二元运算都通过Calculator处理，包括赋值运算
        // 使用getOperationType()获取标准化的操作类型
        return calculator->executeBinaryOperation(left, right, op);
    } catch (const std::runtime_error& e) {
        Interpreter::reportError("Binary operation failed: " + std::string(e.what()));
        return nullptr;
    }
}

// 访问表达式求值 - 直接使用ScopeManager的统一接口
Value* Interpreter::visit(AccessExpression* access) {
    if (!access || !access->target || !access->key) return nullptr;
    
    // 求值目标
    Value* target = visit(access->target);
    if (!target) return nullptr;
    
    // 求值键
    Value* key = visit(access->key);
    if (!key) return nullptr;
    
    // 获取成员名称
    string memberName = extractMemberName(key);
    if (memberName.empty()) return nullptr;
    
    // 情况1: 目标没有运行时类型，尝试作为类型名称处理（静态访问）
    if (!target->getValueType()) {
        // 如果target已经是ClassMethodValue，说明之前已经找到了方法，直接返回
        if (ClassMethodValue* classMethod = dynamic_cast<ClassMethodValue*>(target)) {
            cout << "DEBUG: AccessExpression: target is already a ClassMethodValue, returning it" << endl;
            return classMethod;
        }
        
        // 如果target已经是MethodValue，说明之前已经找到了方法，直接返回
        if (MethodValue* method = dynamic_cast<MethodValue*>(target)) {
            cout << "DEBUG: AccessExpression: target is already a MethodValue, returning it" << endl;
            return method;
        }
        
        if (String* typeNameStr = dynamic_cast<String*>(target)) {
            string typeName = typeNameStr->getValue();
            cout << "DEBUG: AccessExpression: target is String, typeName: '" + typeName + "'" << endl;
            
            // 使用统一接口查找类型方法
            Value* typeMethod = scopeManager.lookupTypeMethod(typeName, memberName);
            if (typeMethod) {
                cout << "DEBUG: AccessExpression: found type method '" + memberName + "' in type '" + typeName + "'" << endl;
                return typeMethod;
            }
            
            // 如果找不到方法，尝试查找类型本身
            ObjectType* type = scopeManager.lookupType(typeName);
            if (type) {
                cout << "DEBUG: AccessExpression: found type '" + typeName + "'" << endl;
                return new String(typeName);
            }
            
            cout << "DEBUG: AccessExpression: no static method/static member '" + memberName + "' found in type '" + typeName + "'" << endl;
            Interpreter::reportError("Static member '" + memberName + "' not found in type '" + typeName + "'");
            return nullptr;
        }
        
        cout << "DEBUG: AccessExpression: target is not String, cannot access object without runtime type" << endl;
        Interpreter::reportError("Cannot access object without runtime type");
        return nullptr;
    }
    
    // 情况2: 目标有运行时类型，处理实例成员访问
    ObjectType* targetType = target->getValueType();
    
    // 使用统一接口查找方法
    Value* method = scopeManager.lookupMethod(memberName);
    if (method) {
        return method;
    }
    
    // 检查成员变量
    if (targetType->hasMember(memberName)) {
        Value* memberValue = targetType->accessMember(target, memberName);
        if (memberValue) {
            return memberValue;
        }
        Interpreter::reportError("Failed to access member variable '" + memberName + "'");
        return nullptr;
    }
    
    // 如果是类类型，检查静态成员
    if (ClassType* classType = dynamic_cast<ClassType*>(targetType)) {
        if (classType->hasStaticMethodName(memberName)) {
            return new ClassMethodValue(classType, memberName);
        }
    }
    
    Interpreter::reportError("Member '" + memberName + "' not found in type '" + targetType->getTypeName() + "'");
    return nullptr;
}

// 提取成员名称的辅助方法
string Interpreter::extractMemberName(Value* key) {
    if (String* stringKey = dynamic_cast<String*>(key)) {
        return stringKey->getValue();
    }
    
    // 如果不是字符串，尝试转换为字符串
    ObjectType* keyType = key->getValueType();
    if (!keyType) {
        Interpreter::reportError("Access key has no runtime type");
        return "";
    }
    
    // 检查是否有toString方法
    if (!keyType->hasMethod("toString")) {
        Interpreter::reportError("Access key type does not support conversion to string");
        return "";
    }
    
    // 调用toString方法
    vector<Value*> convertArgs;
    Value* convertedStringKey = keyType->callMethod(key, "toString", convertArgs);
    if (convertedStringKey) {
        string result = dynamic_cast<String*>(convertedStringKey)->getValue();
        delete convertedStringKey;
        return result;
    }
    
    Interpreter::reportError("Failed to convert access key to string");
    return "";
}

// 函数调用表达式求值 - 统一处理所有函数调用
Value* Interpreter::visit(CallExpression* call) {
    if (!call) return nullptr;
    
    // 求值被调用者（函数表达式）
    Value* callee = visit(call->callee);
    if (!callee) {
        Interpreter::reportError("Failed to evaluate function callee");
        return nullptr;
    }
    
    // 求值所有参数
    vector<Value*> evaluatedArgs;
    for (size_t i = 0; i < call->arguments.size(); ++i) {
        Expression* arg = call->arguments[i];
        Value* value = visit(arg);
        if (value) {
            evaluatedArgs.push_back(value);
        } else {
            Interpreter::reportError("Failed to evaluate argument " + to_string(i + 1) + " in function call");
            return nullptr;
        }
    }
    
    // 检查是否为可调用对象
    LOG_DEBUG("CallExpression: checking if callee is callable");
    LOG_DEBUG("CallExpression: callee type: " + (callee->getValueType() ? callee->getValueType()->getTypeName() : "nullptr"));
    LOG_DEBUG("CallExpression: callee builtin type: " + callee->getBuiltinTypeName());
    LOG_DEBUG("CallExpression: callee toString: " + callee->toString());
    
    if (!callee->isCallable()) {
        Interpreter::reportError("Object is not callable");
        return nullptr;
    }
    
    // 执行函数调用 - 使用执行器自动管理作用域
    Value* result = nullptr;
    
    // 检查函数类型并调用相应的方法
    if (Function* func = dynamic_cast<Function*>(callee)) {
        // 统一处理所有函数类型（内置函数和用户函数）
        if (!func->isCallable()) {
            Interpreter::reportError("Function is not callable");
        } else {
            // 检查参数数量（使用函数的参数验证方法）
            if (!func->validateArguments(evaluatedArgs)) {
                size_t expectedParams = func->getParameters().size();
                Interpreter::reportError("Function expects " + to_string(expectedParams) + " arguments, but got " + to_string(evaluatedArgs.size()));
            } else {
                // 使用新的函数调用架构
                BasicFunctionCall functionCall(scopeManager.getCurrentScope(), func, evaluatedArgs);
                result = functionCall.execute();
            }
        }
    } else {
        // 其他可调用对象（通过类型系统的方法调用）
        ObjectType* funcType = callee->getValueType();
        if (funcType) {
            vector<Value*> methodArgs = evaluatedArgs;
            result = funcType->callMethod(callee, "call", methodArgs);
        } else {
            Interpreter::reportError("Function object has no type information");
        }
    }
    
    // 清理参数（除了返回值）
    for (Value* arg : evaluatedArgs) {
        if (arg != result) {
            delete arg;
        }
    }
    
    return result;
}

// ==================== 自增自减操作处理 ====================

// 处理自增自减操作 - 现在通过UnaryExpression统一处理
Value* Interpreter::handleIncrementDecrement(UnaryExpression* unary) {
    if (!unary || !unary->operand) return nullptr;
    
    // 检查操作数是否为变量表达式（只有变量才能被修改）
    VariableExpression* varExpr = dynamic_cast<VariableExpression*>(unary->operand);
    if (!varExpr) {
        Interpreter::reportError("Increment/decrement operand must be a variable");
        return nullptr;
    }
    
    std::string varName = varExpr->name;
    
    // 获取变量的当前值
    Value* currentValue = scopeManager.lookupVariable(varName);
    if (!currentValue) {
        Interpreter::reportError("Variable '" + varName + "' not found for increment/decrement");
        return nullptr;
    }
    
    // 计算新值
    Value* newValue = nullptr;
    bool isIncrement = (unary->operator_->Tag == INCREMENT);
    
    if (isIncrement) {
        // 自增操作
        if (Integer* intVal = dynamic_cast<Integer*>(currentValue)) {
            newValue = new Integer(intVal->getValue() + 1);
        } else if (Double* doubleVal = dynamic_cast<Double*>(currentValue)) {
            newValue = new Double(doubleVal->getValue() + 1.0);
        } else {
            Interpreter::reportError("Cannot increment non-numeric value");
            return nullptr;
        }
    } else {
        // 自减操作
        if (Integer* intVal = dynamic_cast<Integer*>(currentValue)) {
            newValue = new Integer(intVal->getValue() - 1);
        } else if (Double* doubleVal = dynamic_cast<Double*>(currentValue)) {
            newValue = new Double(doubleVal->getValue() - 1.0);
        } else {
            Interpreter::reportError("Cannot decrement non-numeric value");
            return nullptr;
        }
    }
    
    // 根据前缀/后缀返回不同的值
    Value* result = nullptr;
    if (unary->isPrefixOperation()) {
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

// 三元表达式求值 - 条件 ? 真值 : 假值
Value* Interpreter::visit(TernaryExpression* ternary) {
    if (!ternary || !ternary->condition || !ternary->left || !ternary->right) {
        Interpreter::reportError("Invalid ternary expression");
        return nullptr;
    }
    
    // 1. 计算条件表达式的值
    Value* condition = visit(ternary->condition);
    if (!condition) {
        Interpreter::reportError("Failed to evaluate condition in ternary expression");
        return nullptr;
    }
    
    // 2. 计算真值和假值表达式
    Value* trueValue = visit(ternary->left);
    Value* falseValue = visit(ternary->right);
    
    if (!trueValue || !falseValue) {
        Interpreter::reportError("Failed to evaluate true or false value in ternary expression");
        delete condition;
        if (trueValue) delete trueValue;
        if (falseValue) delete falseValue;
        return nullptr;
    }
    
    // 3. 使用Calculator处理三元运算
    try {
        Value* result = calculator->executeTernaryOperation(condition, trueValue, falseValue);
        return result;
    } catch (const std::runtime_error& e) {
        Interpreter::reportError("Ternary operation failed: " + std::string(e.what()));
        delete condition;
        delete trueValue;
        delete falseValue;
        return nullptr;
    }
}
