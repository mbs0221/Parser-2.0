#include "lexer/value.h"

// 静态成员定义
Type *Type::Int = new Type(NUM, "int", 4);
Type *Type::Double = new Type(REAL, "double", 8);
Type *Type::Char = new Type(CHAR, "char", 1);
Type *Type::Float = new Type(REAL, "float", 8);
Type *Type::Bool = new Type(BOOL, "bool", 1);
Type *Type::String = new Type(STR, "string", 0);

// 算术运算符
Operator *Operator::Add = new Operator(PLUS, "+", 4, true);
Operator *Operator::Sub = new Operator(MINUS, "-", 4, true);
Operator *Operator::Mul = new Operator(MULTIPLY, "*", 5, true);
Operator *Operator::Div = new Operator(DIVIDE, "/", 5, true);
Operator *Operator::Mod = new Operator(MODULO, "%", 5, true);

// 比较运算符
Operator *Operator::LT = new Operator(Tag::LT, "<", 3, true);
Operator *Operator::GT = new Operator(Tag::GT, ">", 3, true);
Operator *Operator::LE = new Operator(Tag::LE, "<=", 3, true);
Operator *Operator::GE = new Operator(Tag::GE_EQ, ">=", 3, true);
Operator *Operator::EQ = new Operator(Tag::EQ_EQ, "==", 3, true);
Operator *Operator::NE = new Operator(Tag::NE_EQ, "!=", 3, true);

// 逻辑运算符
Operator *Operator::AND = new Operator(AND_AND, "&&", 2, true);
Operator *Operator::OR = new Operator(OR_OR, "||", 1, true);
Operator *Operator::Not = new Operator(NOT, "!", 6, false);

// 位运算符
Operator *Operator::BitAnd = new Operator(BIT_AND, "&", 4, true);
Operator *Operator::BitOr = new Operator(BIT_OR, "|", 2, true);
Operator *Operator::BitXor = new Operator(BIT_XOR, "^", 3, true);
Operator *Operator::BitNot = new Operator(BIT_NOT, "~", 6, false);
Operator *Operator::LeftShift = new Operator(LEFT_SHIFT, "<<", 5, true);
Operator *Operator::RightShift = new Operator(RIGHT_SHIFT, ">>", 5, true);

// 其他运算符
Operator *Operator::Increment = new Operator(INCREMENT, "++", 6, false);
Operator *Operator::Decrement = new Operator(DECREMENT, "--", 6, false);
Operator *Operator::Dot = new Operator(DOT, ".", 7, true);
Operator *Operator::Arrow = new Operator(ARROW, "->", 7, true);
Operator *Operator::Question = new Operator(QUESTION, "?", 1, false);
Operator *Operator::Colon = new Operator(COLON, ":", 1, false);
Operator *Operator::NullValue = new Operator(NULL_VALUE, "null", 0, true);

Bool *Bool::True = new Bool(true);
Bool *Bool::False = new Bool(false);
