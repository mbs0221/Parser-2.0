#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <map>
#include <fstream>
#include <memory>
#include <mutex>
#include <mutex>
#include "lexer/value.h"

using namespace std;


// TokenFlyweight单例类 - 实现享元模式来缓存和复用Token对象
class TokenFlyweight {
private:
	static TokenFlyweight* instance;
	static std::mutex mutex;
	std::map<std::string, std::shared_ptr<Token>> tokenCache;
	std::map<std::string, std::shared_ptr<Integer>> intCache;
	std::map<std::string, std::shared_ptr<Word>> wordCache;
	std::map<std::string, std::shared_ptr<Double>> doubleCache;
	std::map<std::string, std::shared_ptr<Char>> charCache;
	
	TokenFlyweight() = default; // 私有构造函数
	~TokenFlyweight();
	
public:
	static TokenFlyweight* getInstance(); // 获取单例实例
	
	std::shared_ptr<Token> getToken(int tag);
	std::shared_ptr<Integer> getInteger(int val);
	std::shared_ptr<Double> getDouble(double val);
	std::shared_ptr<Char> getChar(char val);
	std::shared_ptr<Word> getWord(int tag, const string& lexeme);
	
	// 获取缓存大小（用于测试）
	size_t getTokenCacheSize() const { return tokenCache.size(); }
	
	// 禁用拷贝构造和赋值
	TokenFlyweight(const TokenFlyweight&) = delete;
	TokenFlyweight& operator=(const TokenFlyweight&) = delete;
};

// 全局TokenFlyweight获取函数声明
extern TokenFlyweight* factory;

// 前向声明
class Token;
class Word;
class Operator;
class Type;


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

#endif // LEXER_H