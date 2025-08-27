#ifndef LEXER_H
#define LEXER_H

#include "lexer/value.h"

using namespace std;

// 词法分析器
class Lexer{
private:
	char peek;
	map<string, Token*> words;
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
	Value *match_char();
	Value *match_string();
	Token *match_id();
	Value *match_number();
	Value *match_decimal();
	Value *match_hex();
	Value *match_oct();
	Operator *match_other();
	Token *skip_comment();
};

#endif // LEXER_H