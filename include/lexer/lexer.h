#ifndef LEXER_H
#define LEXER_H

#include "lexer/token.h"

#include <string>
#include <map>
#include <fstream>
#include <iostream>
#include <sstream>
#include <memory>
#include <mutex>


using namespace std;

namespace lexer {

// 词法分析器
class Lexer{
private:
	char peek;
	map<string, Token*> words;
	istream* inf;// 输入流指针，可以是文件流或字符串流
	ifstream file_inf; // 文件流对象
	istringstream str_inf; // 字符串流对象
	Token* look; // 当前token，从Parser移动过来
	bool read(char c){
		char a;
		inf->read(&a, sizeof(char));
		return a == c;
	}
public:
	int column = 1;
	int line = 1;
	Lexer();
	~Lexer();
	bool from_file(const string& filepath); // 从文件加载
	bool from_string(const string& code); // 从字符串加载
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
	
	// 辅助函数 - token类型判断
	bool isToken(int tag) const;
	bool isKeyword(int tag) const;
	bool isOperator(int tag) const;
	bool isIdentifier() const;
	bool isString() const;
	bool isNumber() const;
	bool isAs() const;
	bool isFrom() const;
	
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