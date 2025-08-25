#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "inter.h"
#include <string>
#include <list>
#include <map>
#include <vector>
#include <functional>

using namespace std;

// 解释器类 - 负责AST的求值和作用域管理
class Interpreter {
private:
    // 作用域结构 - 同时管理变量和函数
    struct Scope {
        map<string, Expression*> variables;
        map<string, FunctionDefinition*> functions;
        
        Scope() {}
    };
    
    // 作用域栈 - 管理变量和函数的作用域
    list<Scope*> scopes;
    
    // 当前作用域
    Scope* currentScope;
    
    // 全局作用域（程序级别的变量和函数）
    Scope* globalScope;
    
    // 内置函数映射
    map<string, function<Expression*(vector<Expression*>&)>> builtinFunctions;
    
    // 调用栈（用于调试和错误报告）
    vector<string> callStack;

public:
    Interpreter();
    ~Interpreter();
    
    // 作用域管理
    void enterScope();
    void exitScope();
    void defineVariable(const string& name, Expression* value);
    void defineFunction(const string& name, FunctionDefinition* func);
    Expression* lookupVariable(const string& name);
    FunctionDefinition* lookupFunction(const string& name);
    
    // AST求值方法
    Expression* evaluate(AST* node);
    void execute(Statement* stmt);
    void execute(Program* program);

    // 声明求值
    Expression* evaluateDeclaration(VariableDeclaration* decl);

    // 表达式求值
    Expression* evaluateExpression(Expression* expr);

    Expression* evaluateIdentifierExpression(IdentifierExpression* idExpr);
    Expression* evaluateArithmeticExpression(ArithmeticExpression* arith);
    Expression* evaluateStringLiteral(StringLiteral* strLit);
    Expression* evaluateArrayNode(ArrayNode* array);
    Expression* evaluateDictNode(DictNode* dict);
    Expression* evaluateStringNode(StringNode* strNode);
    Expression* evaluateAccessExpression(AccessExpression* access);
    Expression* evaluateCallExpression(CallExpression* call);
    
    // 语句执行
    void executeExpressionStatement(ExpressionStatement* stmt);
    void executeVariableDeclaration(VariableDeclaration* decl);
    void executeIfStatement(IfStatement* ifStmt);
    void executeWhileStatement(WhileStatement* whileStmt);
    void executeBlockStatement(BlockStatement* block);
    
    // 字符串运算
    Expression* stringConcatenation(Expression* left, Expression* right);
    Expression* stringComparison(Expression* left, Expression* right, const string& op);
    Expression* stringIndexing(StringNode* str, Expression* index);
    Expression* stringLength(StringNode* str);
    Expression* stringSubstring(StringNode* str, Expression* start, Expression* length);
    
    // 内置函数
    void registerBuiltinFunctions();
    Expression* executePrint(vector<Expression*>& args);
    Expression* executeCount(vector<Expression*>& args);
    Expression* executeCin(vector<Expression*>& args);
    Expression* executeStringLength(vector<Expression*>& args);
    Expression* executeStringSubstring(vector<Expression*>& args);
    
    // 字符串拼接相关
    Expression* evaluateStringConcatenationExpression(StringConcatenationExpression* concat);
    string convertToString(Expression* expr);
    
    // 错误处理
    void reportError(const string& message);
    void reportTypeError(const string& expected, const string& actual);
    
    // 调试和诊断
    void printScope();
    void printCallStack();

    // 内置函数检查器实现
    bool isBuiltinFunction(const string& funcName);
    Expression* executeBuiltinFunction(const string& funcName, vector<Expression*>& args);
    
    // 函数执行
    Expression* executeReturn(ReturnStatement* returnStmt);
    Expression* executeFunction(FunctionDefinition* funcDef, vector<Expression*>& args);
    void executeFunctionDefinition(FunctionDefinition* funcDef);
};

#endif // INTERPRETER_H
