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
    Expression* parseAssignment();
    Expression* parseCompare();
    Expression* parseAdditive();
    Expression* parseTerm();
    Expression* parseUnary();
    Expression* parseFactor();
    Expression* parseParentheses();
    Expression* parseReal();
    Expression* parseInt();
    Expression* parseIdentifier();
    Expression* parseCall(IdentifierExpression* calleeExpr);
    AccessExpression* parseAccess(IdentifierExpression* id);
    
    // 字符串解析
    StringLiteral* parseStringLiteral();
    CharExpression* parseCharLiteral();
    
    // 数组和字典解析
    ArrayNode* parseArray();
    DictNode* parseDict();
    
    // 结构体和类解析
    StructDefinition* parseStruct();
    ClassDefinition* parseClass();
    ClassMethod* parseClassMethod();
    Expression* parseStructInstantiation(IdentifierExpression* structName);
    Expression* parseMemberAccess();
    Expression* parseMethodCall();
    
    // 类型解析
    Type* parseType();

private:
    Lexer lex;
    Token *look;
    int depth = 0;
    // Parser不再管理作用域，只负责生成AST
};

#endif