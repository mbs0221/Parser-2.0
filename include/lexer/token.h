#ifndef TOKEN_H
#define TOKEN_H


// 包含必要的头文件
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <stdexcept>
#include <sstream>
#include <iostream>
#include <fstream>
#include <list>
#include <stack>
#include <iomanip>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include <algorithm>
#include <mutex>

using namespace std;

namespace lexer {

//词法分析器标记
enum Tag{
	// 基本类型
	BASIC = 256, ID, NUM, REAL, DOUBLE, FLOAT, CHAR, STR, BOOL, ARRAY, DICT, END_OF_FILE,
	
	// 关键字
	IF, THEN, ELSE, DO, WHILE, FOR, CASE, DEFAULT,
	LET, BREAK, CONTINUE, RETURN, THROW, TRY, CATCH, FINALLY, SWITCH,
	BEGIN, END, FUNCTION, LAMBDA,
	STRUCT, CLASS, PUBLIC, PRIVATE, PROTECTED, IMPORT,
	
	// 注释
	COMMENT,
	
	// 双字符运算符范围：300-399
	// 赋值运算符
	PLUS_ASSIGN = 300, MINUS_ASSIGN, MULTIPLY_ASSIGN, DIVIDE_ASSIGN, MODULO_ASSIGN,
	
	// 比较运算符
	LE, GE, EQ_EQ, NE_EQ,
	
	// 逻辑运算符
	AND_AND, OR_OR,
	
	// 位运算符
	LEFT_SHIFT = 1001, RIGHT_SHIFT = 1002,
	
	// 其他双字符运算符
	INCREMENT, DECREMENT, ARROW, TERNARY_OP,
	
	// 单字符运算符使用ASCII码（保持原有ASCII值）
	// 这些值直接对应ASCII码，不需要重新定义
	// '+' = 43, '-' = 45, '*' = 42, '/' = 47, '%' = 37
	// '&' = 38, '|' = 124, '^' = 94, '<' = 60, '>' = 62
	// '~' = 126, '!' = 33, '=' = 61, '.' = 46, ',' = 44
	// ';' = 59, ':' = 58, '(' = 40, ')' = 41, '[' = 91
	// ']' = 93, '{' = 123, '}' = 125
	
	// 特殊值
	NULL_VALUE
};

//词法单元
struct Token{
	int Tag;		//标记
	static Token* END_OF_FILE;  // 静态END_OF_FILE Token
	
	Token(){
		Tag = 0;
	}
	Token(int Tag) :Tag(Tag){  }
	
	// 获取字面值的字符串表示
	virtual string str() const {
		if (Tag >= 32 && Tag <= 126) {
			return string(1, (char)Tag);
		}
		return "token(" + to_string(Tag) + ")";
	}
};

struct Word :Token{
	string word;	//字符串
	Word(){
		this->Tag = 0;
		this->word = "";
	}
	Word(int Tag, string word) :Token(Tag){
		this->word = word;
	}
	virtual Word *eval(){
		return this;
	}
	
	// 获取字面值的字符串表示
	string str() const override {
		return word;
	}
};

// 注释Token
struct Comment :Token{
	string content;	//注释内容
	string type;		//注释类型："//", "/*", "#"
	
	Comment(){
		this->Tag = COMMENT;
		this->content = "";
		this->type = "";
	}
	
	Comment(string content, string type) :Token(COMMENT){
		this->content = content;
		this->type = type;
	}
	
	// 获取字面值的字符串表示
	std::string str() const override {
		return "comment: " + content;
	}
};

struct Type :Word{
	int width;
	static Type *Int, *Char, *Double, *Float, *Bool, *String, *Null;
	Type(){ Tag = BASIC; word = "type", width = 0; }
	Type(Word word, int width) :Word(word), width(width){}
	Type(int Tag, string word, int width) :Word(Tag, word), width(width){}
	static Type *max(Type *T1, Type *T2){
		return T1->width > T2->width ? T1 : T2;
	}
	
	// 创建null类型
	static Type* createNullType() {
		return new Type(NULL_VALUE, "null", 0);
	}
	virtual Type *eval(){
		return this;
	}
	
	// 获取字面值的字符串表示
	string str() const override {
		return word;
	}
	
	// 获取类型大小（字节数）
	int getSize() const {
		return width;
	}
	
	// 获取类型大小的字符串表示
	string getSizeString() const {
		return to_string(width) + " bytes";
	}
};

// 操作符
struct Operator :Token {
	string symbol;  // 操作符符号
	int precedence; // 优先级
	bool isLeftAssociative; // 左结合性

    static Operator *Add, *Sub, *Mul, *Div, *Mod, *LT, *GT, *LE, *GE, *EQ, *NE, *AND, *OR, *Not, *BitAnd, *BitOr, *BitXor, *BitNot, *LeftShift, *RightShift, *Increment, *Decrement, *Dot, *Arrow, *Question, *Colon, *Assign;
	
	Operator() : Token(0), symbol(""), precedence(0), isLeftAssociative(true) {}
	Operator(int tag, const string& sym, int prec = 0, bool leftAssoc = true) 
		: Token(tag), symbol(sym), precedence(prec), isLeftAssociative(leftAssoc) {}
	
	// 获取操作符符号
	string getSymbol() const { 
		if (symbol.empty()) {
			return "unknown";
		}
		return symbol; 
	}
	
	// 获取优先级
	int getPrecedence() const { return precedence; }
	
	// 是否为左结合
	bool isLeftAssoc() const { return isLeftAssociative; }
	
	// 获取字面值的字符串表示
	string str() const override {
		return symbol;
	}
	
	// 获取操作符对应的操作名（用于calculate方法）
	string getOperationName() const {
		switch (Tag) {
			// 单字符操作符（使用ASCII码）
			case '+': return "add";
			case '-': return "subtract";
			case '*': return "multiply";
			case '/': return "divide";
			case '%': return "modulo";
			case '&': return "bitwiseAnd";
			case '|': return "bitwiseOr";
			case '^': return "bitwiseXor";
			case '<': return "lessThan";
			case '>': return "greaterThan";
			case '~': return "bitwiseNot";
			case '!': return "logicalNot";
			
			// 双字符操作符（枚举值）
			case EQ_EQ: return "equals";      // ==
			case NE_EQ: return "notEquals";   // !=
			case 300: return "lessEqual";      // <= (LE)
			case 301: return "greaterEqual";   // >= (GE)
			case LEFT_SHIFT: return "leftShift";   // <<
			case RIGHT_SHIFT: return "rightShift"; // >>
			case AND_AND: return "logicalAnd";     // &&
			case OR_OR: return "logicalOr";        // ||
			case INCREMENT: return "increment";    // ++
			case DECREMENT: return "decrement";    // --
			default: return "";
		}
	}
	
	// 获取操作符对应的枚举类型（用于表达式计算）
	int getOperationType() const {
		switch (Tag) {
			// 单字符操作符（直接返回ASCII码）
			case '+': return '+';
			case '-': return '-';
			case '*': return '*';
			case '/': return '/';
			case '%': return '%';
			case '&': return '&';
			case '|': return '|';
			case '^': return '^';
			case '<': return '<';
			case '>': return '>';
			case '~': return '~';
			case '!': return '!';
			
			// 双字符操作符（已经是枚举值）
			case EQ_EQ: return EQ_EQ;      // ==
			case NE_EQ: return NE_EQ;   // !=
			case 300: return 300;      // <= (LE)
			case 301: return 301;   // >= (GE)
			case LEFT_SHIFT: return LEFT_SHIFT;   // <<
			case RIGHT_SHIFT: return RIGHT_SHIFT; // >>
			case AND_AND: return AND_AND;     // &&
			case OR_OR: return OR_OR;        // ||
			case INCREMENT: return INCREMENT;    // ++
			case DECREMENT: return DECREMENT;    // --
			default: return 0;
		}
	}
	
	// 获取操作符符号（提供给parser和interpreter使用）
	string getOperatorSymbol() const {
		switch (Tag) {
			// 单字符操作符（使用ASCII码）
			case '+': return "+";
			case '-': return "-";
			case '*': return "*";
			case '/': return "/";
			case '%': return "%";
			case '&': return "&";
			case '|': return "|";
			case '^': return "^";
			case '<': return "<";
			case '>': return ">";
			case '~': return "~";
			case '!': return "!";
			
			// 双字符操作符（枚举值）
			case EQ_EQ: return "==";
			case NE_EQ: return "!=";
			case 300: return "<=";      // LE
			case 301: return ">=";      // GE
			case LEFT_SHIFT: return "<<";
			case RIGHT_SHIFT: return ">>";
			case AND_AND: return "&&";
			case OR_OR: return "||";
			case INCREMENT: return "++";
			case DECREMENT: return "--";
			default: return "unknown";
		}
	}
};

} // namespace lexer

#endif // TOKEN_H
