#ifndef TOKEN_H
#define TOKEN_H

// 前向声明
struct Token;
struct Type;

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

//词法分析器标记
enum Tag{
	// 基本类型
	BASIC = 256, ID, NUM, REAL, DOUBLE, CHAR, STR, BOOL, ARRAY, DICT, END_OF_FILE,
	
	// 关键字
	IF, THEN, ELSE, DO, WHILE, FOR, CASE, DEFAULT,
	LET, BREAK, CONTINUE, RETURN, THROW, TRY, CATCH, FINALLY, SWITCH,
	BEGIN, END, FUNCTION, LAMBDA,
	STRUCT, CLASS, PUBLIC, PRIVATE, PROTECTED, IMPORT,
	
	// 注释
	COMMENT,
	
	// 运算符范围：300-399
	// 算术运算符
	PLUS = 300, MINUS, MULTIPLY, DIVIDE, MODULO,
	
	// 赋值运算符
	ASSIGN, PLUS_ASSIGN, MINUS_ASSIGN, MULTIPLY_ASSIGN, DIVIDE_ASSIGN, MODULO_ASSIGN,
	
	// 比较运算符
	LT, GT, LE, GE, EQ_EQ, NE_EQ,
	
	// 逻辑运算符
	AND_AND, OR_OR, NOT,
	
	// 位运算符
	BIT_AND, BIT_OR, BIT_XOR, BIT_NOT, LEFT_SHIFT, RIGHT_SHIFT,
	
	// 其他运算符
	INCREMENT, DECREMENT, DOT, ARROW, QUESTION, LBRACKET, RBRACKET, LBRACE, RBRACE, LPAREN, RPAREN, COMMA, SEMICOLON, COLON,
	
	// 特殊值
	NULL_VALUE
};

// 访问修饰符枚举类型
enum VisibilityType {
    VIS_PUBLIC,
    VIS_PRIVATE,
    VIS_PROTECTED
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
		return "token";
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
	string str() const override {
		return "comment: " + content;
	}
};

struct Type :Word{
	int width;
	static Type *Int, *Char, *Double, *Float, *Bool, *String;
	Type(){ Tag = BASIC; word = "type", width = 0; }
	Type(Word word, int width) :Word(word), width(width){}
	Type(int Tag, string word, int width) :Word(Tag, word), width(width){}
	static Type *max(Type *T1, Type *T2){
		return T1->width > T2->width ? T1 : T2;
	}
	virtual Type *eval(){
		return this;
	}
	
	// 获取字面值的字符串表示
	string str() const override {
		return word;
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
	
	// 转换为字符串
	string toString() const { return symbol; }
	
	// 获取字面值的字符串表示
	string str() const override {
		return symbol;
	}
};

// 访问修饰符
struct Visibility : public Token {
    VisibilityType visibilityType;  // 访问修饰符枚举类型
    
    static Visibility *Public, *Private, *Protected;
    
    Visibility() : Token(0), visibilityType(VIS_PUBLIC) {}
    Visibility(int tag, VisibilityType type) : Token(tag), visibilityType(type) {}
    
    // 根据Tag创建Visibility对象
    static Visibility* createFromTag(int tag) {
        switch(tag) {
            case Tag::PUBLIC: return Public;
            case Tag::PRIVATE: return Private;
            case Tag::PROTECTED: return Protected;
            default: return nullptr;
        }
    }
    
    // 获取访问修饰符类型
    VisibilityType getVisibilityType() const { return visibilityType; }
    
    // 转换为字符串
    string toString() const { 
        switch(visibilityType) {
            case VIS_PUBLIC: return "public";
            case VIS_PRIVATE: return "private";
            case VIS_PROTECTED: return "protected";
            default: return "unknown";
        }
    }
    
    // 获取字面值的字符串表示
    string str() const override {
        return toString();
    }
    
    // 判断是否为特定类型
    bool isPublic() const { return visibilityType == VIS_PUBLIC; }
    bool isPrivate() const { return visibilityType == VIS_PRIVATE; }
    bool isProtected() const { return visibilityType == VIS_PROTECTED; }
};

#endif // TOKEN_H
