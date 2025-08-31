#include "interpreter/interpreter.h"
#include "interpreter/calculate.h"
#include "interpreter/value.h"
#include "parser/expression.h"

using namespace std;

// ==================== Calculator类实现 ====================

// 执行二元运算
Value* Calculator::executeBinaryOperation(Value* left, Value* right, int op) {
    if (!left || !right) {
        throw runtime_error("Null operands for binary operation");
    }
    
    string methodName = OperatorMapping::getBinaryMethodName(op);
    if (methodName.empty()) {
        string opSymbol = OperatorMapping::getOperatorSymbol(op);
        throw runtime_error("Unsupported binary operation: " + opSymbol);
    }
    
    // 首先尝试左操作数的方法
    if (ObjectValue* leftObj = dynamic_cast<ObjectValue*>(left)) {
        if (Function* method = leftObj->getMethod(methodName)) {
            vector<Value*> args = {right};
            return method->execute(left, args);
        }
    }
    
    // 然后尝试右操作数的方法
    if (ObjectValue* rightObj = dynamic_cast<ObjectValue*>(right)) {
        if (Function* method = rightObj->getMethod(methodName)) {
            vector<Value*> args = {left};
            return method->execute(right, args);
        }
    }
    
    // 如果都没有，尝试类型转换后再次查找
    Value* result = tryWithTypeConversion(left, right, methodName);
    if (result) {
        return result;
    }
    
    // 最终失败，抛出详细错误信息
    string opSymbol = OperatorMapping::getOperatorSymbol(op);
    string leftType = left ? left->getBuiltinTypeName() : "null";
    string rightType = right ? right->getBuiltinTypeName() : "null";
    throw runtime_error("Operation " + opSymbol + " not supported between types " + leftType + " and " + rightType);
}

// 执行一元运算
Value* Calculator::executeUnaryOperation(Value* operand, int op) {
    if (!operand) {
        throw runtime_error("Null operand for unary operation");
    }
    
    string methodName = OperatorMapping::getUnaryMethodName(op);
    if (methodName.empty()) {
        string opSymbol = OperatorMapping::getOperatorSymbol(op);
        throw runtime_error("Unsupported unary operation: " + opSymbol);
    }
    
    // 尝试调用操作数的方法
    if (ObjectValue* operandObj = dynamic_cast<ObjectValue*>(operand)) {
        if (Function* method = operandObj->getMethod(methodName)) {
            vector<Value*> args;
            return method->execute(operand, args);
        }
    }
    
    // 如果类型系统没有该方法，说明不支持该操作
    string opSymbol = OperatorMapping::getOperatorSymbol(op);
    string operandType = operand ? operand->getBuiltinTypeName() : "null";
    throw runtime_error("Operation " + opSymbol + " not supported for type " + operandType);
}

// 智能类型转换 - 自动选择最佳转换策略
Value* Calculator::smartTypeConversion(Value* source, const string& targetType) {
    if (!source) return nullptr;
    
    // 如果已经是目标类型，直接返回
    if (source->getBuiltinTypeName() == targetType) {
        return source->clone();
    }
    
    // 首先尝试基于继承关系的转换
    Value* result = convertUsingInheritance(source, targetType);
    if (result) {
        return result;
    }
    
    // 尝试直接转换
    result = tryConvertValue(source, targetType);
    if (result) {
        return result;
    }
    
    // 尝试通过通用类型转换
    result = tryConvertThroughCommonType(source, targetType);
    if (result) {
        return result;
    }
    
    return nullptr;
}

// 基于继承关系的类型转换
Value* Calculator::convertUsingInheritance(Value* source, const string& targetType) {
    if (!source || !source->getValueType()) return nullptr;
    
    ObjectType* sourceType = source->getValueType();
    ObjectType* targetTypeObj = getTypeRegistry()->getType(targetType);
    
    if (!targetTypeObj) return nullptr;
    
    // 检查是否可以直接转换
    if (canConvertTo(sourceType, targetTypeObj)) {
        return sourceType->convertTo(targetTypeObj, source);
    }
    
    // 检查是否有共同的祖先类型
    ObjectType* commonAncestor = findCommonAncestor(sourceType, targetTypeObj);
    if (commonAncestor) {
        // 先转换到共同祖先，再转换到目标类型
        Value* intermediate = sourceType->convertTo(commonAncestor, source);
        if (intermediate) {
            Value* result = commonAncestor->convertTo(targetTypeObj, intermediate);
            delete intermediate;
            return result;
        }
    }
    
    return nullptr;
}

// 查找两个类型的共同祖先
ObjectType* Calculator::findCommonAncestor(ObjectType* type1, ObjectType* type2) {
    if (!type1 || !type2) return nullptr;
    
    // 如果类型相同，返回自身
    if (type1 == type2) return type1;
    
    // 检查type1是否是type2的父类
    if (type2->isSubtypeOf(type1)) {
        return type1;
    }
    
    // 检查type2是否是type1的父类
    if (type1->isSubtypeOf(type2)) {
        return type2;
    }
    
    // 递归查找共同祖先
    if (type1->getParentType()) {
        ObjectType* ancestor = findCommonAncestor(type1->getParentType(), type2);
        if (ancestor) return ancestor;
    }
    
    if (type2->getParentType()) {
        ObjectType* ancestor = findCommonAncestor(type1, type2->getParentType());
        if (ancestor) return ancestor;
    }
    
    return nullptr;
}

// 检查是否可以从源类型转换到目标类型
bool Calculator::canConvertTo(ObjectType* sourceType, ObjectType* targetType) {
    if (!sourceType || !targetType) return false;
    
    // 检查目标类型是否支持从源类型转换
    if (targetType->isCompatibleWith(sourceType)) {
        return true;
    }
    
    // 检查源类型是否支持转换到目标类型
    if (sourceType->isCompatibleWith(targetType)) {
        return true;
    }
    
    // 检查继承关系
    if (sourceType->isSubtypeOf(targetType)) {
        return true;
    }
    
    return false;
}

// 智能二元运算 - 自动处理类型转换
Value* Calculator::smartBinaryOperation(Value* left, Value* right, int op) {
    if (!left || !right) {
        throw runtime_error("Null operands for binary operation");
    }
    
    string methodName = OperatorMapping::getBinaryMethodName(op);
    if (methodName.empty()) {
        string opSymbol = OperatorMapping::getOperatorSymbol(op);
        throw runtime_error("Unsupported binary operation: " + opSymbol);
    }
    
    // 首先尝试直接运算
    try {
        return executeBinaryOperation(left, right, op);
    } catch (...) {
        // 如果直接运算失败，尝试智能类型转换
    }
    
    // 智能类型转换策略
    Value* result = trySmartTypeConversion(left, right, methodName);
    if (result) {
        return result;
    }
    
    // 最终失败
    string opSymbol = OperatorMapping::getOperatorSymbol(op);
    string leftType = left ? left->getBuiltinTypeName() : "null";
    string rightType = right ? right->getBuiltinTypeName() : "null";
    throw runtime_error("Operation " + opSymbol + " not supported between types " + leftType + " and " + rightType);
}

// 尝试类型转换后执行运算
Value* Calculator::tryWithTypeConversion(Value* left, Value* right, const string& methodName) {
    // 智能类型转换逻辑
    // 1. 尝试将右操作数转换为左操作数的类型
    Value* convertedRight = tryConvertValue(right, left->getBuiltinTypeName());
    if (convertedRight) {
        if (ObjectValue* leftObj = dynamic_cast<ObjectValue*>(left)) {
            if (Function* method = leftObj->getMethod(methodName)) {
                vector<Value*> args = {convertedRight};
                Value* result = method->execute(left, args);
                delete convertedRight; // 清理转换后的值
                return result;
            }
        }
        delete convertedRight;
    }
    
    // 2. 尝试将左操作数转换为右操作数的类型
    Value* convertedLeft = tryConvertValue(left, right->getBuiltinTypeName());
    if (convertedLeft) {
        if (ObjectValue* rightObj = dynamic_cast<ObjectValue*>(right)) {
            if (Function* method = rightObj->getMethod(methodName)) {
                vector<Value*> args = {convertedLeft};
                Value* result = method->execute(right, args);
                delete convertedLeft; // 清理转换后的值
                return result;
            }
        }
        delete convertedLeft;
    }
    
    // 3. 尝试转换为通用类型（如double）
    if (methodName == "add" || methodName == "sub" || methodName == "mul" || methodName == "div") {
        Value* convertedLeft = tryConvertValue(left, "double");
        Value* convertedRight = tryConvertValue(right, "double");
        if (convertedLeft && convertedRight) {
            if (ObjectValue* leftObj = dynamic_cast<ObjectValue*>(convertedLeft)) {
                if (Function* method = leftObj->getMethod(methodName)) {
                    vector<Value*> args = {convertedRight};
                    Value* result = method->execute(convertedLeft, args);
                    delete convertedLeft;
                    delete convertedRight;
                    return result;
                }
            }
            delete convertedLeft;
            delete convertedRight;
        }
    }
    
    return nullptr;
}

// 尝试将值转换为指定类型
Value* Calculator::tryConvertValue(Value* value, const string& targetType) {
    if (!value) return nullptr;
    
    if (ObjectValue* obj = dynamic_cast<ObjectValue*>(value)) {
        if (Function* convertMethod = obj->getMethod("convertTo")) {
            vector<Value*> args = {new String(targetType)};
            Value* result = convertMethod->execute(value, args);
            delete args[0]; // 清理临时字符串
            return result;
        }
    }
    
    return nullptr;
}

// 尝试通过通用类型进行转换
Value* Calculator::tryConvertThroughCommonType(Value* source, const string& targetType) {
    // 这里可以实现更复杂的转换策略
    // 例如：通过double作为中间类型进行转换
    return nullptr;
}

// 智能类型转换策略
Value* Calculator::trySmartTypeConversion(Value* left, Value* right, const string& methodName) {
    // 实现更智能的类型转换策略
    // 例如：根据操作符类型选择最佳转换策略
    return nullptr;
}

// ==================== Interpreter类的方法实现 ====================

// 通用的二元运算计算方法 - 完全通过类型系统调用运算符方法
Value* Interpreter::calculate_binary(Value* left, Value* right, int op) {
    // 使用智能二元运算，自动处理类型转换
    return Calculator::smartBinaryOperation(left, right, op);
}

// 通用的一元运算计算方法 - 完全通过类型系统调用运算符方法
Value* Interpreter::calculate_unary(Value* operand, int op) {
    return Calculator::executeUnaryOperation(operand, op);
}


