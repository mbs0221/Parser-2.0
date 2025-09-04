#ifndef PARSER_H
#define PARSER_H

#include "lexer/lexer.h"
#include "parser/inter.h"
#include "parser/expression.h"
#include "parser/statement.h"
#include "parser/definition.h"
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
    
    // 声明解析（全局）
    Statement* parseDeclaration();
    
    // 语句解析（函数内部）
    Statement* parseStatement();
    ImportStatement* parseImportStatement();
    ImportStatement* parseSelectiveImport();
    ImportStatement* parseWildcardImport();
    VariableDefinition* parseVariableDefinition();
    IfStatement* parseIfStatement();
    WhileStatement* parseWhileStatement();
    ForStatement* parseForStatement();
    BreakStatement* parseBreakStatement();
    ContinueStatement* parseContinueStatement();
    ReturnStatement* parseReturnStatement();
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
    // 解析常量 - 模板方法
    template<typename T>
    Expression* parseConstant();
    Expression* parseConstant();  // 通用版本
    Expression* parseVariable();
    Expression* parseCallExpression(Expression* calleeExpr);

    // 数组和字典解析
    Expression* parseArray();
    Expression* parseDict();
    
    // 结构体实例化解析
    Expression* parseStructInstantiation(const string& structName);
    
    // 结构体和类解析
    StructDefinition* parseStruct();
    ClassDefinition* parseClass();

private:
    lexer::Lexer lex;
    int depth = 0;
    // 参数解析
    std::vector<std::pair<std::string, lexer::Type*>> parseParameterList();
};



#endif