#ifndef PARSER_H
#define PARSER_H

#include "lexer/lexer.h"
#include "parser/inter.h"
#include "parser/expression.h"
#include "parser/statement.h"
#include <string>
#include <list>
#include <map>

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
    Expression* parseCall(VariableExpression* calleeExpr);
    Expression* parseAccess(VariableExpression* id);
    Expression* parseStructInstantiation(VariableExpression* structName);
    Expression* parseMemberAccess();
    Expression* parseMethodCall();

    // 数组和字典解析
    Expression* parseArray();
    Expression* parseDict();
    
    // 结构体和类解析
    StructDefinition* parseStruct();
    ClassDefinition* parseClass();
    ClassMethod* parseClassMethod();

    
    // 类型解析
    Type* parseType();

private:
    Lexer lex;
    Token *look;
    int depth = 0;
    // Parser不再管理作用域，只负责生成AST
    
    // 私有helper方法
    bool move();
    bool match(int tag);
    Operator* matchOperator();
    Integer* matchInt();
    Double* matchDouble();
    Char* matchChar();
    Bool* matchBool();
    String* matchString();
    Word* matchWord();
    string matchIdentifier();
    void matchToken(int tag);
};

#endif