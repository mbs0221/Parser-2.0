#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "lexer/token.h"
#include "parser/inter.h"
#include "parser/ast_visitor.h"

using namespace std;
using namespace lexer;

// ==================== 表达式基类 ====================
// 表达式基类 - 继承自AST，支持泛型访问者
struct Expression : public AST {
    virtual int getTypePriority() const = 0;
    
    // 泛型accept方法声明
    template<typename ReturnType>
    ReturnType accept(ExpressionVisitor<ReturnType>* visitor);
};

// ==================== 常量表达式 ====================
// 常量表达式节点 - 存储原始数据，不直接使用Value类型
template<typename T>
struct ConstantExpression : public Expression {
    T value;
    
    // 构造函数 - 直接存储原始数据
    ConstantExpression(const T& val) : value(val) {}
    
    ~ConstantExpression() {
        // 没有动态分配的内存需要清理
    }
    
    // 重写泛型accept方法声明
    template<typename ReturnType>
    ReturnType accept(ExpressionVisitor<ReturnType>* visitor);
    
    int getTypePriority() const override {
        return 0;  // 常量优先级最高
    }
    
    string getLocation() const override;
    
    // 获取类型名称
    string getTypeName() const;
    
    // 获取原始值的方法
    const T& getValue() const { return value; }
};

// 包含模板特化声明
#include "parser/expression_specializations.h"



// ==================== 复合表达式 ====================
// 变量引用表达式节点
struct VariableExpression : public Expression {
    string name;
    
    VariableExpression(const string& n) : name(n) {}
    
    ~VariableExpression() {
        // VariableExpression 没有需要清理的指针成员
    }
    
    string getLocation() const override {
        return "variable: " + name;
    }
    
    // 重写泛型accept方法声明
    template<typename ReturnType>
    ReturnType accept(ExpressionVisitor<ReturnType>* visitor);
    
    int getTypePriority() const override {
        return 0;
    }
};

// 一元表达式节点
struct UnaryExpression : public Expression {
    Expression* operand;
    Operator* operator_;
    
    UnaryExpression(Expression* expr, Operator* op) : operand(expr), operator_(op) {}
    
    ~UnaryExpression() {
        if (operand) {
            delete operand;
            operand = nullptr;
        }
        // 注意：operator_ 是静态分配的，不需要删除
    }
    
    string getLocation() const override {
        return "unary expression";
    }
    
    // 重写泛型accept方法声明
    template<typename ReturnType>
    ReturnType accept(ExpressionVisitor<ReturnType>* visitor);
    
    int getTypePriority() const override {
        return operand ? operand->getTypePriority() : 0;
    }
    
    // 获取操作名称
    string getOperationName() const {
        return operator_ ? operator_->getOperationName() : "";
    }
    
    // 获取操作类型（枚举值）
    int getOperationType() const {
        return operator_ ? operator_->getOperationType() : 0;
    }
    
    // 获取操作符
    Operator* getOperator() const {
        return operator_;
    }
};

// 二元表达式节点
struct BinaryExpression : public Expression {
    Expression* left;
    Expression* right;
    Operator* operator_;
    
    BinaryExpression(Expression* l, Expression* r, Operator* op) : left(l), right(r), operator_(op) {}
    
    ~BinaryExpression() {
        if (left) {
            delete left;
            left = nullptr;
        }
        if (right) {
            delete right;
            right = nullptr;
        }
        // 注意：operator_ 是静态分配的，不需要删除
    }
    
    string getLocation() const override {
        return "binary expression";
    }
    
    // 重写泛型accept方法声明
    template<typename ReturnType>
    ReturnType accept(ExpressionVisitor<ReturnType>* visitor);
    
    int getTypePriority() const override {
        int leftPriority = left ? left->getTypePriority() : 0;
        int rightPriority = right ? right->getTypePriority() : 0;
        return max(leftPriority, rightPriority);
    }
    
    // 获取操作名称
    string getOperationName() const {
        return operator_ ? operator_->getOperationName() : "";
    }
    
    // 获取操作类型（枚举值）
    int getOperationType() const {
        return operator_ ? operator_->getOperationType() : 0;
    }
    
    // 获取操作符
    Operator* getOperator() const {
        return operator_;
    }
};

// 赋值表达式现在直接使用BinaryExpression，不再需要单独的AssignExpression类型

// 类型转换表达式 - 使用类型系统进行转换
struct CastExpression : public Expression {
    Expression* operand;     // 被转换的表达式
    string targetTypeName;   // 目标类型名称
    
    CastExpression(Expression* expr, const string& targetType) 
        : operand(expr), targetTypeName(targetType) {}
    
    ~CastExpression() {
        if (operand) {
            delete operand;
            operand = nullptr;
        }
    }
    
    // 重写泛型accept方法声明
    template<typename ReturnType>
    ReturnType accept(ExpressionVisitor<ReturnType>* visitor);
    
    // 获取目标类型名称
    string getTargetTypeName() const {
        return targetTypeName;
    }
    
    // 获取目标类型优先级
    int getTargetTypePriority() const {
        if (targetTypeName == "int") return 2;
        else if (targetTypeName == "double") return 3;
        else if (targetTypeName == "bool") return 0;
        else if (targetTypeName == "char") return 1;
        else if (targetTypeName == "string") return 4;
        else return 0;
    }
    
    int getTypePriority() const override {
        return getTargetTypePriority();
    }
    
    string getLocation() const override {
        return "cast to " + targetTypeName;
    }
};



// ==================== 访问和调用表达式 ====================
// 访问表达式节点 - 统一处理数组/字典访问和成员访问
struct AccessExpression : public Expression {
    Expression* target;  // 目标对象/数组/字典/结构体
    Expression* key;     // 访问键（表达式形式，如数组索引或字符串常量）
    
    AccessExpression(Expression* t, Expression* k) 
        : target(t), key(k) {}
    
    ~AccessExpression() {
        if (target) {
            delete target;
            target = nullptr;
        }
        if (key) {
            delete key;
            key = nullptr;
        }
    }
    
    string getLocation() const override {
        return "access expression";
    }
    
    // 重写泛型accept方法声明
    template<typename ReturnType>
    ReturnType accept(ExpressionVisitor<ReturnType>* visitor);
    
    int getTypePriority() const override {
        return target ? target->getTypePriority() : 0;
    }
};

// 函数调用表达式节点 - 统一处理所有函数调用
struct CallExpression : public Expression {
    Expression* callee;  // 被调用者（函数表达式）
    vector<Expression*> arguments;
    
    CallExpression(Expression* func, const vector<Expression*>& args) 
        : callee(func), arguments(args) {}
    
    ~CallExpression() {
        if (callee) {
            delete callee;
            callee = nullptr;
        }
        for (Expression* arg : arguments) {
            if (arg) {
                delete arg;
            }
        }
        arguments.clear();
    }
    
    string getLocation() const override {
        return "function call";
    }
    
    // 重写泛型accept方法声明
    template<typename ReturnType>
    ReturnType accept(ExpressionVisitor<ReturnType>* visitor);
    
    int getTypePriority() const override {
        return 0;
    }
    
    // 获取被调用者
    Expression* getCallee() const {
        return callee;
    }
    
    // 获取参数列表
    const vector<Expression*>& getArguments() const {
        return arguments;
    }
};

// ==================== 自增自减表达式 ====================
// 自增自减表达式节点 - 继承自UnaryExpression，统一处理 ++ 和 -- 操作
struct IncrementDecrementExpression : public UnaryExpression {
    bool isPrefix;           // 是否为前缀操作 (++i vs i++)
    int delta;               // 增量值：+1 表示自增，-1 表示自减
    
    IncrementDecrementExpression(Expression* expr, bool prefix, int d) 
        : UnaryExpression(expr, nullptr), isPrefix(prefix), delta(d) {}
    
    ~IncrementDecrementExpression() {
        // 基类UnaryExpression会处理operand的删除
    }
    
    string getLocation() const override {
        string prefix = isPrefix ? "prefix" : "postfix";
        string operation = (delta > 0) ? "increment" : "decrement";
        return prefix + " " + operation;
    }
    
    // 重写泛型accept方法声明
    template<typename ReturnType>
    ReturnType accept(ExpressionVisitor<ReturnType>* visitor);
    
    int getTypePriority() const override {
        return isPrefix ? 6 : 2;  // 前缀优先级高，后缀优先级低
    }
    
    // 是否为前缀操作
    bool isPrefixOperation() const {
        return isPrefix;
    }
    
    // 是否为后缀操作
    bool isPostfixOperation() const {
        return !isPrefix;
    }
    
    // 是否为自增操作
    bool isIncrement() const {
        return delta > 0;
    }
    
    // 是否为自减操作
    bool isDecrement() const {
        return delta < 0;
    }
    
    // 获取增量值
    int getDelta() const {
        return delta;
    }
};

// ==================== 三元表达式 ====================
// 三元表达式节点 - 条件 ? 真值 : 假值，继承自BinaryExpression
struct TernaryExpression : public BinaryExpression {
    Expression* condition;   // 条件表达式
    
    TernaryExpression(Expression* cond, Expression* trueVal, Expression* falseVal) 
        : BinaryExpression(trueVal, falseVal, nullptr), condition(cond) {}
    
    ~TernaryExpression() {
        if (condition) {
            delete condition;
            condition = nullptr;
        }
        // 基类BinaryExpression会处理left和right的删除
    }
    
    string getLocation() const override {
        return "ternary expression";
    }
    
    // 重写泛型accept方法声明
    template<typename ReturnType>
    ReturnType accept(ExpressionVisitor<ReturnType>* visitor);
    
    int getTypePriority() const override {
        return 1;  // 三元运算符优先级较低，仅次于赋值
    }
    
    // 获取条件表达式
    Expression* getCondition() const {
        return condition;
    }
    
    // 获取真值表达式（继承自BinaryExpression的left）
    Expression* getTrueValue() const {
        return left;
    }
    
    // 获取假值表达式（继承自BinaryExpression的right）
    Expression* getFalseValue() const {
        return right;
    }
};

// MemberAssignExpression已移除，成员赋值通过AccessExpression和AssignExpression的组合实现

// ==================== 结构体和类相关表达式 ====================
// MemberAccessExpression已合并到AccessExpression中



// 类型转换模板特化已删除，使用Value类型的运算符重载替代

#endif // EXPRESSION_H
