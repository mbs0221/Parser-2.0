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

private:
    Lexer lex;
    int depth = 0;
    // 参数解析
    vector<pair<string, Type*>> parseParameterList();
};



#endif