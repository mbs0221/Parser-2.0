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
    // 第二步：类型转换
    Value* convertedLeft = nullptr;
    Value* convertedRight = nullptr;
    // 创建类型转换表达式
    CastExpression* leftCastExpr = createCastExpression(left, targetTypeName);
    CastExpression* rightCastExpr = createCastExpression(right, targetTypeName);
    if (leftCastExpr) {
        convertedLeft = interpreter_->visit(leftCastExpr);
        delete leftCastExpr;
    } else {
        convertedLeft = left->clone(); // 如果不需要转换，直接克隆
    }
    if (rightCastExpr) {
        convertedRight = interpreter_->visit(rightCastExpr);
        delete rightCastExpr;
    } else {
        convertedRight = right->clone(); // 如果不需要转换，直接克隆
    }
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
                if (leftType && leftType->hasMethod(operation)) {
                    vector<Value*> args = {right};
                    return leftType->callMethod(left, operation, args);
                }
                // 尝试调用右操作数类型注册的运算方法
                ObjectType* rightType = right->getValueType();
                if (rightType && rightType->hasMethod(operation)) {
                    vector<Value*> args = {left};
                    return rightType->callMethod(right, operation, args);
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
    // 检查是否为用户自定义类型（非基础类型）
    if (!isBasicType(targetTypeName)) {
        // 用户自定义类型：调用类型系统注册的运算方法
        result = performUserTypeBinaryOperation(convertedLeft, convertedRight, op);
    } else {
        // 基础类型：使用模板方法
        result = performBasicTypeBinaryOperation(convertedLeft, convertedRight, op);
    }
    // 清理转换后的操作数
    delete convertedLeft;
    delete convertedRight;
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
    Value* convertedOperand = nullptr;
    CastExpression* castExpr = createCastExpression(operand, targetTypeName);
    if (castExpr) {
        convertedOperand = interpreter_->visit(castExpr);
        delete castExpr;
    } else {
        convertedOperand = operand->clone(); // 如果不需要转换，直接克隆
    }
    if (!convertedOperand) {
        // 如果转换失败，尝试用户定义类型的一元运算
        if (op) {
            string operation = op->getOperationName();
            if (!operation.empty()) {
                ObjectType* operandType = operand->getValueType();
                if (operandType && operandType->hasMethod(operation)) {
                    vector<Value*> args = {};
                    return operandType->callMethod(operand, operation, args);
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
    // 使用类型系统将条件转换为布尔值
    ObjectType* conditionType = condition->getValueType();
    if (!conditionType) {
        throw runtime_error("Condition has no runtime type");
    }
    // 检查是否有toBool方法
    if (!conditionType->hasMethod("toBool")) {
        throw runtime_error("Condition type does not support conversion to boolean");
    }
    // 调用类型注册的toBool方法
    vector<Value*> convertArgs;
    Value* boolCondition = conditionType->callMethod(condition, "toBool", convertArgs);
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

// 创建类型转换表达式
CastExpression* Calculator::createCastExpression(Value* value, const string& targetTypeName) {
    if (!value) return nullptr;
    string currentTypeName = getTypeName(value);
    // 如果已经是目标类型，不需要转换
    if (currentTypeName == targetTypeName) {
        return nullptr;
    }
    // 根据value的类型创建对应的常量表达式
    Expression* operandExpr = nullptr;
    if (Integer* intValue = dynamic_cast<Integer*>(value)) {
        operandExpr = new ConstantExpression<int>(intValue->getValue());
    } else if (Double* doubleValue = dynamic_cast<Double*>(value)) {
        operandExpr = new ConstantExpression<double>(doubleValue->getValue());
    } else if (Bool* boolValue = dynamic_cast<Bool*>(value)) {
        operandExpr = new ConstantExpression<bool>(boolValue->getValue());
    } else if (Char* charValue = dynamic_cast<Char*>(value)) {
        operandExpr = new ConstantExpression<char>(charValue->getValue());
    } else if (String* stringValue = dynamic_cast<String*>(value)) {
        operandExpr = new ConstantExpression<string>(stringValue->getValue());
    } else {
        // 对于用户自定义类型，不支持自动转换
        // 在实际编程中，不同类型之间的转换通常需要显式定义
        return nullptr;
    }
    // 创建Cast表达式
    return new CastExpression(operandExpr, targetTypeName);
}

// 检查类型是否可以转换
bool Calculator::canConvertTo(ObjectType* sourceType, ObjectType* targetType) {
    if (!sourceType || !targetType) return false;
    // 检查是否为子类型关系
    if (sourceType->isSubtypeOf(targetType)) {
        return true;
    }
    // 检查是否有对应的转换方法
    string targetTypeName = targetType->getTypeName();
    string methodName = "to" + targetTypeName;
    // 首字母大写
    if (!methodName.empty() && methodName.length() > 2) {
        methodName[2] = toupper(methodName[2]);
    }
    if (sourceType->hasMethod(methodName)) {
        return true;
    }
    return false;
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

// 执行用户自定义类型的二元运算（使用类型系统方法）
Value* Calculator::performUserTypeBinaryOperation(Value* left, Value* right, Operator* op) {
    if (!left || !right || !op) return nullptr;
    // 获取两个操作数的类型
    ObjectType* leftType = left->getValueType();
    ObjectType* rightType = right->getValueType();
    if (!leftType || !rightType) return nullptr;
    // 策略1：优先尝试左操作数类型的方法
    if (leftType->hasMethod(op->getOperationName())) {
        vector<Value*> args = {right};
        Value* result = leftType->callMethod(left, op->getOperationName(), args);
        if (result) return result;
    }
    // 策略2：如果左操作数类型没有方法，尝试右操作数类型的方法
    if (rightType->hasMethod(op->getOperationName())) {
        vector<Value*> args = {left};
        Value* result = rightType->callMethod(right, op->getOperationName(), args);
        if (result) return result;
    }
    // 策略3：如果两个类型都没有方法，尝试查找专门的混合类型运算方法
    // 例如：MyClass + int 可能需要特殊的处理
    string mixedMethodName = op->getOperationName() + "With" + rightType->getTypeName();
    if (leftType->hasMethod(mixedMethodName)) {
        vector<Value*> args = {right};
        Value* result = leftType->callMethod(left, mixedMethodName, args);
        if (result) return result;
    }
    return nullptr;
}

// 执行用户自定义类型的一元运算（使用类型系统方法）
Value* Calculator::performUserTypeUnaryOperation(Value* operand, Operator* op) {
    if (!operand || !op) return nullptr;
    // 获取操作数的类型
    ObjectType* operandType = operand->getValueType();
    if (!operandType) return nullptr;
    // 检查是否有对应的单目运算方法
    if (operandType->hasMethod(op->getOperationName())) {
        // 调用类型系统注册的单目运算方法
        vector<Value*> args = {};
        return operandType->callMethod(operand, op->getOperationName(), args);
    }
    return nullptr;
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
