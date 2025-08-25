#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <ctype.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <list>
#include <vector>
#include <stack>
#include <sstream>
#include <map>



using namespace std;

//词法分析器标记
enum Tag{
	IF = 256, THEN, ELSE, DO, WHILE, FOR, CASE, PRINT, BASIC,
	LET, ID, NUM, REAL, INDEX, BREAK, CONTINUE, RETURN, THROW, TRY, CATCH, FINALLY, SWITCH, DEFAULT,
	INT, DOUBLE, STR, BEGIN, END, FUNCTION, LAMBDA,
	AND, OR, EQ, NE, GE, BE, NEG, TRUE, FALSE, INPUT, OUTPUT, CALL,
	INC, DEC, SHL, SHR, TEMP
};

//词法单元
struct Token{
	int Tag;		//标记
	Token(){
		Tag = NULL;
	}
	Token(int Tag) :Tag(Tag){  }
};

struct Word :Token{
	string word;	//字符串
	static Word *Temp;
	Word(){
		this->Tag = NULL;
		this->word = "";
	}
	Word(int Tag, string word) :Token(Tag){
		this->word = word;
	}
	virtual Word *eval(){
		return this;
	}
};

struct Type :Word{
	int width;
	static Type *Int, *Char, *Double, *Float, *Bool;
	Type(){ Tag = BASIC; word = "type", width = 0; }
	Type(Word word, int width) :Word(word), width(width){}
	Type(int Tag, string word, int width) :Word(Tag, word), width(width){}
	static Type *max(Type *T1, Type *T2){
		return T1->width > T2->width ? T1 : T2;
	}
	virtual Type *eval(){
		return this;
	}
};

// 整数
struct Integer :Type{
	int value;		//值
	static Integer *True, *False;
	Integer() : value(-1){ Tag = NUM; width = 4; }
	Integer(int val) : value(val){ Tag = NUM; width = 4; }
	virtual Type *eval(){
		ostringstream oss;
		oss << value;
		word = oss.str();
		return this;
	}
};

// 小数
struct Double :Type{
	double value;
	Double(double value) : value(value){ Tag = REAL; width = 16; }
	virtual Type *eval(){
		ostringstream oss;
		oss << value;
		word = oss.str();
		return this;
	}
};



// 词法分析器
class Lexer{
private:
	char peek;
	map<string, Word*> words;
	ifstream inf;// 文件输入流
	bool read(char c){
		char a;
		inf.read(&a, sizeof(char));
		return a == c;
	}
public:
	int column = 1;
	int line = 1;
	Lexer();
	~Lexer();
	bool open(string file);
	Token *scan();
protected:
	Token *match_char();
	Token *match_string();
	Token *match_id();
	Token *match_number();
	Token *match_decimal();
	Token *match_hex();
	Token *match_oct();
	Token *match_other();
	Token *skip_comment();
};

#endif // LEXER_H