#ifndef PARSER_H
#define PARSER_H

#include "lexer/lexer.h"
#include "parser/inter.h"
#include "parser/expression.h"
#include "parser/statement.h"
#include "parser/function.h"
#include <string>
#include <list>
#include <map>
#include <typeinfo>

class Parser {
public:
    Parser();
    ~Parser();
    Program *parse(const std::string &file);

protected:
    // 程序解析
    Program* parseProgram();
    
    // 语句解析
    Statement* parseStatement();
    ImportStatement* parseImportStatement();
    VariableDeclaration* parseVariableDeclaration();
    IfStatement* parseIfStatement();
    WhileStatement* parseWhileStatement();
    ForStatement* parseForStatement();
    BreakStatement* parseBreakStatement();
    ContinueStatement* parseContinueStatement();
    ReturnStatement* parseReturnStatement();
    ThrowStatement* parseThrowStatement();
    TryStatement* parseTryStatement();
    SwitchStatement* parseSwitchStatement();
    BlockStatement* parseBlock();
    ExpressionStatement* parseExpressionStatement();
    
    // 函数解析
    FunctionDefinition* parseFunction();
    FunctionPrototype* parsePrototype();
    
    // 表达式解析
    Expression* parseExpression();
    Expression* parseExpressionWithPrecedence(int minPrecedence);
    Expression* parsePrimary();
    Expression* parsePostfix(Expression* expr);
    bool isBinaryOperator(int tag);
    Expression* parseConstant();
    Expression* parseVariable();
    Expression* parseCallExpression(Expression* calleeExpr);

    // 数组和字典解析
    Expression* parseArray();
    Expression* parseDict();
    
    // 结构体和类解析
    StructDefinition* parseStruct();
    ClassDefinition* parseClass();
    ClassMethod* parseClassMethod(const string& visibility);
    StructMember parseClassMember(const string& visibility);

    
    // 类型解析
    Type* parseType();

private:
    Lexer lex;
    Token *look;
    int depth = 0;
    // Parser不再管理作用域，只负责生成AST
    
    // 私有helper方法
    void move();
    bool match(int tag);
    Operator* matchOperator();
    bool isOperator(int tag);
    Word* matchWord();
    string matchIdentifier();
    Type* matchType();
    template<typename T>
    T* matchValue();
    Value* matchValue();  // 通用版本，根据token类型自动返回正确的值
    void matchToken(int tag);
    
    // 参数解析
    vector<pair<string, Type*>> parseParameterList();
};

// 模板函数实现 - 使用inline避免重复定义
template<typename T>
inline T* Parser::matchValue() {
    // 通用实现，会报错
    printf("SYNTAX ERROR line[%03d]: unsupported type %s\n", lex.line, typeid(T).name());
    exit(1);
    return nullptr;
}

// 模板特化 - 使用inline
template<>
inline Integer* Parser::matchValue<Integer>() {
    if (look->Tag == NUM) {
        Integer* intVal = static_cast<Integer*>(look);
        move();
        return intVal;
    }
    printf("SYNTAX ERROR line[%03d]: expected integer, got %d\n", lex.line, look->Tag);
    exit(1);
    return nullptr;
}

template<>
inline Double* Parser::matchValue<Double>() {
    if (look->Tag == REAL) {
        Double* doubleVal = static_cast<Double*>(look);
        move();
        return doubleVal;
    }
    printf("SYNTAX ERROR line[%03d]: expected double, got %d\n", lex.line, look->Tag);
    exit(1);
    return nullptr;
}

template<>
inline String* Parser::matchValue<String>() {
    if (look->Tag == STR) {
        String* stringVal = static_cast<String*>(look);
        move();
        return stringVal;
    }
    printf("SYNTAX ERROR line[%03d]: expected string, got %d\n", lex.line, look->Tag);
    exit(1);
    return nullptr;
}

template<>
inline Char* Parser::matchValue<Char>() {
    if (look->Tag == CHAR) {
        Char* charVal = static_cast<Char*>(look);
        move();
        return charVal;
    }
    printf("SYNTAX ERROR line[%03d]: expected char, got %d\n", lex.line, look->Tag);
    exit(1);
    return nullptr;
}

template<>
inline Bool* Parser::matchValue<Bool>() {
    if (look->Tag == BOOL) {
        Bool* boolVal = static_cast<Bool*>(look);
        move();
        return boolVal;
    }
    printf("SYNTAX ERROR line[%03d]: expected bool, got %d\n", lex.line, look->Tag);
    exit(1);
    return nullptr;
}

// 通用版本实现
inline Value* Parser::matchValue() {
    switch (look->Tag) {
        case NUM:
            return matchValue<Integer>();
        case REAL:
            return matchValue<Double>();
        case STR:
            return matchValue<String>();
        case CHAR:
            return matchValue<Char>();
        case BOOL:
            return matchValue<Bool>();
        default:
            printf("SYNTAX ERROR line[%03d]: expected value, got %d\n", lex.line, look->Tag);
            exit(1);
            return nullptr;
    }
}

#endif