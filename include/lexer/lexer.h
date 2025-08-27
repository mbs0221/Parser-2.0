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
	Token* look; // 当前token，从Parser移动过来
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
	
	// 匹配方法 - 从Parser移动过来
	bool match(int Tag);
	string matchIdentifier();
	Type* matchType();
	void matchToken(int tag);
	
	// matchValue方法 - 从Parser移动过来
	template<typename T>
	T* matchValue();
	Value* matchValue();  // 通用版本，根据token类型自动返回正确的值
	
	// Token管理方法
	void move(); // 移动到下一个token
	Token* token() { return look; }
	
	// 操作符匹配方法
	Operator* matchOperator();
	
	// 单词匹配方法
	Word* matchWord();
	
protected:
	Value *match_char();
	Value *match_string();
	Token *match_id();
	Value *match_number();
	Value *match_decimal();
	Value *match_hex();
	Value *match_oct();
	Token *match_other();
	Token *skip_comment();
};

#endif // LEXER_H