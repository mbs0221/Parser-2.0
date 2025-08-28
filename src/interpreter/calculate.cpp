#include "interpreter/interpreter.h"
#include "lexer/value.h"
#include "parser/expression.h"

using namespace std;

// ==================== 模板特化实现 ====================

// Bool类型的特化版本
template<>
Bool* Interpreter::calculate<Bool>(Value* left, Value* right, int op) {
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
Integer* Interpreter::calculate<Integer>(Value* left, Value* right, int op) {
    Integer* leftInt = static_cast<Integer*>(left);
    Integer* rightInt = static_cast<Integer*>(right);
    
    Integer* result = nullptr;
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
        default: throw runtime_error("Unsupported binary operation for Integer");
    }
    
    // 不删除 left 和 right，因为它们可能还在其他地方被使用
    // delete left;
    // delete right;
    return result;
}

// Double类型的特化版本
template<>
Double* Interpreter::calculate<Double>(Value* left, Value* right, int op) {
    Double* leftDouble = static_cast<Double*>(left);
    Double* rightDouble = static_cast<Double*>(right);
    
    Double* result = nullptr;
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
        default: throw runtime_error("Unsupported binary operation for Double");
    }
    
    // 不删除 left 和 right，因为它们可能还在其他地方被使用
    // delete left;
    // delete right;
    return result;
}

// Char类型的特化版本
template<>
Char* Interpreter::calculate<Char>(Value* left, Value* right, int op) {
    Char* leftChar = static_cast<Char*>(left);
    Char* rightChar = static_cast<Char*>(right);
    
    Char* result = nullptr;
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
        default: throw runtime_error("Unsupported binary operation for Char");
    }
    
    // 不删除 left 和 right，因为它们可能还在其他地方被使用
    // delete left;
    // delete right;
    return result;
}

// String类型的特化版本
template<>
String* Interpreter::calculate<String>(Value* left, Value* right, int op) {
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
Bool* Interpreter::calculate<Bool>(Value* operand, int op) {
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
Integer* Interpreter::calculate<Integer>(Value* operand, int op) {
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
Double* Interpreter::calculate<Double>(Value* operand, int op) {
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
Char* Interpreter::calculate<Char>(Value* operand, int op) {
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
String* Interpreter::calculate<String>(Value* operand, int op) {
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
    ConstantExpression* left_constant = new ConstantExpression(left);
    ConstantExpression* right_constant = new ConstantExpression(right);
    Value* left_casted = visit(new CastExpression<T>(left_constant));
    Value* right_casted = visit(new CastExpression<T>(right_constant));
    // 不删除 ConstantExpression 对象，让它们自然销毁
    // delete left_constant;
    // delete right_constant;
    return calculate<T>(left_casted, right_casted, opTag);
}

// 二元操作的兼容性计算方法
Value* Interpreter::calculate_binary_compatible(Value* left, Value* right, int opTag) {
    // 逻辑和比较运算：统一转换为布尔类型
    if (opTag == AND_AND || opTag == OR_OR || opTag == EQ_EQ || opTag == NE_EQ || opTag == '<' || opTag == '>' || opTag == LT || opTag == GT || opTag == LE || opTag == GE) {
        return calculate_binary_casted<Bool>(left, right, opTag);
    }
    
    // 位运算：统一转换为整数类型
    if (opTag == '&' || opTag == '|' || opTag == '^' || 
        opTag == BIT_AND || opTag == BIT_OR || opTag == BIT_XOR || 
        opTag == LEFT_SHIFT || opTag == RIGHT_SHIFT) {
        return calculate_binary_casted<Integer>(left, right, opTag);
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
    ConstantExpression* constant_operand = new ConstantExpression(value);
    Value* operand_casted = visit(new CastExpression<T>(constant_operand));
    // 不删除 ConstantExpression 对象，让它们自然销毁
    // delete constant_operand; 
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
    ConstantExpression* right_constant = new ConstantExpression(right);
    Value* right_casted = visit(new CastExpression<T>(right_constant));
    // 不删除 ConstantExpression 对象，让它们自然销毁
    // delete right_constant;
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

// ==================== CastExpression访问方法 ====================

// CastExpression<Integer>访问方法
Value* Interpreter::visit(CastExpression<Integer>* castExpr) {
    if (!castExpr) return nullptr;
    
    // 求值操作数
    Value* operandValue = visit(castExpr->operand);
    if (!operandValue) return nullptr;
    
    // 转换为Integer类型
    Value* result = operandValue->convert<Integer>();
    
    // 不删除操作数值，因为它可能还在其他地方被使用
    // delete operandValue;
    
    return result;
}

// CastExpression<Double>访问方法
Value* Interpreter::visit(CastExpression<Double>* castExpr) {
    if (!castExpr) return nullptr;
    
    // 求值操作数
    Value* operandValue = visit(castExpr->operand);
    if (!operandValue) return nullptr;
    
    // 转换为Double类型
    Value* result = operandValue->convert<Double>();
    
    // 不删除操作数值，因为它可能还在其他地方被使用
    // delete operandValue;
    
    return result;
}

// CastExpression<Bool>访问方法
Value* Interpreter::visit(CastExpression<Bool>* castExpr) {
    if (!castExpr) return nullptr;
    
    // 求值操作数
    Value* operandValue = visit(castExpr->operand);
    if (!operandValue) return nullptr;
    
    // 转换为Bool类型
    Value* result = operandValue->convert<Bool>();
    
    // 不删除操作数值，因为它可能还在其他地方被使用
    // delete operandValue;
    
    return result;
}

// CastExpression<Char>访问方法
Value* Interpreter::visit(CastExpression<Char>* castExpr) {
    if (!castExpr) return nullptr;
    
    // 求值操作数
    Value* operandValue = visit(castExpr->operand);
    if (!operandValue) return nullptr;
    
    // 转换为Char类型
    Value* result = operandValue->convert<Char>();
    
    // 不删除操作数值，因为它可能还在其他地方被使用
    // delete operandValue;
    
    return result;
}

// CastExpression<String>访问方法
Value* Interpreter::visit(CastExpression<String>* castExpr) {
    if (!castExpr) return nullptr;
    
    // 求值操作数
    Value* operandValue = visit(castExpr->operand);
    if (!operandValue) return nullptr;
    
    // 转换为String类型
    Value* result = operandValue->convert<String>();
    
    // 不删除操作数值，因为它可能还在其他地方被使用
    // delete operandValue;
    
    return result;
}
