#include "lexer/value.h"
#include <map>
#include <mutex>

// 声明外部factory变量
extern TokenFlyweight* factory;



// Bool静态成员定义 - 直接初始化
Bool *Bool::True = new Bool(true);
Bool *Bool::False = new Bool(false);

// 访问修饰符
Visibility *Visibility::Public = new Visibility(PUBLIC, VIS_PUBLIC);
Visibility *Visibility::Private = new Visibility(PRIVATE, VIS_PRIVATE);
Visibility *Visibility::Protected = new Visibility(PROTECTED, VIS_PROTECTED);

TokenFlyweight* TokenFlyweight::instance = nullptr;
std::mutex TokenFlyweight::mutex;

// 获取单例实例
TokenFlyweight* TokenFlyweight::getInstance() {
    if (instance == nullptr) {
        std::lock_guard<std::mutex> lock(mutex);
        if (instance == nullptr) {
            instance = new TokenFlyweight();
        }
    }
    return instance;
}

// 析构函数
TokenFlyweight::~TokenFlyweight() {
    	// 使用智能指针后，不需要手动删除，shared_ptr会自动管理内存
	tokenCache.clear();
	intCache.clear();
	doubleCache.clear();
	charCache.clear();
	wordCache.clear();
}

// 获取Token对象
std::shared_ptr<Token> TokenFlyweight::getToken(int type) {
    std::string key = "token:" + std::to_string(type);
    auto it = tokenCache.find(key);
    if (it != tokenCache.end()) {
        return it->second;
    }
    
    auto token = std::make_shared<Token>(type);
    tokenCache[key] = token;
    return token;
}

// 获取Integer对象
std::shared_ptr<Integer> TokenFlyweight::getInteger(int val) {
    std::string key = "int:" + std::to_string(val);
    auto it = intCache.find(key);
    if (it != intCache.end()) {
        return it->second;
    }

    auto i = std::make_shared<Integer>(val);
    intCache[key] = i;
    return i;
}

// 获取Double对象
std::shared_ptr<Double> TokenFlyweight::getDouble(double val) {
    std::string key = "double:" + std::to_string(val);
    auto it = doubleCache.find(key);
    if (it != doubleCache.end()) {
        return it->second;
    }

    auto d = std::make_shared<Double>(val);
    doubleCache[key] = d;
    return d;
}

// 获取Char对象
std::shared_ptr<Char> TokenFlyweight::getChar(char val) {
    std::string key = "char:" + std::to_string(static_cast<int>(val));
    auto it = charCache.find(key);
    if (it != charCache.end()) {
        return it->second;
    }

    auto c = std::make_shared<Char>(val);
    charCache[key] = c;
    return c;
}

// 获取Word对象（关键字）
std::shared_ptr<Word> TokenFlyweight::getWord(int tag, const std::string& lexeme) {
    std::string key = "word:" + std::to_string(tag) + ":" + lexeme;
    auto it = wordCache.find(key);
    if (it != wordCache.end()) {
        return it->second;
    }
    
    auto word = std::make_shared<Word>(tag, lexeme);
    wordCache[key] = word;
    return word;
}

// 全局函数获取TokenFlyweight实例
TokenFlyweight* getTokenFlyweight() {
    return TokenFlyweight::getInstance();
}

// 使用统一Token享元工厂创建实例
TokenFlyweight* factory = TokenFlyweight::getInstance();