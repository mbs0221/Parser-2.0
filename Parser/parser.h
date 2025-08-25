#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "inter.h"
#include <string>
#include <list>
#include <map>

class Parser {
public:
    Parser();
    ~Parser();
    Program *parse(const std::string &file);

protected:
    void move();
    bool match(int Tag);
    
    // 程序解析
    Program* parseProgram();
    
    // 语句解析
    Statement* parseStatement();
    VariableDeclaration* parseVariableDeclaration();
    IfStatement* parseIfStatement();
    WhileStatement* parseWhileStatement();
    ForStatement* parseForStatement();
    BreakStatement* parseBreakStatement();
    ContinueStatement* parseContinueStatement();
    ReturnStatement* parseReturnStatement();
    ThrowStatement* parseThrowStatement();
    TryStatement* parseTryStatement();
    CatchStatement* parseCatchStatement();
    FinallyStatement* parseFinallyStatement();
    SwitchStatement* parseSwitchStatement();
    CaseStatement* parseCaseStatement();
    DefaultStatement* parseDefaultStatement();
    BlockStatement* parseBlock();
    ExpressionStatement* parseExpressionStatement();
    ExpressionStatement* parsePrintStatement();
    
    // 函数解析
    FunctionDefinition* parseFunction();
    FunctionPrototype* parsePrototype();
    
    // 表达式解析
    Expression* parseExpression();
    Expression* parseCompare();
    Expression* parseAdditive();
    Expression* parseTerm();
    Expression* parseFactor();
    CallExpression* parseCall(Token* funcToken);
    AccessExpression* parseAccess(IdentifierExpression* id);
    
    // 字符串解析
    StringLiteral* parseStringLiteral();
    
    // 数组和字典解析
    ArrayNode* parseArray();
    DictNode* parseDict();
    
    // 类型解析
    Type* parseType();

private:
    Lexer lex;
    Token *look;
    int depth = 0;
    // Parser不再管理作用域，只负责生成AST
};

#endif