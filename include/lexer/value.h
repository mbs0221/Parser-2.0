#ifndef VALUE_H
#define VALUE_H

#include "lexer/token.h"
// 基本值类型Token
#include <string>

using namespace std;

namespace lexer {

struct Integer : public Token {
    using value_type = int;
    int value;
    
    Integer() : Token(NUM), value(0) {}
    Integer(int val) : Token(NUM), value(val) {}
    
    int getValue() const { return value; }
    void setValue(int val) { value = val; }
    
    string str() const override {
        return to_string(value);
    }
};

struct Double : public Token {
    using value_type = double;
    double value;
    
    Double() : Token(REAL), value(0.0) {}
    Double(double val) : Token(REAL), value(val) {}
    
    double getValue() const { return value; }
    void setValue(double val) { value = val; }
    
    string str() const override {
        return to_string(value);
    }
};

struct Bool : public Token {
    using value_type = bool;
    bool value;
    static Bool *True, *False;
    
    Bool() : Token(BOOL), value(false) {}
    Bool(bool val) : Token(BOOL), value(val) {}
    
    bool getValue() const { return value; }
    void setValue(bool val) { value = val; }
    
    string str() const override {
        return value ? "true" : "false";
    }
};

struct Char : public Token {
    using value_type = char;
    char value;
    
    Char() : Token(CHAR), value('\0') {}
    Char(char val) : Token(CHAR), value(val) {}
    
    char getValue() const { return value; }
    void setValue(char val) { value = val; }
    
    string str() const override {
        return string(1, value);
    }
};

struct String : public Token {
    using value_type = string;
    string value;
    
    String() : Token(STR), value("") {}
    String(const string& str) : Token(STR), value(str) {}
    
    const string& getValue() const { return value; }
    void setValue(const string& str) { value = str; }
    
    string str() const override {
        return value;
    }
};

// 访问修饰符枚举类型
enum VisibilityType {
    VIS_PUBLIC,
    VIS_PRIVATE,
    VIS_PROTECTED
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
TokenFlyweight* getFactory();
void initializeBoolStatics();
} // namespace lexer

#endif // VALUE_H