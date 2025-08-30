#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <map>
#include <fstream>
#include <memory>
#include <mutex>
#include <mutex>
#include "lexer/token.h"

using namespace std;

namespace lexer {

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
	
	// 泛型匹配方法 - 返回Token类型
	template<typename T>
	T* match();
	
	// Token管理方法
	void move(); // 移动到下一个token
	Token* token() { return look; }
	
	// 操作符匹配方法
	Operator* matchOperator();
	
	// 单词匹配方法
	Word* matchWord();
	
protected:
	Token *match_id();
	Token *match_other();
	Token *skip_comment();
	Token *match_number();
	Token *match_char();
	Token *match_decimal();
	Token *match_oct();
	Token *match_hex();
	Token *match_string();
};

} // namespace lexer

#endif // LEXER_H