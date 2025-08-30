#ifndef VALUE_H
#define VALUE_H

#include "lexer/token.h"
// 基本值类型Token
#include <string>

using namespace std;

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

#endif