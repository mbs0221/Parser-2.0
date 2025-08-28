#include "lexer/value.h"

// 静态成员定义
Type *Type::Int = new Type(NUM, "int", 4);
Type *Type::Double = new Type(REAL, "double", 8);
Type *Type::Char = new Type(CHAR, "char", 1);
Type *Type::Float = new Type(REAL, "float", 8);
Type *Type::Bool = new Type(BOOL, "bool", 1);
Type *Type::String = new Type(STR, "string", 0);

// 赋值运算符
Operator *Operator::Assign = new Operator('=', "=", 2, false);

// 算术运算符
Operator *Operator::Add = new Operator('+', "+", 4, true);
Operator *Operator::Sub = new Operator('-', "-", 4, true);
Operator *Operator::Mul = new Operator('*', "*", 5, true);
Operator *Operator::Div = new Operator('/', "/", 5, true);
Operator *Operator::Mod = new Operator('%', "%", 5, true);

// 比较运算符
Operator *Operator::LT = new Operator('<', "<", 3, true);
Operator *Operator::GT = new Operator('>', ">", 3, true);
Operator *Operator::LE = new Operator(Tag::LE, "<=", 3, true);
Operator *Operator::GE = new Operator(Tag::GE, ">=", 3, true);
Operator *Operator::EQ = new Operator(Tag::EQ_EQ, "==", 3, true);
Operator *Operator::NE = new Operator(Tag::NE_EQ, "!=", 3, true);

// 逻辑运算符
Operator *Operator::AND = new Operator(AND_AND, "&&", 2, true);
Operator *Operator::OR = new Operator(OR_OR, "||", 1, true);
Operator *Operator::Not = new Operator('!', "!", 6, false);

// 位运算符
Operator *Operator::BitAnd = new Operator('&', "&", 4, true);
Operator *Operator::BitOr = new Operator('|', "|", 2, true);
Operator *Operator::BitXor = new Operator('^', "^", 3, true);
Operator *Operator::BitNot = new Operator('~', "~", 6, false);
Operator *Operator::LeftShift = new Operator(LEFT_SHIFT, "<<", 5, true);
Operator *Operator::RightShift = new Operator(RIGHT_SHIFT, ">>", 5, true);

// 其他运算符
Operator *Operator::Increment = new Operator(INCREMENT, "++", 6, false);
Operator *Operator::Decrement = new Operator(DECREMENT, "--", 6, false);
Operator *Operator::Dot = new Operator('.', ".", 7, true);
Operator *Operator::Arrow = new Operator(ARROW, "->", 7, true);
Operator *Operator::Question = new Operator('?', "?", 1, false);
Operator *Operator::Colon = new Operator(':', ":", 1, false);
// NULL_VALUE 应该是 Value 类型，不应该是 Operator

// 标点符号不是运算符，在词法分析器中直接返回Token类型

// 访问修饰符
Visibility *Visibility::Public = new Visibility(PUBLIC, VIS_PUBLIC);
Visibility *Visibility::Private = new Visibility(PRIVATE, VIS_PRIVATE);
Visibility *Visibility::Protected = new Visibility(PROTECTED, VIS_PROTECTED);

Bool *Bool::True = new Bool(true);
Bool *Bool::False = new Bool(false);

// 空值 - 使用Integer(0)作为空值表示
Value *Value::NullValue = new Integer(0);

// ==================== getTypeName方法实现 ====================

string Bool::getTypeName() const {
    return "bool";
}

string Integer::getTypeName() const {
    return "int";
}

string Char::getTypeName() const {
    return "char";
}

string Double::getTypeName() const {
    return "double";
}

string String::getTypeName() const {
    return "string";
}

string Array::getTypeName() const {
    return "array";
}

string Dict::getTypeName() const {
    return "dict";
}

// ==================== convertTo方法实现 ====================

// Bool类型的convertTo实现
Value* Bool::convertTo(Type* type) {
    if (type == Type::Int) {
        return new Integer(value ? 1 : 0);
    } else if (type == Type::Double) {
        return new Double(value ? 1.0 : 0.0);
    } else if (type == Type::Char) {
        return new Char(value ? '1' : '0');
    } else if (type == Type::String) {
        return new String(value ? "true" : "false");
    } else if (type == Type::Bool) {
        return new Bool(value);
    } else {
        throw runtime_error("Cannot convert Bool to type: " + type->word);
    }
}

// Integer类型的convertTo实现
Value* Integer::convertTo(Type* type) {
    if (type == Type::Int) {
        return new Integer(value);
    } else if (type == Type::Double) {
        return new Double(static_cast<double>(value));
    } else if (type == Type::Char) {
        return new Char(static_cast<char>(value));
    } else if (type == Type::String) {
        return new String(to_string(value));
    } else if (type == Type::Bool) {
        return new Bool(value != 0);
    } else {
        throw runtime_error("Cannot convert Integer to type: " + type->word);
    }
}

// Char类型的convertTo实现
Value* Char::convertTo(Type* type) {
    if (type == Type::Int) {
        return new Integer(static_cast<int>(value));
    } else if (type == Type::Double) {
        return new Double(static_cast<double>(value));
    } else if (type == Type::Char) {
        return new Char(value);
    } else if (type == Type::String) {
        return new String(string(1, value));
    } else if (type == Type::Bool) {
        return new Bool(value != '\0');
    } else {
        throw runtime_error("Cannot convert Char to type: " + type->word);
    }
}

// Double类型的convertTo实现
Value* Double::convertTo(Type* type) {
    if (type == Type::Int) {
        return new Integer(static_cast<int>(value));
    } else if (type == Type::Double) {
        return new Double(value);
    } else if (type == Type::Char) {
        return new Char(static_cast<char>(value));
    } else if (type == Type::String) {
        ostringstream oss;
        oss << value;
        return new String(oss.str());
    } else if (type == Type::Bool) {
        return new Bool(value != 0.0);
    } else {
        throw runtime_error("Cannot convert Double to type: " + type->word);
    }
}

// String类型的convertTo实现
Value* String::convertTo(Type* type) {
    if (type == Type::Int) {
        try {
            return new Integer(stoi(value));
        } catch (const exception&) {
            throw runtime_error("Cannot convert string '" + value + "' to Integer");
        }
    } else if (type == Type::Double) {
        try {
            return new Double(stod(value));
        } catch (const exception&) {
            throw runtime_error("Cannot convert string '" + value + "' to Double");
        }
    } else if (type == Type::Char) {
        if (value.length() == 1) {
            return new Char(value[0]);
        } else {
            throw runtime_error("Cannot convert string '" + value + "' to Char (length != 1)");
        }
    } else if (type == Type::String) {
        return new String(value);
    } else if (type == Type::Bool) {
        // 尝试解析布尔值
        string lowerValue = value;
        transform(lowerValue.begin(), lowerValue.end(), lowerValue.begin(), ::tolower);
        if (lowerValue == "true" || lowerValue == "1") {
            return new Bool(true);
        } else if (lowerValue == "false" || lowerValue == "0") {
            return new Bool(false);
        } else {
            throw runtime_error("Cannot convert string '" + value + "' to Bool");
        }
    } else {
        throw runtime_error("Cannot convert String to type: " + type->word);
    }
}

// Array类型的convertTo实现
Value* Array::convertTo(Type* type) {
    if (type == Type::String) {
        return new String(toString());
    } else if (type == valueType) {
        return new Array(*this);
    } else {
        throw runtime_error("Cannot convert Array to type: " + type->word);
    }
}

// Dict类型的convertTo实现
Value* Dict::convertTo(Type* type) {
    if (type == Type::String) {
        return new String(toString());
    } else if (type == valueType) {
        return new Dict(*this);
    } else {
        throw runtime_error("Cannot convert Dict to type: " + type->word);
    }
}
