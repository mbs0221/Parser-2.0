#include "interpreter/interpreter.h"
#include "lexer/value.h"
#include "parser/expression.h"

using namespace std;

// ==================== 模板特化实现 ====================

// Bool类型的特化版本
template<>
Bool* Interpreter::calculate<Bool>(Expression* left, Expression* right, int op) {
    Value* leftValue = visit(left);
    Value* rightValue = visit(right);
    
    if (!leftValue || !rightValue) {
        throw runtime_error("Failed to evaluate operands");
    }
    
    Bool* leftBool = static_cast<Bool*>(leftValue);
    Bool* rightBool = static_cast<Bool*>(rightValue);
    
    Bool* result = nullptr;
    switch (op) {
        case AND_AND: result = new Bool(leftBool->getValue() && rightBool->getValue()); break;
        case OR_OR: result = new Bool(leftBool->getValue() || rightBool->getValue()); break;
        case EQ_EQ: result = new Bool(leftBool->getValue() == rightBool->getValue()); break;
        case NE_EQ: result = new Bool(leftBool->getValue() != rightBool->getValue()); break;
        default: throw runtime_error("Unsupported binary operation for Bool");
    }
    
    delete leftValue;
    delete rightValue;
    return result;
}

// Integer类型的特化版本
template<>
Integer* Interpreter::calculate<Integer>(Expression* left, Expression* right, int op) {
    Value* leftValue = visit(left);
    Value* rightValue = visit(right);
    
    if (!leftValue || !rightValue) {
        throw runtime_error("Failed to evaluate operands");
    }
    
    Integer* leftInt = static_cast<Integer*>(leftValue);
    Integer* rightInt = static_cast<Integer*>(rightValue);
    
    Integer* result = nullptr;
    switch (op) {
        case '+': result = new Integer(leftInt->getValue() + rightInt->getValue()); break;
        case '-': result = new Integer(leftInt->getValue() - rightInt->getValue()); break;
        case '*': result = new Integer(leftInt->getValue() * rightInt->getValue()); break;
        case '/': 
            if (rightInt->getValue() == 0) {
                delete leftValue;
                delete rightValue;
                throw runtime_error("Division by zero");
            }
            result = new Integer(leftInt->getValue() / rightInt->getValue()); 
            break;
        case '%': 
            if (rightInt->getValue() == 0) {
                delete leftValue;
                delete rightValue;
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
    
    delete leftValue;
    delete rightValue;
    return result;
}

// Double类型的特化版本
template<>
Double* Interpreter::calculate<Double>(Expression* left, Expression* right, int op) {
    Value* leftValue = visit(left);
    Value* rightValue = visit(right);
    
    if (!leftValue || !rightValue) {
        throw runtime_error("Failed to evaluate operands");
    }
    
    Double* leftDouble = static_cast<Double*>(leftValue);
    Double* rightDouble = static_cast<Double*>(rightValue);
    
    Double* result = nullptr;
    switch (op) {
        case '+': result = new Double(leftDouble->getValue() + rightDouble->getValue()); break;
        case '-': result = new Double(leftDouble->getValue() - rightDouble->getValue()); break;
        case '*': result = new Double(leftDouble->getValue() * rightDouble->getValue()); break;
        case '/': 
            if (rightDouble->getValue() == 0.0) {
                delete leftValue;
                delete rightValue;
                throw runtime_error("Division by zero");
            }
            result = new Double(leftDouble->getValue() / rightDouble->getValue()); 
            break;
        default: throw runtime_error("Unsupported binary operation for Double");
    }
    
    delete leftValue;
    delete rightValue;
    return result;
}

// Char类型的特化版本
template<>
Char* Interpreter::calculate<Char>(Expression* left, Expression* right, int op) {
    Value* leftValue = visit(left);
    Value* rightValue = visit(right);
    
    if (!leftValue || !rightValue) {
        throw runtime_error("Failed to evaluate operands");
    }
    
    Char* leftChar = static_cast<Char*>(leftValue);
    Char* rightChar = static_cast<Char*>(rightValue);
    
    Char* result = nullptr;
    switch (op) {
        case '+': result = new Char(leftChar->getValue() + rightChar->getValue()); break;
        case '-': result = new Char(leftChar->getValue() - rightChar->getValue()); break;
        case '*': result = new Char(leftChar->getValue() * rightChar->getValue()); break;
        case '/': 
            if (rightChar->getValue() == 0) {
                delete leftValue;
                delete rightValue;
                throw runtime_error("Division by zero");
            }
            result = new Char(leftChar->getValue() / rightChar->getValue()); 
            break;
        default: throw runtime_error("Unsupported binary operation for Char");
    }
    
    delete leftValue;
    delete rightValue;
    return result;
}

// String类型的特化版本
template<>
String* Interpreter::calculate<String>(Expression* left, Expression* right, int op) {
    Value* leftValue = visit(left);
    Value* rightValue = visit(right);
    
    if (!leftValue || !rightValue) {
        throw runtime_error("Failed to evaluate operands");
    }
    
    String* leftStr = static_cast<String*>(leftValue);
    String* rightStr = static_cast<String*>(rightValue);
    
    String* result = nullptr;
    switch (op) {
        case '+': result = new String(leftStr->getValue() + rightStr->getValue()); break;
        default: throw runtime_error("Unsupported binary operation for String");
    }
    
    delete leftValue;
    delete rightValue;
    return result;
}

// Bool类型的单目运算特化版本
template<>
Bool* Interpreter::calculate<Bool>(Expression* operand, int op) {
    Value* operandValue = visit(operand);
    
    if (!operandValue) {
        throw runtime_error("Failed to evaluate operand");
    }
    
    Bool* boolOperand = static_cast<Bool*>(operandValue);
    
    Bool* result = nullptr;
    switch (op) {
        case '!': result = new Bool(!boolOperand->getValue()); break;
        default: throw runtime_error("Unsupported unary operation for Bool");
    }
    
    delete operandValue;
    return result;
}

// Integer类型的单目运算特化版本
template<>
Integer* Interpreter::calculate<Integer>(Expression* operand, int op) {
    Value* operandValue = visit(operand);
    
    if (!operandValue) {
        throw runtime_error("Failed to evaluate operand");
    }
    
    Integer* intOperand = static_cast<Integer*>(operandValue);
    
    Integer* result = nullptr;
    switch (op) {
        case '+': result = new Integer(intOperand->getValue()); break;
        case '-': result = new Integer(-intOperand->getValue()); break;
        case '~': result = new Integer(~intOperand->getValue()); break;
        default: throw runtime_error("Unsupported unary operation for Integer");
    }
    
    delete operandValue;
    return result;
}

// Double类型的单目运算特化版本
template<>
Double* Interpreter::calculate<Double>(Expression* operand, int op) {
    Value* operandValue = visit(operand);
    
    if (!operandValue) {
        throw runtime_error("Failed to evaluate operand");
    }
    
    Double* doubleOperand = static_cast<Double*>(operandValue);
    
    Double* result = nullptr;
    switch (op) {
        case '+': result = new Double(doubleOperand->getValue()); break;
        case '-': result = new Double(-doubleOperand->getValue()); break;
        default: throw runtime_error("Unsupported unary operation for Double");
    }
    
    delete operandValue;
    return result;
}

// Char类型的单目运算特化版本
template<>
Char* Interpreter::calculate<Char>(Expression* operand, int op) {
    Value* operandValue = visit(operand);
    
    if (!operandValue) {
        throw runtime_error("Failed to evaluate operand");
    }
    
    Char* charOperand = static_cast<Char*>(operandValue);
    
    Char* result = nullptr;
    switch (op) {
        case '+': result = new Char(charOperand->getValue()); break;
        case '-': result = new Char(-charOperand->getValue()); break;
        case '~': result = new Char(~charOperand->getValue()); break;
        default: throw runtime_error("Unsupported unary operation for Char");
    }
    
    delete operandValue;
    return result;
}
