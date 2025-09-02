#include <unordered_map>
#include <unordered_set>

#include "interpreter/core/interpreter.h"
#include "interpreter/values/calculate.h"
#include "interpreter/values/value.h"
#include "parser/expression.h"
#include "lexer/token.h"

using namespace std;
using namespace lexer;

// ==================== 泛型模板运算函数 ====================
// 通用的二元运算模板函数 - 支持所有基础类型
template<typename T>
Value* performBinaryOperation(T* left, T* right, Operator* op) {
    if (!left || !right) return nullptr;
    switch (op->getOperationType()) {

        // 算术运算符（使用ASCII码）
        case '+': return new T(*left + *right);
        case '-': return new T(*left - *right);
        case '*': return new T(*left * *right);
        case '/': return new T(*left / *right);
        case '%': return new T(*left % *right);
        // 比较运算符
        case '<': return new Bool(*left < *right);
        case '>': return new Bool(*left > *right);
        case LE: return new Bool(*left <= *right);  // LE
        case GE: return new Bool(*left >= *right);  // GE
        case EQ_EQ: return new Bool(*left == *right);
        case NE_EQ: return new Bool(*left != *right);
        // 位运算符
        case '&': return new T(*left & *right);
        case '|': return new T(*left | *right);
        case '^': return new T(*left ^ *right);
        case LEFT_SHIFT: return new T(*left << *right);
        case RIGHT_SHIFT: return new T(*left >> *right);
        default: return nullptr;
    }
}

// 布尔类型的特殊处理 - 只支持逻辑运算
template<>
Value* performBinaryOperation(Bool* left, Bool* right, Operator* op) {
    if (!left || !right || !op) return nullptr;
    switch (op->getOperationType()) {

        case AND_AND: return new Bool(*left && *right);
        case OR_OR: return new Bool(*left || *right);
        case EQ_EQ: return new Bool(*left == *right);
        case NE_EQ: return new Bool(*left != *right);
        default: {
            throw std::runtime_error("Unsupported binary operation for Bool type");
        }
    }
}

// 字符串类型的特殊处理 - 只支持连接和比较
template<>
Value* performBinaryOperation(String* left, String* right, Operator* op) {
    if (!left || !right || !op) return nullptr;
    switch (op->getOperationType()) {

        case '+': {
            // 字符串连接操作
            std::string result = left->getValue() + right->getValue();
            return new String(result);
        }
        case '<': return new Bool(*left < *right);
        case '>': return new Bool(*left > *right);
        case LE: return new Bool(*left <= *right);
        case GE: return new Bool(*left >= *right);
        case EQ_EQ: return new Bool(*left == *right);
        case NE_EQ: return new Bool(*left != *right);
        default: {
            throw std::runtime_error("Unsupported binary operation for String type");
        }
    }
}

// Double类型的特殊处理 - 只支持算术运算和比较运算，不支持位运算
template<>
Value* performBinaryOperation(Double* left, Double* right, Operator* op) {
    if (!left || !right || !op) return nullptr;
    switch (op->getOperationType()) {

        // 算术运算符
        case '+': return new Double(*left + *right);
        case '-': return new Double(*left - *right);
        case '*': return new Double(*left * *right);
        case '/': return new Double(*left / *right);
        // 注意：Double不支持取模运算，因为浮点数取模没有意义
        
        // 比较运算符
        case '<': return new Bool(*left < *right);
        case '>': return new Bool(*left > *right);
        case LE: return new Bool(*left <= *right);
        case GE: return new Bool(*left >= *right);
        case EQ_EQ: return new Bool(*left == *right);
        case NE_EQ: return new Bool(*left != *right);
        
        // Double不支持位运算，这些操作抛出异常
        case '&': {
            throw std::runtime_error("Bitwise AND operation not supported for Double type");
        }
        case '|': {
            throw std::runtime_error("Bitwise OR operation not supported for Double type");
        }
        case '^': {
            throw std::runtime_error("Bitwise XOR operation not supported for Double type");
        }
        case LEFT_SHIFT: {
            throw std::runtime_error("Left shift operation not supported for Double type");
        }
        case RIGHT_SHIFT: {
            throw std::runtime_error("Right shift operation not supported for Double type");
        }
        default: {
            throw std::runtime_error("Unsupported binary operation for Double type");
        }
    }
}

// 跨类型运算的模板特化
template<typename T1, typename T2>
Value* performBinaryOperation(T1* left, T2* right, Operator* op) {
    if (!left || !right || !op) return nullptr;
    switch (op->getOperationType()) {

        // 算术运算符 - 转换为更宽的类型
        case '+': return new T2(*left + *right);
        case '-': return new T2(*left - *right);
        case '*': return new T2(*left * *right);
        case '/': return new T2(*left / *right);
        case '%': return new T2(*left % *right);
        // 比较运算符 - 返回布尔值
        case '<': return new Bool(*left < *right);
        case '>': return new Bool(*left > *right);
        case LE: return new Bool(*left <= *right);
        case GE: return new Bool(*left >= *right);
        case EQ_EQ: return new Bool(*left == *right);
        case NE_EQ: return new Bool(*left != *right);
        // 位运算符 - 转换为更宽的类型
        case '&': return new T2(*left & *right);
        case '|': return new T2(*left | *right);
        case '^': return new T2(*left ^ *right);
        case LEFT_SHIFT: return new T2(*left << *right);
        case RIGHT_SHIFT: return new T2(*left >> *right);
        default: {
            throw std::runtime_error("Unsupported binary operation for mixed types");
        }
    }
}

// 通用的一元运算模板函数（基础类型）
template<typename T>
Value* performUnaryOperation(T* operand, Operator* op) {
    if (!operand || !op) return nullptr;
    switch (op->getOperationType()) {

        case '+': return new T(+*operand);
        case '-': return new T(-*operand);
        case '~': return new T(~*operand);
        case '!': return new Bool(!*operand);
        case INCREMENT: return new T(*operand + 1);
        case DECREMENT: return new T(*operand - 1);
        default: {
            throw std::runtime_error("Unsupported unary operation for generic type");
        }
    }
}

// Integer类型的一元运算特殊处理 - 现在支持逻辑非
template<>
Value* performUnaryOperation(Integer* operand, Operator* op) {
    if (!operand || !op) return nullptr;
    switch (op->getOperationType()) {

        case '+': return new Integer(+*operand);
        case '-': return new Integer(-*operand);
        case '~': return new Integer(~*operand);
        case '!': return new Bool(!*operand);  // 现在支持逻辑非
        case INCREMENT: return new Integer(*operand + 1);
        case DECREMENT: return new Integer(*operand - 1);
        default: {
            throw std::runtime_error("Unsupported unary operation for Integer type");
        }
    }
}

// Char类型的一元运算特殊处理 - 现在支持逻辑非
template<>
Value* performUnaryOperation(Char* operand, Operator* op) {
    if (!operand || !op) return nullptr;
    switch (op->getOperationType()) {

        case '+': return new Char(+*operand);
        case '-': return new Char(-*operand);
        case '~': return new Char(~*operand);
        case '!': return new Bool(!*operand);  // 现在支持逻辑非
        case INCREMENT: return new Char(*operand + 1);
        case DECREMENT: return new Char(*operand - 1);
        default: {
            throw std::runtime_error("Unsupported unary operation for Char type");
        }
    }
}

// Double类型的一元运算特殊处理 - 不支持位运算和逻辑非
template<>
Value* performUnaryOperation(Double* operand, Operator* op) {
    if (!operand || !op) return nullptr;
    switch (op->getOperationType()) {

        case '+': return new Double(+*operand);
        case '-': return new Double(-*operand);
        case '!': {
            throw std::runtime_error("Logical NOT operation not supported for Double type");
        }
        case INCREMENT: return new Double(*operand + 1.0);
        case DECREMENT: return new Double(*operand - 1.0);
        
        // Double不支持位运算
        case '~': {
            throw std::runtime_error("Bitwise NOT operation not supported for Double type");
        }
        default: {
            throw std::runtime_error("Unsupported unary operation for Double type");
        }
    }
}

// 布尔类型的一元运算特殊处理
template<>
Value* performUnaryOperation(Bool* operand, Operator* op) {
    if (!operand || !op) return nullptr;
    switch (op->getOperationType()) {

        case '!': return new Bool(!*operand);
        default: {
            throw std::runtime_error("Unsupported unary operation for Bool type");
        }
    }
}

// 字符串类型的一元运算特殊处理
template<>
Value* performUnaryOperation(String* operand, Operator* op) {
    if (!operand || !op) return nullptr;
    switch (op->getOperationType()) {
        case '+': return new String(*operand); // 字符串的正号操作返回原字符串
        default: {
            throw std::runtime_error("Unsupported unary operation for String type");
        }
    }
}


// ==================== Calculator类实现 ====================
// 宏定义：简化基础类型一元运算的代码
#define PERFORM_BASIC_TYPE_UNARY_OPERATION(TYPE_CLASS) \
    if (TYPE_CLASS* operand##TYPE_CLASS = dynamic_cast<TYPE_CLASS*>(operand)) { \
        return performUnaryOperation(operand##TYPE_CLASS, op); \
    }
// 执行基础类型的一元运算（使用运算符重载）
Value* Calculator::executeBasicTypeUnaryOperation(Value* operand, Operator* op) {
    if (!operand || !op) return nullptr;
    // 检查是否为基础类型
    string typeName = getTypeName(operand);
    if (isBasicType(typeName)) {
        // 基础类型：使用模板方法
        PERFORM_BASIC_TYPE_UNARY_OPERATION(Integer)
        PERFORM_BASIC_TYPE_UNARY_OPERATION(Double)
        PERFORM_BASIC_TYPE_UNARY_OPERATION(Bool)
        PERFORM_BASIC_TYPE_UNARY_OPERATION(Char)
        PERFORM_BASIC_TYPE_UNARY_OPERATION(String)
    } else {
        // 用户自定义类型：使用类型系统方法
        return performUserTypeUnaryOperation(operand, op);
    }
    return nullptr; // 不支持的类型
}

// 执行二元运算 - 简化的计算流程
Value* Calculator::executeBinaryOperation(Value* left, Value* right, Operator* op) {
    if (!left || !right) {
        throw runtime_error("Null operands for binary operation");
    }
    // 第一步：确定兼容类型（选择优先级更高的类型）
    string targetTypeName = determineCompatibleType(left, right);
    LOG_DEBUG("Target type name: " + targetTypeName);
    // 第二步：类型转换
    Value* convertedLeft = tryConvertValue(left, targetTypeName);
    Value* convertedRight = tryConvertValue(right, targetTypeName);
    if (!convertedLeft || !convertedRight) {
        // 清理转换后的操作数
        if (convertedLeft) delete convertedLeft;
        if (convertedRight) delete convertedRight;
        // 如果转换失败，尝试用户定义类型的运算
        if (op) {
            string operation = op->getOperationName();
            if (!operation.empty()) {
                // 尝试调用左操作数类型注册的运算方法
                ObjectType* leftType = left->getValueType();
                if (leftType && leftType->supportsMethods()) {
                    Value* result = callMethodWithReference(leftType, left, operation, {right});
                    if (result) return result;
                }
                
                // 尝试调用右操作数类型注册的运算方法
                ObjectType* rightType = right->getValueType();
                if (rightType && rightType->supportsMethods()) {
                    Value* result = callMethodWithReference(rightType, right, operation, {left});
                    if (result) return result;
                }
            }
        }
        // 3. 如果都不支持，抛出错误
        string opSymbol = op ? op->getOperatorSymbol() : "unknown";
        string leftTypeName = left->getTypeName();
        string rightTypeName = right->getTypeName();
        throw runtime_error("Operation " + opSymbol + " not supported between types " + leftTypeName + " and " + rightTypeName);
    }
    // 第三步：具体计算
    Value* result = nullptr;
    
    // 执行类型转换
    if (convertedLeft && convertedRight) {
        // 检查是否为用户自定义类型（非基础类型）
        if (!isBasicType(targetTypeName)) {
            // 用户自定义类型：调用类型系统注册的运算方法
            result = performUserTypeBinaryOperation(convertedLeft, convertedRight, op);
        } else {
            // 基础类型：使用模板方法
            result = performBasicTypeBinaryOperation(convertedLeft, convertedRight, op);
        }
    }
    
    return result;
}

// 执行一元运算 - 简化的计算流程
Value* Calculator::executeUnaryOperation(Value* operand, Operator* op) {
    if (!operand) {
        throw runtime_error("Null operand for unary operation");
    }
    // 第一步：确定目标类型（对于一元运算，通常保持原类型）
    string targetTypeName = getTypeName(operand);
    // 第二步：类型转换（如果需要的话）
    Value* convertedOperand = tryConvertValue(operand, targetTypeName);
    LOG_DEBUG("Converted operand: " + convertedOperand->toString());
    if (!convertedOperand) {
        // 如果转换失败，尝试用户定义类型的一元运算
        if (op) {
            string operation = op->getOperationName();
            if (!operation.empty()) {
                ObjectType* operandType = operand->getValueType();
                if (operandType && operandType->supportsMethods()) {
                    // 使用callMethodWithReference优化方法调用
                    Value* result = callMethodWithReference(operandType, operand, operation, {});
                    if (result) return result;
                }
            }
        }
        // 如果都不支持，抛出错误
        string opSymbol = op ? op->getOperatorSymbol() : "unknown";
        string operandTypeName = operand->getTypeName();
        throw runtime_error("Unary operation " + opSymbol + " not supported for type " + operandTypeName);
    }
    // 第三步：具体计算
    Value* result = nullptr;
    // 检查是否为用户自定义类型（非基础类型）
    if (!isBasicType(targetTypeName)) {
        // 用户自定义类型：调用类型系统注册的运算方法
        result = performUserTypeUnaryOperation(convertedOperand, op);
    } else {
        // 基础类型：使用模板方法
        result = executeBasicTypeUnaryOperation(convertedOperand, op);
    }
    // 清理转换后的操作数
    delete convertedOperand;
    return result;
}

// 执行三元运算 - 条件 ? 真值 : 假值
Value* Calculator::executeTernaryOperation(Value* condition, Value* trueValue, Value* falseValue) {
    if (!condition || !trueValue || !falseValue) {
        throw runtime_error("Null operands for ternary operation");
    }
    // 使用tryConvertValue将条件转换为布尔值
    Value* boolCondition = tryConvertValue(condition, "bool");
    
    if (!boolCondition) {
        throw runtime_error("Failed to convert condition to boolean");
    }
    
    // 根据条件选择返回值
    if (dynamic_cast<Bool*>(boolCondition)->getValue()) {
        delete boolCondition;
        return trueValue->clone(); // 返回真值的副本
    } else {
        delete boolCondition;
        return falseValue->clone(); // 返回假值的副本
    }
}

// 私有辅助方法：使用callMethodOnValue调用value的转换方法
Value* Calculator::tryConvertValue(Value* sourceValue, const string& targetTypeName) {
    if (!sourceValue) return nullptr;
    
    string currentTypeName = getTypeName(sourceValue);
    
    // 如果已经是目标类型，不需要转换
    if (currentTypeName == targetTypeName) {
        return sourceValue; // 不需要转换
    }

    // 使用callMethodOnValue调用value的转换方法
    // 例如：调用 toInt(), toDouble(), toBool(), toString() 等方法
    // 首字母大写
    string methodName = "to" + targetTypeName;
    if (!methodName.empty() && methodName.length() > 2) {
        methodName[2] = toupper(methodName[2]);
    }
    return interpreter_->callMethodOnInstance(sourceValue, methodName, {});
}

// 确定兼容类型（选择优先级更高的类型）
string Calculator::determineCompatibleType(Value* left, Value* right) {
    if (!left || !right) return "unknown";
    // 获取操作数的类型名称
    string leftTypeName = getTypeName(left);
    string rightTypeName = getTypeName(right);
    // 使用类型优先级确定目标类型（选择优先级更高的类型）
    int leftPriority = TypeConversionPriority::getPriority(leftTypeName);
    int rightPriority = TypeConversionPriority::getPriority(rightTypeName);
    return (leftPriority > rightPriority) ? leftTypeName : rightTypeName;
}

// 检查是否为基础类型
bool Calculator::isBasicType(const string& typeName) {
    static const unordered_set<string> basicTypes = {
        "int", "double", "bool", "string", "char"
    };
    return basicTypes.find(typeName) != basicTypes.end();
}

// 私有辅助方法：使用Interpreter的通用方法调用机制
Value* Calculator::callMethodWithReference(ObjectType* targetType, Value* targetInstance, 
                                         const string& methodName, const vector<Value*>& args) {
    if (!targetType || !targetInstance) return nullptr;
    
    // 使用Interpreter的通用方法调用机制
    return interpreter_->callMethodOnInstance(targetInstance, methodName, args);
}

// 执行用户自定义类型的二元运算（使用MethodReference优化）
Value* Calculator::performUserTypeBinaryOperation(Value* left, Value* right, Operator* op) {
    if (!left || !right || !op) return nullptr;
    
    // 获取两个操作数的类型
    ObjectType* leftType = left->getValueType();
    ObjectType* rightType = right->getValueType();
    if (!leftType || !rightType) return nullptr;
    
    // 策略1：优先尝试左操作数类型的方法
    Value* result = callMethodWithReference(leftType, left, op->getOperationName(), {right});
    if (result) return result;
    
    // 策略2：如果左操作数类型没有方法，尝试右操作数类型的方法
    result = callMethodWithReference(rightType, right, op->getOperationName(), {left});
    if (result) return result;
    
    // 策略3：如果两个类型都没有方法，尝试查找专门的混合类型运算方法
    // 例如：MyClass + int 可能需要特殊的处理
    string mixedMethodName = op->getOperationName() + "With" + rightType->getTypeName();
    result = callMethodWithReference(leftType, left, mixedMethodName, {right});
    
    return result;
}

// 执行用户自定义类型的一元运算（使用MethodReference优化）
Value* Calculator::performUserTypeUnaryOperation(Value* operand, Operator* op) {
    if (!operand || !op) return nullptr;
    
    // 获取操作数的类型
    ObjectType* operandType = operand->getValueType();
    if (!operandType) return nullptr;
    
    // 使用辅助方法调用一元运算方法
    return callMethodWithReference(operandType, operand, op->getOperationName(), {});
}

// 宏定义：简化基础类型二元运算的代码
#define PERFORM_BASIC_TYPE_OPERATION(TYPE_NAME, TYPE_CLASS) \
    if (typeName == TYPE_NAME) { \
        TYPE_CLASS* left##TYPE_CLASS = dynamic_cast<TYPE_CLASS*>(left); \
        TYPE_CLASS* right##TYPE_CLASS = dynamic_cast<TYPE_CLASS*>(right); \
        if (left##TYPE_CLASS && right##TYPE_CLASS) { \
            return performBinaryOperation(left##TYPE_CLASS, right##TYPE_CLASS, op); \
        } \
    }
// 执行基础类型的二元运算（使用模板方法）
Value* Calculator::performBasicTypeBinaryOperation(Value* left, Value* right, Operator* op) {
    if (!left || !right || !op) return nullptr;
    string typeName = getTypeName(left);
    // 使用宏简化重复代码
    PERFORM_BASIC_TYPE_OPERATION("int", Integer)
    PERFORM_BASIC_TYPE_OPERATION("double", Double)
    PERFORM_BASIC_TYPE_OPERATION("bool", Bool)
    PERFORM_BASIC_TYPE_OPERATION("string", String)
    PERFORM_BASIC_TYPE_OPERATION("char", Char)
    return nullptr;
}

// 获取值的类型名称
string Calculator::getTypeName(Value* value) {
    if (!value) return "null";
    ObjectType* valueType = value->getValueType();
    if (valueType) {
        return valueType->getTypeName();
    }
    return "unknown";
}
