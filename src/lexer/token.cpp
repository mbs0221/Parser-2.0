#include "lexer/token.h"
#include <mutex>

// 静态成员初始化
// Token静态成员定义 - 直接初始化
Token* Token::END_OF_FILE = new Token(::END_OF_FILE);

// 静态成员定义 - 直接初始化
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