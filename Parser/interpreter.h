#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "inter.h"
#include "parser.h"
#include "control_flow.h"
#include "ast_visitor.h"
#include <string>
#include <list>
#include <map>
#include <vector>
#include <functional>
#include <iostream>

using namespace std;

// 返回值包装结构
struct ReturnResult {
    bool hasReturn;
    bool hasBreak;
    bool hasContinue;
    Expression* value;
    
    ReturnResult() : hasReturn(false), hasBreak(false), hasContinue(false), value(nullptr) {}
    ReturnResult(Expression* val) : hasReturn(true), hasBreak(false), hasContinue(false), value(val) {}
    
    // 创建break结果
    static ReturnResult Break() {
        ReturnResult result;
        result.hasBreak = true;
        return result;
    }
    
    // 创建continue结果
    static ReturnResult Continue() {
        ReturnResult result;
        result.hasContinue = true;
        return result;
    }
};

// 解释器类 - 负责AST的求值和作用域管理
class Interpreter : public ASTVisitor {
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
    
    // 结构体定义存储
    map<string, StructDefinition*> structDefinitions;
    map<string, ClassDefinition*> classDefinitions;
    
    // 内置函数映射
    map<string, function<Expression*(vector<Expression*>&)>> builtinFunctions;
    
    // 调用栈（用于调试和错误报告）
    vector<string> callStack;
    
    // 结果栈 - 用于访问者模式存储计算结果
    vector<Expression*> resultStack;

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
    
    // 结果栈操作
    void pushResult(Expression* result);
    Expression* popResult();
    Expression* peekResult();
    void clearResultStack();
    
    // AST求值方法
    Expression* evaluate(AST* node);
    Expression* evaluate(Expression* expr);
    void execute(Statement* stmt);
    void execute(Program* program);
    void visit(AST* node);

    // ASTVisitor接口实现 - 表达式访问方法
    void visit(IntExpression* expr) override;
    void visit(DoubleExpression* expr) override;    
    void visit(IdentifierExpression* expr) override;
    void visit(UnaryExpression* expr) override;
    void visit(BinaryExpression* expr) override;
    void visit(AssignmentExpression* expr) override;
    void visit(BoolExpression* expr) override;
    void visit(CharExpression* expr) override;
    void visit(ArrayNode* expr) override;
    void visit(DictNode* expr) override;
    void visit(StringLiteral* expr) override;
    void visit(AccessExpression* expr) override;
    void visit(CallExpression* expr) override;
    
    // CastExpression具体访问方法
    void visit(CastExpression<IntExpression>* expr) override;
    void visit(CastExpression<DoubleExpression>* expr) override;
    // void visit(CastExpression<StringLiteral>* expr) override;
    void visit(CastExpression<CharExpression>* expr) override;
    void visit(CastExpression<BoolExpression>* expr) override;
    void visit(StructInstantiationExpression* expr) override;
    void visit(ClassInstantiationExpression* expr) override;
    void visit(MemberAccessExpression* expr) override;
    void visit(MethodCallExpression* expr) override;

    // ASTVisitor接口实现 - 语句访问方法
    void visit(ImportStatement* stmt) override;
    void visit(ExpressionStatement* stmt) override;
    void visit(VariableDeclaration* stmt) override;
    void visit(IfStatement* stmt) override;
    void visit(WhileStatement* stmt) override;
    void visit(ForStatement* stmt) override;
    void visit(DoWhileStatement* stmt) override;
    void visit(BlockStatement* stmt) override;
    void visit(FunctionDefinition* stmt) override;
    void visit(StructDefinition* stmt) override;
    void visit(ClassDefinition* stmt) override;
    void visit(BreakStatement* stmt) override;
    void visit(ContinueStatement* stmt) override;
    void visit(ReturnStatement* stmt) override;
    void visit(ThrowStatement* stmt) override;
    void visit(TryStatement* stmt) override;
    void visit(CatchStatement* stmt) override;
    void visit(FinallyStatement* stmt) override;
    void visit(SwitchStatement* stmt) override;
    void visit(CaseStatement* stmt) override;
    void visit(DefaultStatement* stmt) override;
    void visit(FunctionPrototype* stmt) override;

    // ASTVisitor接口实现 - 程序访问方法
    void visit(Program* program) override;
    
    // 内置函数
    Expression* executePrint(vector<Expression*>& args);
    Expression* executeCount(vector<Expression*>& args);
    Expression* executeCin(vector<Expression*>& args);
    void registerBuiltinFunctions();
    bool isBuiltinFunction(const string& funcName);
    Expression* executeBuiltinFunction(const string& funcName, vector<Expression*>& args);

    // 二元运算辅助方法
    void executeArithmeticOperation(BinaryExpression* binary, Expression* left, Expression* right);
    void executeComparisonOperation(BinaryExpression* binary, Expression* left, Expression* right);
    void executeLogicalOperation(BinaryExpression* binary, Expression* left, Expression* right);
    
    // 类型转换辅助方法
    template<typename T>
    void executeCastOperation(CastExpression<T>* cast);
    template<typename T>
    Expression* performCast(Expression* operand);
    
    // 错误处理
    void reportError(const string& message);
    void reportTypeError(const string& expected, const string& actual);
    
    // 调试和诊断
    void printScope();
    void printCallStack();

    

    // 结构体相关
    void registerStructDefinition(StructDefinition* structDef);
    void registerClassDefinition(ClassDefinition* classDef);
};

#endif // INTERPRETER_H
