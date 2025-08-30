#include "interpreter/interpreter.h"
#include "interpreter/value.h"
#include "parser/expression.h"

using namespace std;

// ==================== 模板特化实现 ====================

// Bool类型的特化版本
template<>
Value* Interpreter::calculate<Bool>(Value* left, Value* right, int op) {
    Bool* leftBool = static_cast<Bool*>(left);
    Bool* rightBool = static_cast<Bool*>(right);
    
    Bool* result = nullptr;
    switch (op) {
        case AND_AND: result = new Bool(leftBool->getValue() && rightBool->getValue()); break;
        case OR_OR: result = new Bool(leftBool->getValue() || rightBool->getValue()); break;
        case EQ_EQ: result = new Bool(leftBool->getValue() == rightBool->getValue()); break;
        case NE_EQ: result = new Bool(leftBool->getValue() != rightBool->getValue()); break;
        default: throw runtime_error("Unsupported binary operation for Bool");
    }
    
    // 不删除 left 和 right，因为它们可能还在其他地方被使用
    // delete left;
    // delete right;
    return result;
}

// Integer类型的特化版本
template<>
Value* Interpreter::calculate<Integer>(Value* left, Value* right, int op) {
    Integer* leftInt = static_cast<Integer*>(left);
    Integer* rightInt = static_cast<Integer*>(right);
    
    Value* result = nullptr;
    switch (op) {
        case '+': result = new Integer(leftInt->getValue() + rightInt->getValue()); break;
        case '-': result = new Integer(leftInt->getValue() - rightInt->getValue()); break;
        case '*': result = new Integer(leftInt->getValue() * rightInt->getValue()); break;
        case '/': 
            if (rightInt->getValue() == 0) {
                delete left;
                delete right;
                throw runtime_error("Division by zero");
            }
            result = new Integer(leftInt->getValue() / rightInt->getValue()); 
            break;
        case '%': 
            if (rightInt->getValue() == 0) {
                delete left;
                delete right;
                throw runtime_error("Modulo by zero");
            }
            result = new Integer(leftInt->getValue() % rightInt->getValue()); 
            break;
        case '&': result = new Integer(leftInt->getValue() & rightInt->getValue()); break;
        case '|': result = new Integer(leftInt->getValue() | rightInt->getValue()); break;
        case '^': result = new Integer(leftInt->getValue() ^ rightInt->getValue()); break;
        case LEFT_SHIFT: result = new Integer(leftInt->getValue() << rightInt->getValue()); break;
        case RIGHT_SHIFT: result = new Integer(leftInt->getValue() >> rightInt->getValue()); break;
        case EQ_EQ: result = new Bool(leftInt->getValue() == rightInt->getValue()); break;
        case NE_EQ: result = new Bool(leftInt->getValue() != rightInt->getValue()); break;
        case '<': result = new Bool(leftInt->getValue() < rightInt->getValue()); break;
        case '>': result = new Bool(leftInt->getValue() > rightInt->getValue()); break;
        case LE: result = new Bool(leftInt->getValue() <= rightInt->getValue()); break;
        case GE: result = new Bool(leftInt->getValue() >= rightInt->getValue()); break;
        default: throw runtime_error("Unsupported binary operation for Integer");
    }
    
    // 不删除 left 和 right，因为它们可能还在其他地方被使用
    // delete left;
    // delete right;
    return result;
}

// Double类型的特化版本
template<>
Value* Interpreter::calculate<Double>(Value* left, Value* right, int op) {
    Double* leftDouble = static_cast<Double*>(left);
    Double* rightDouble = static_cast<Double*>(right);
    
    Value* result = nullptr;
    switch (op) {
        case '+': result = new Double(leftDouble->getValue() + rightDouble->getValue()); break;
        case '-': result = new Double(leftDouble->getValue() - rightDouble->getValue()); break;
        case '*': result = new Double(leftDouble->getValue() * rightDouble->getValue()); break;
        case '/': 
            if (rightDouble->getValue() == 0.0) {
                // 不删除 left 和 right，因为它们可能还在其他地方被使用
                // delete left;
                // delete right;
                throw runtime_error("Division by zero");
            }
            result = new Double(leftDouble->getValue() / rightDouble->getValue()); 
            break;
        case EQ_EQ: result = new Bool(leftDouble->getValue() == rightDouble->getValue()); break;
        case NE_EQ: result = new Bool(leftDouble->getValue() != rightDouble->getValue()); break;
        case '<': result = new Bool(leftDouble->getValue() < rightDouble->getValue()); break;
        case '>': result = new Bool(leftDouble->getValue() > rightDouble->getValue()); break;
        case LE: result = new Bool(leftDouble->getValue() <= rightDouble->getValue()); break;
        case GE: result = new Bool(leftDouble->getValue() >= rightDouble->getValue()); break;
        default: throw runtime_error("Unsupported binary operation for Double");
    }
    
    // 不删除 left 和 right，因为它们可能还在其他地方被使用
    // delete left;
    // delete right;
    return result;
}

// Char类型的特化版本
template<>
Value* Interpreter::calculate<Char>(Value* left, Value* right, int op) {
    Char* leftChar = static_cast<Char*>(left);
    Char* rightChar = static_cast<Char*>(right);
    
    Value* result = nullptr;
    switch (op) {
        case '+': result = new Char(leftChar->getValue() + rightChar->getValue()); break;
        case '-': result = new Char(leftChar->getValue() - rightChar->getValue()); break;
        case '*': result = new Char(leftChar->getValue() * rightChar->getValue()); break;
        case '/': 
            if (rightChar->getValue() == 0) {
                // 不删除 left 和 right，因为它们可能还在其他地方被使用
                // delete left;
                // delete right;
                throw runtime_error("Division by zero");
            }
            result = new Char(leftChar->getValue() / rightChar->getValue()); 
            break;
        case EQ_EQ: result = new Bool(leftChar->getValue() == rightChar->getValue()); break;
        case NE_EQ: result = new Bool(leftChar->getValue() != rightChar->getValue()); break;
        case '<': result = new Bool(leftChar->getValue() < rightChar->getValue()); break;
        case '>': result = new Bool(leftChar->getValue() > rightChar->getValue()); break;
        case LE: result = new Bool(leftChar->getValue() <= rightChar->getValue()); break;
        case GE: result = new Bool(leftChar->getValue() >= rightChar->getValue()); break;
        default: throw runtime_error("Unsupported binary operation for Char");
    }
    
    // 不删除 left 和 right，因为它们可能还在其他地方被使用
    // delete left;
    // delete right;
    return result;
}

// String类型的特化版本
template<>
Value* Interpreter::calculate<String>(Value* left, Value* right, int op) {
    String* leftStr = static_cast<String*>(left);
    String* rightStr = static_cast<String*>(right);
    
    if (!leftStr || !rightStr) {
        throw runtime_error("Failed to evaluate operands");
    }
    
    String* result = nullptr;
    switch (op) {
        case '+': result = new String(leftStr->getValue() + rightStr->getValue()); break;
        default: throw runtime_error("Unsupported binary operation for String");
    }
    
    // 不删除 left 和 right，因为它们可能还在其他地方被使用
    // delete left;
    // delete right;
    return result;
}

// Bool类型的单目运算特化版本
template<>
Value* Interpreter::calculate<Bool>(Value* operand, int op) {
    Bool* boolOperand = static_cast<Bool*>(operand);
    
    Bool* result = nullptr;
    switch (op) {
        case '!': result = new Bool(!boolOperand->getValue()); break;
        default: throw runtime_error("Unsupported unary operation for Bool");
    }
    
    // 不删除 operand，因为它可能还在其他地方被使用
    // delete operand;
    return result;
}

// Integer类型的单目运算特化版本
template<>
Value* Interpreter::calculate<Integer>(Value* operand, int op) {
    Integer* intOperand = static_cast<Integer*>(operand);
    
    Integer* result = nullptr;
    switch (op) {
        case '+': result = new Integer(intOperand->getValue()); break;
        case '-': result = new Integer(-intOperand->getValue()); break;
        case '~': result = new Integer(~intOperand->getValue()); break;
        default: throw runtime_error("Unsupported unary operation for Integer");
    }
    
    // 不删除 operand，因为它可能还在其他地方被使用
    // delete operand;
    return result;
}

// Double类型的单目运算特化版本
template<>
Value* Interpreter::calculate<Double>(Value* operand, int op) {
    Double* doubleOperand = static_cast<Double*>(operand);
    
    Double* result = nullptr;
    switch (op) {
        case '+': result = new Double(doubleOperand->getValue()); break;
        case '-': result = new Double(-doubleOperand->getValue()); break;
        default: throw runtime_error("Unsupported unary operation for Double");
    }
    
    // 不删除 operand，因为它可能还在其他地方被使用
    // delete operand;
    return result;
}

// Char类型的单目运算特化版本
template<>
Value* Interpreter::calculate<Char>(Value* operand, int op) {
    Char* charOperand = static_cast<Char*>(operand);
    
    Char* result = nullptr;
    switch (op) {
        case '+': result = new Char(charOperand->getValue()); break;
        case '-': result = new Char(-charOperand->getValue()); break;
        case '~': result = new Char(~charOperand->getValue()); break;
        default: throw runtime_error("Unsupported unary operation for Char");
    }
    
    // 不删除 operand，因为它可能还在其他地方被使用
    // delete operand;
    return result;
}

// String类型的单目运算特化版本
template<>
Value* Interpreter::calculate<String>(Value* operand, int op) {
    String* stringOperand = static_cast<String*>(operand);
    
    String* result = nullptr;
    switch (op) {
        case '+': result = new String(stringOperand->getValue()); break;
        case '-': result = new String("-" + stringOperand->getValue()); break;
        default: throw runtime_error("Unsupported unary operation for String");
    }
    
    // 不删除 operand，因为它可能还在其他地方被使用
    // delete operand;
    return result;
}

// 辅助方法：计算并转换为指定类型
template<typename T>
Value* Interpreter::calculate_binary_casted(Value* left, Value* right, int opTag) {
    // 创建常量表达式 - 需要根据实际类型创建
    // 这里简化处理，直接进行类型转换
    Value* left_casted = nullptr;
    Value* right_casted = nullptr;
    
    // 根据类型T确定目标类型名称
    string targetTypeName;
    if (std::is_same<T, Integer>::value) targetTypeName = "int";
    else if (std::is_same<T, Double>::value) targetTypeName = "double";
    else if (std::is_same<T, Bool>::value) targetTypeName = "bool";
    else if (std::is_same<T, Char>::value) targetTypeName = "char";
    else if (std::is_same<T, String>::value) targetTypeName = "string";
    else targetTypeName = "unknown";
    
    // 使用CastExpression进行类型转换，实现计算逻辑与类型转换逻辑的解耦
    if (left) {
        // 创建一个包装表达式，将Value包装成Expression
        Expression* leftExpr = createExpressionFromValueImpl(left);
        CastExpression* left_cast = new CastExpression(leftExpr, targetTypeName);
        left_casted = visit(left_cast);
        delete left_cast;
        delete leftExpr;
    }
    
    if (right) {
        // 创建一个包装表达式，将Value包装成Expression
        Expression* rightExpr = createExpressionFromValueImpl(right);
        CastExpression* right_cast = new CastExpression(rightExpr, targetTypeName);
        right_casted = visit(right_cast);
        delete right_cast;
        delete rightExpr;
    }
    
    return calculate<T>(left_casted, right_casted, opTag);
}

// 二元操作的兼容性计算方法
Value* Interpreter::calculate_binary_compatible(Value* left, Value* right, int opTag) {
    // 位运算：统一转换为整数类型
    if (opTag == '&' || opTag == '|' || opTag == '^' || 
        opTag == BIT_AND || opTag == BIT_OR || opTag == BIT_XOR || 
        opTag == LEFT_SHIFT || opTag == RIGHT_SHIFT) {
        return calculate_binary_casted<Integer>(left, right, opTag);
    }

    // 逻辑运算：统一转换为布尔类型
    if (opTag == AND_AND || opTag == OR_OR) {
        return calculate_binary_casted<Bool>(left, right, opTag);
    }
    
    // 字符串类型：如果任一操作数是字符串，则返回string
    // 数值类型转换：优先级 double > int > char > bool
    if ((dynamic_cast<String*>(left) || dynamic_cast<String*>(right))) {
        return calculate_binary_casted<String>(left, right, opTag);
    } else if (dynamic_cast<Double*>(left) || dynamic_cast<Double*>(right)) {
        return calculate_binary_casted<Double>(left, right, opTag);
    } else if (dynamic_cast<Integer*>(left) || dynamic_cast<Integer*>(right)) {
        return calculate_binary_casted<Integer>(left, right, opTag);
    } else if (dynamic_cast<Char*>(left) || dynamic_cast<Char*>(right)) {
        // 对于字符运算，根据操作符类型决定返回类型
        if (opTag == '+') {
            return calculate_binary_casted<Char>(left, right, opTag);
        } else {
            return calculate_binary_casted<Integer>(left, right, opTag);
        }
    } else if (dynamic_cast<Bool*>(left) || dynamic_cast<Bool*>(right)) {
        return calculate_binary_casted<Bool>(left, right, opTag);
    }

    return nullptr;
}

// 辅助方法：计算并转换为指定类型（一元操作）
template<typename T>
Value* Interpreter::calculate_unary_casted(Value* value, int opTag) {
    // 创建常量表达式 - 需要根据实际类型创建
    // 这里简化处理，直接进行类型转换
    Value* operand_casted = nullptr;
    
    // 根据类型T确定目标类型名称
    string targetTypeName;
    if (std::is_same<T, Integer>::value) targetTypeName = "int";
    else if (std::is_same<T, Double>::value) targetTypeName = "double";
    else if (std::is_same<T, Bool>::value) targetTypeName = "bool";
    else if (std::is_same<T, Char>::value) targetTypeName = "char";
    else if (std::is_same<T, String>::value) targetTypeName = "string";
    else targetTypeName = "unknown";
    
    // 使用CastExpression进行类型转换，实现计算逻辑与类型转换逻辑的解耦
    Expression* operandExpr = createExpressionFromValueImpl(value);
    if (operandExpr) {
        CastExpression* operand_cast = new CastExpression(operandExpr, targetTypeName);
        operand_casted = visit(operand_cast);
        delete operand_cast;
        delete operandExpr;
    } else {
        operand_casted = value; // 如果无法创建表达式，保持原值
    }
    return calculate<T>(operand_casted, opTag);
}

// 一元操作的兼容性计算方法
Value* Interpreter::calculate_unary_compatible(Value* value, int opTag) {
    // 逻辑非运算：转换为布尔类型
    if (opTag == '!') {
        return calculate_unary_casted<Bool>(value, opTag);
    }
    
    // 位运算：转换为整数类型
    if (opTag == '~') {
        return calculate_unary_casted<Integer>(value, opTag);
    }

    // 正负号运算：根据操作数类型决定
    if (opTag == '+' || opTag == '-') {
        if (dynamic_cast<String*>(value)) {
            return calculate_unary_casted<String>(value, opTag);
        } else if (dynamic_cast<Double*>(value)) {
            return calculate_unary_casted<Double>(value, opTag);
        } else if (dynamic_cast<Integer*>(value)) {
            return calculate_unary_casted<Integer>(value, opTag);
        } else if (dynamic_cast<Char*>(value)) {
            return calculate_unary_casted<Char>(value, opTag);
        } else if (dynamic_cast<Bool*>(value)) {
            return calculate_unary_casted<Bool>(value, opTag);
        }
    }
    
    return nullptr;
}

// 辅助方法：计算并转换为指定类型（赋值操作）
template<typename T>
Value* Interpreter::calculate_assign_casted(Value* left, Value* right, int opTag) {
    // 对于赋值操作，我们需要将右操作数转换为左操作数的类型
    // 创建常量表达式 - 需要根据实际类型创建
    // 这里简化处理，直接进行类型转换
    Value* right_casted = nullptr;
    
    // 根据类型T确定目标类型名称
    string targetTypeName;
    if (std::is_same<T, Integer>::value) targetTypeName = "int";
    else if (std::is_same<T, Double>::value) targetTypeName = "double";
    else if (std::is_same<T, Bool>::value) targetTypeName = "bool";
    else if (std::is_same<T, Char>::value) targetTypeName = "char";
    else if (std::is_same<T, String>::value) targetTypeName = "string";
    else targetTypeName = "unknown";
    
    // 使用CastExpression进行类型转换，实现计算逻辑与类型转换逻辑的解耦
    Expression* rightExpr = createExpressionFromValueImpl(right);
    if (rightExpr) {
        CastExpression* right_cast = new CastExpression(rightExpr, targetTypeName);
        right_casted = visit(right_cast);
        delete right_cast;
        delete rightExpr;
    } else {
        right_casted = right; // 如果无法创建表达式，保持原值
    }
    return right_casted;
}

// 赋值操作的兼容性计算方法
Value* Interpreter::calculate_assign_compatible(Value* left, Value* right, int opTag) {
    // 根据左操作数的类型进行转换
    if (dynamic_cast<String*>(left)) {
        return calculate_assign_casted<String>(left, right, opTag);
    } else if (dynamic_cast<Double*>(left)) {
        return calculate_assign_casted<Double>(left, right, opTag);
    } else if (dynamic_cast<Integer*>(left)) {
        return calculate_assign_casted<Integer>(left, right, opTag);
    } else if (dynamic_cast<Char*>(left)) {
        return calculate_assign_casted<Char>(left, right, opTag);
    } else if (dynamic_cast<Bool*>(left)) {
        return calculate_assign_casted<Bool>(left, right, opTag);
    }
    
    return nullptr;
}

// ==================== 辅助方法 ====================

// 将Value包装成Expression的非模板接口方法
Expression* Interpreter::createExpressionFromValueImpl(Value* value) {
    if (!value) return nullptr;
    
    // 根据Value的类型创建对应的ConstantExpression
    if (Integer* intVal = dynamic_cast<Integer*>(value)) {
        return new ConstantExpression<int>(intVal->getValue());
    } else if (Double* doubleVal = dynamic_cast<Double*>(value)) {
        return new ConstantExpression<double>(doubleVal->getValue());
    } else if (Bool* boolVal = dynamic_cast<Bool*>(value)) {
        return new ConstantExpression<bool>(boolVal->getValue());
    } else if (Char* charVal = dynamic_cast<Char*>(value)) {
        return new ConstantExpression<char>(charVal->getValue());
    } else if (String* stringVal = dynamic_cast<String*>(value)) {
        return new ConstantExpression<string>(stringVal->getValue());
    }
    
    // 如果无法识别类型，返回nullptr
    return nullptr;
}

// 将Value包装成Expression的模板辅助方法（内部实现）
template<typename T>
Expression* Interpreter::createExpressionFromValue(Value* value) {
    if (!value) return nullptr;
    
    // 尝试动态转换为目标类型
    T* typedValue = dynamic_cast<T*>(value);
    if (typedValue) {
        return new ConstantExpression<typename T::value_type>(typedValue->getValue());
    }
    
    return nullptr;
}

// 特化版本：处理不同类型的Value
template<>
Expression* Interpreter::createExpressionFromValue<Integer>(Value* value) {
    if (!value) return nullptr;
    
    Integer* intVal = dynamic_cast<Integer*>(value);
    if (intVal) {
        return new ConstantExpression<int>(intVal->getValue());
    }
    return nullptr;
}

template<>
Expression* Interpreter::createExpressionFromValue<Double>(Value* value) {
    if (!value) return nullptr;
    
    Double* doubleVal = dynamic_cast<Double*>(value);
    if (doubleVal) {
        return new ConstantExpression<double>(doubleVal->getValue());
    }
    return nullptr;
}

template<>
Expression* Interpreter::createExpressionFromValue<Bool>(Value* value) {
    if (!value) return nullptr;
    
    Bool* boolVal = dynamic_cast<Bool*>(value);
    if (boolVal) {
        return new ConstantExpression<bool>(boolVal->getValue());
    }
    return nullptr;
}

template<>
Expression* Interpreter::createExpressionFromValue<Char>(Value* value) {
    if (!value) return nullptr;
    
    Char* charVal = dynamic_cast<Char*>(value);
    if (charVal) {
        return new ConstantExpression<char>(charVal->getValue());
    }
    return nullptr;
}

template<>
Expression* Interpreter::createExpressionFromValue<String>(Value* value) {
    if (!value) return nullptr;
    
    String* stringVal = dynamic_cast<String*>(value);
    if (stringVal) {
        return new ConstantExpression<string>(stringVal->getValue());
    }
    return nullptr;
}

// ==================== CastExpression访问方法 ====================

// CastExpression访问方法 - 完全使用类型系统进行转换
Value* Interpreter::visit(CastExpression* castExpr) {
    if (!castExpr) return nullptr;
    
    // 求值操作数
    Value* operandValue = visit(castExpr->operand);
    if (!operandValue) return nullptr;
    
    // 获取目标类型
    ObjectType* targetType = getTypeRegistry()->getType(castExpr->getTargetTypeName());
    if (!targetType) {
        reportError("Unknown target type: " + castExpr->getTargetTypeName());
        return nullptr;
    }
    
    // 使用类型系统进行转换
    Value* result = targetType->convertTo(targetType, operandValue);
    
    if (!result) {
        reportError("Type conversion failed from " + getValueTypeName(operandValue) + 
                   " to " + castExpr->getTargetTypeName());
        return nullptr;
    }
    
    return result;
}
