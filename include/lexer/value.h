#ifndef VALUE_H
#define VALUE_H

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

using namespace std;

//词法分析器标记
enum Tag{
	// 基本类型
	BASIC = 256, ID, NUM, REAL, DOUBLE, CHAR, STR, BOOL, END_OF_FILE,
	
	// 关键字
	IF, THEN, ELSE, DO, WHILE, FOR, CASE, DEFAULT,
	LET, BREAK, CONTINUE, RETURN, THROW, TRY, CATCH, FINALLY, SWITCH,
	BEGIN, END, FUNCTION, LAMBDA,
	STRUCT, CLASS, PUBLIC, PRIVATE, PROTECTED, IMPORT,
	
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

// ==================== 值类型基类 ====================
// 所有值的基类 - 继承自Token
struct Value : public Token {
    Type* valueType;  // 值的类型
    static Value* NullValue;  // 空值
    
    Value() : Token(0), valueType(nullptr) {}
    Value(int tag, Type* type) : Token(tag), valueType(type) {}
    virtual ~Value() = default;
    
    // 获取值的字符串表示
    virtual string toString() const { return "value"; }
    
    // 获取字面值的字符串表示
    virtual string str() const { return toString(); }
    
    // 转换为布尔值 - 用于逻辑运算
    virtual bool toBool() const { return false; }
    
    // 访问操作 - 默认不支持访问
    virtual Value* access(Value* key) {
        throw runtime_error("Type does not support access operation");
    }
};

// ==================== 基本值类型 ====================
// 注意：基本值类型现在在lexer.h中定义，直接继承自Value

// 布尔值类型
struct Bool : public Value {
    bool value;
    static Bool *True, *False;
    
    Bool() : Value(BOOL, Type::Bool), value(false) {}
    Bool(bool val) : Value(BOOL, Type::Bool), value(val) {}
    
    // 获取布尔值
    bool getValue() const {
        return value;
    }
    
    // 设置布尔值
    void setValue(bool val) {
        value = val;
    }
    
    // 逻辑运算
    Bool operator&&(const Bool& other) const {
        return Bool(value && other.value);
    }
    
    Bool operator||(const Bool& other) const {
        return Bool(value || other.value);
    }
    
    Bool operator==(const Bool& other) const {
        return Bool(value == other.value);
    }
    
    Bool operator!=(const Bool& other) const {
        return Bool(value != other.value);
    }
    
    // 一元运算符
    Bool operator!() const {
        return Bool(!value);
    }
    
    string toString() const override {
        return value ? "true" : "false";
    }
    
    // 获取字面值的字符串表示
    string str() const override {
        return toString();
    }
};

struct Integer : public Value {
    int value;
    
    Integer() : Value(NUM, Type::Int), value(0) {}
    Integer(int val) : Value(NUM, Type::Int), value(val) {}
    
    // 获取整数值
    int getValue() const {
        return value;
    }
    
    // 设置整数值
    void setValue(int val) {
        value = val;
    }
    
    // 算术运算
    Integer operator+(const Integer& other) const {
        return Integer(value + other.value);
    }
    
    Integer operator-(const Integer& other) const {
        return Integer(value - other.value);
    }
    
    Integer operator*(const Integer& other) const {
        return Integer(value * other.value);
    }
    
    Integer operator/(const Integer& other) const {
        if (other.value == 0) {
            throw runtime_error("Division by zero");
        }
        return Integer(value / other.value);
    }
    
    Integer operator%(const Integer& other) const {
        if (other.value == 0) {
            throw runtime_error("Modulo by zero");
        }
        return Integer(value % other.value);
    }
    
    // 一元运算符
    Integer operator+() const {
        return Integer(+value);
    }
    
    Integer operator-() const {
        return Integer(-value);
    }
    
    Integer operator++() {
        return Integer(++value);
    }
    
    Integer operator++(int) {
        return Integer(value++);
    }
    
    Integer operator--() {
        return Integer(--value);
    }
    
    Integer operator--(int) {
        return Integer(value--);
    }
    
    // 比较运算
    Bool operator==(const Integer& other) const {
        return Bool(value == other.value);
    }
    
    Bool operator!=(const Integer& other) const {
        return Bool(value != other.value);
    }
    
    Bool operator<(const Integer& other) const {
        return Bool(value < other.value);
    }
    
    Bool operator>(const Integer& other) const {
        return Bool(value > other.value);
    }
    
    Bool operator<=(const Integer& other) const {
        return Bool(value <= other.value);
    }
    
    Bool operator>=(const Integer& other) const {
        return Bool(value >= other.value);
    }
    
    // 位运算
    Integer operator&(const Integer& other) const {
        return Integer(value & other.value);
    }
    
    Integer operator|(const Integer& other) const {
        return Integer(value | other.value);
    }
    
    Integer operator^(const Integer& other) const {
        return Integer(value ^ other.value);
    }
    
    Integer operator<<(const Integer& other) const {
        return Integer(value << other.value);
    }
    
    Integer operator>>(const Integer& other) const {
        return Integer(value >> other.value);
    }
    
    Integer operator~() const {
        return Integer(~value);
    }
    
    // 逻辑运算
    Bool operator&&(const Integer& other) const {
        return Bool(value && other.value);
    }
    
    Bool operator||(const Integer& other) const {
        return Bool(value || other.value);
    }
    
    Bool operator!() const {
        return Bool(!value);
    }
    
    // 转换为布尔值
    bool toBool() const override {
        return value != 0;
    }
    
    // 转换为字符串
    string toString() const override {
        return to_string(value);
    }
    
    // 获取字面值的字符串表示
    string str() const override {
        return toString();
    }
};

// 字符值类型
struct Char : public Value {
    char value;
    
    Char() : Value(CHAR, Type::Char), value('\0') {}
    Char(char val) : Value(CHAR, Type::Char), value(val) {}
    
    // 获取字符值
    char getValue() const {
        return value;
    }
    
    // 设置字符值
    void setValue(char val) {
        value = val;
    }
    
    // 算术运算
    Char operator+(const Char& other) const {
        return Char(value + other.value);
    }
    
    // 比较运算
    Bool operator==(const Char& other) const {
        return Bool(value == other.value);
    }
    
    Bool operator!=(const Char& other) const {
        return Bool(value != other.value);
    }
    
    string toString() const override {
        return string(1, value);
    }
    
    // 获取字面值的字符串表示
    string str() const override {
        return toString();
    }
};

// 浮点数类型 - 作为值类型
struct Double : public Value {
    double value;
    
    Double() : Value(REAL, Type::Double), value(0.0) {}
    Double(double val) : Value(REAL, Type::Double), value(val) {}
    
    // 获取浮点数值
    double getValue() const {
        return value;
    }
    
    // 设置浮点数值
    void setValue(double val) {
        value = val;
    }
    
    // 算术运算
    Double operator+(const Double& other) const {
        return Double(value + other.value);
    }
    
    Double operator-(const Double& other) const {
        return Double(value - other.value);
    }
    
    Double operator*(const Double& other) const {
        return Double(value * other.value);
    }
    
    Double operator/(const Double& other) const {
        if (other.value == 0.0) {
            throw runtime_error("Division by zero");
        }
        return Double(value / other.value);
    }
    
    // 比较运算
    Bool operator==(const Double& other) const {
        return Bool(value == other.value);
    }
    
    Bool operator!=(const Double& other) const {
        return Bool(value != other.value);
    }
    
    Bool operator<(const Double& other) const {
        return Bool(value < other.value);
    }
    
    Bool operator>(const Double& other) const {
        return Bool(value > other.value);
    }
    
    Bool operator<=(const Double& other) const {
        return Bool(value <= other.value);
    }
    
    Bool operator>=(const Double& other) const {
        return Bool(value >= other.value);
    }
    
    // 逻辑运算
    Bool operator&&(const Double& other) const {
        return Bool(value && other.value);
    }
    
    Bool operator||(const Double& other) const {
        return Bool(value || other.value);
    }
    
    // 一元运算符
    Double operator-() const {
        return Double(-value);
    }
    
    Bool operator!() const {
        return Bool(!value);
    }
    
    string toString() const override {
        ostringstream oss;
        oss << value;
        return oss.str();
    }
    
    // 获取字面值的字符串表示
    string str() const override {
        return toString();
    }
};

// 复合值基类 - 支持访问操作
struct CompositeValue : public Value {
    CompositeValue(int tag, Type* type) : Value(tag, type) {}
    
    // 虚函数：执行访问操作
    virtual Value* access(Value* key) = 0;
};

// 数组类型 - 支持传统数组和JSON数组
struct Array : public CompositeValue {
    vector<Value*> elements;
    
    Array() : CompositeValue(STR, new Type(STR, "array", 0)) {}
    
    // 添加元素
    void addElement(Value* element) {
        elements.push_back(element);
    }
    
    // 获取元素
    Value* getElement(size_t index) const {
        return index < elements.size() ? elements[index] : nullptr;
    }
    
    // 设置元素
    void setElement(size_t index, Value* element) {
        if (index < elements.size()) {
            elements[index] = element;
        } else if (index == elements.size()) {
            elements.push_back(element);
        }
    }
    
    // 获取元素数量
    size_t getElementCount() const {
        return elements.size();
    }
    
    // 获取数组大小
    size_t size() const {
        return elements.size();
    }
    
    string toString() const override {
        string result = "[";
        for (size_t i = 0; i < elements.size(); ++i) {
            if (i > 0) result += ", ";
            result += elements[i]->toString();
        }
        result += "]";
        return result;
    }
    
    // 获取字面值的字符串表示
    string str() const override {
        return toString();
    }
    
    // 访问操作 - 将key作为数组下标
    Value* access(Value* key) override {
        // 将key转换为数字索引
        string keyStr = key->toString();
        try {
            int index = stoi(keyStr);
            return index >= 0 && index < (int)elements.size() ? elements[index] : nullptr;
        } catch (const exception&) {
            // 如果转换失败，返回nullptr
            return nullptr;
        }
    }
};


// 字符串类型 - 继承自数组，元素是字符
struct String : public Array {
    string value;
    
    String() : Array(), value("") {
        // 更新Tag和Type
        Tag = STR;
        valueType = Type::String;
    }
    
    String(const string& str) : Array(), value(str) {
        // 更新Tag和Type
        Tag = STR;
        valueType = Type::String;
        
        // 将字符串的每个字符作为数组元素
        for (char c : str) {
            addElement(new Char(c));
        }
    }
    
    // 获取字符串值
    const string& getValue() const {
        return value;
    }
    
    // 设置字符串值
    void setValue(const string& str) {
        value = str;
        // 清空现有元素并重新添加
        elements.clear();
        for (char c : str) {
            addElement(new Char(c));
        }
    }
    
    // 获取字符串长度
    size_t length() const {
        return value.length();
    }
    
    // 获取字符
    char getChar(size_t index) const {
        return index < value.length() ? value[index] : '\0';
    }
    
    // 设置字符
    void setChar(size_t index, char c) {
        if (index < value.length()) {
            value[index] = c;
            // 更新对应的数组元素
            if (index < elements.size()) {
                if (Char* charVal = dynamic_cast<Char*>(elements[index])) {
                    charVal->setValue(c);
                } else {
                    elements[index] = new Char(c);
                }
            }
        }
    }
    
    // 字符串拼接
    void append(const string& str) {
        value += str;
        // 添加新的字符元素
        for (char c : str) {
            addElement(new Char(c));
        }
    }
    
    // 字符串拼接
    void append(char c) {
        value += c;
        addElement(new Char(c));
    }
    
    // 子字符串
    string substring(size_t start, size_t length) const {
        if (start >= value.length()) return "";
        return value.substr(start, length);
    }
    
    string toString() const override {
        return "\"" + value + "\"";
    }
    
    // 获取字面值的字符串表示
    string str() const override {
        return value;  // 返回不带引号的字符串值
    }
    
    // 访问操作 - 继承自Array，直接使用数组索引访问
    Value* access(Value* key) override {
        return Array::access(key);
    }
    
    // 字符串拼接运算符重载
    String operator+(const String& other) const {
        return String(value + other.value);
    }
    
    // 字符串与字符拼接
    String operator+(char c) const {
        return String(value + c);
    }
    
    // 字符串与整数拼接（将整数转换为字符串）
    String operator+(int num) const {
        return String(value + to_string(num));
    }
    
    // 字符串与浮点数拼接（将浮点数转换为字符串）
    String operator+(double num) const {
        return String(value + to_string(num));
    }
    
    // 字符串与布尔值拼接
    String operator+(bool b) const {
        return String(value + (b ? "true" : "false"));
    }
};

// 全局运算符重载 - 支持String与其他类型的混合运算
inline String operator+(const std::string& str, const String& s) {
    return String(str + s.getValue());
}

inline String operator+(char c, const String& s) {
    return String(std::string(1, c) + s.getValue());
}

inline String operator+(int num, const String& s) {
    return String(std::to_string(num) + s.getValue());
}

inline String operator+(double num, const String& s) {
    return String(std::to_string(num) + s.getValue());
}

inline String operator+(bool b, const String& s) {
    return String((b ? "true" : "false") + s.getValue());
}

// 字典类型 - 支持键值对存储
struct Dict : public CompositeValue {
    map<string, Value*> entries;
    
    Dict() : CompositeValue(STR, new Type(STR, "dict", 0)) {}
    
    // 设置键值对
    void setEntry(const string& key, Value* value) {
        entries[key] = value;
    }
    
    // 获取值
    Value* getEntry(const string& key) const {
        auto it = entries.find(key);
        return it != entries.end() ? it->second : nullptr;
    }
    
    // 检查键是否存在
    bool hasKey(const string& key) const {
        return entries.find(key) != entries.end();
    }
    
    // 删除键值对
    void removeEntry(const string& key) {
        entries.erase(key);
    }
    
    // 获取键值对数量
    size_t getEntryCount() const {
        return entries.size();
    }
    
    // 获取所有键
    vector<string> getKeys() const {
        vector<string> keys;
        for (const auto& pair : entries) {
            keys.push_back(pair.first);
        }
        return keys;
    }
    
    // 获取所有值
    vector<Value*> getValues() const {
        vector<Value*> values;
        for (const auto& pair : entries) {
            values.push_back(pair.second);
        }
        return values;
    }
    
    string toString() const override {
        string result = "{";
        bool first = true;
        for (const auto& pair : entries) {
            if (!first) result += ", ";
            result += "\"" + pair.first + "\": " + pair.second->toString();
            first = false;
        }
        result += "}";
        return result;
    }
    
    // 获取字面值的字符串表示
    string str() const override {
        return toString();
    }
    
    // 访问操作 - 将key作为字符串去查询
    Value* access(Value* key) override {
        // 将key转换为字符串
        string keyStr = key->toString();
        auto it = entries.find(keyStr);
        return it != entries.end() ? it->second : nullptr;
    }
};

#endif // VALUE_H
