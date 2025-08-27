#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "parser/inter.h"
#include "lexer/value.h"

using namespace std;

// 前向声明
struct ConstantExpression;
// StringLiteral已移动到value.h中作为String类型

// ==================== 常量表达式 ====================
// 常量表达式节点 - 直接使用Value类型（Value现在是Token的子类）
struct ConstantExpression : public Expression {
    Value* value;
    
    ConstantExpression(Value* val) : value(val) {}
    ConstantExpression(int val) : value(new Integer(val)) {}
    ConstantExpression(double val) : value(new Double(val)) {}
    ConstantExpression(bool val) : value(new Bool(val)) {}
    ConstantExpression(char val) : value(new Char(val)) {}
    ConstantExpression(const string& val) : value(new String(val)) {}
    
    ~ConstantExpression() {
        if (value) delete value;
    }
    
    void accept(ASTVisitor* visitor) override;
    
    int getTypePriority() const override {
        return 0;  // 常量优先级最高
    }
    
    string getLocation() const override {
        return value ? value->toString() : "null";
    }
};

// ==================== 复合表达式 ====================
// 变量引用表达式节点
struct VariableExpression : public Expression {
    string name;
    
    VariableExpression(const string& n) : name(n) {}
    
    string getLocation() const override {
        return "variable: " + name;
    }
    
    void accept(ASTVisitor* visitor) override;
    
    int getTypePriority() const override {
        return 0;
    }
};

// 一元表达式节点
struct UnaryExpression : public Expression {
    Expression* operand;
    Operator* operator_;
    
    UnaryExpression(Expression* expr, Operator* op) : operand(expr), operator_(op) {}
    
    string getLocation() const override {
        return "unary expression";
    }
    
    void accept(ASTVisitor* visitor) override;
    
    int getTypePriority() const override {
        return operand ? operand->getTypePriority() : 0;
    }
};

// 二元表达式节点
struct BinaryExpression : public Expression {
    Expression* left;
    Expression* right;
    Operator* operator_;
    
    BinaryExpression(Expression* l, Expression* r, Operator* op) : left(l), right(r), operator_(op) {}
    
    string getLocation() const override {
        return "binary expression";
    }
    
    void accept(ASTVisitor* visitor) override;
    
    int getTypePriority() const override {
        int leftPriority = left ? left->getTypePriority() : 0;
        int rightPriority = right ? right->getTypePriority() : 0;
        return max(leftPriority, rightPriority);
    }
};

// 赋值表达式现在使用BinaryExpression，左操作数是变量名，右操作数是值
// 赋值运算符的优先级最低，右结合

// 类型转换表达式 - 支持Value类型转换
struct CastExpression : public Expression {
    Expression* operand;     // 被转换的表达式
    string targetType;       // 目标类型名称
    
    CastExpression(Expression* expr, const string& type) 
        : operand(expr), targetType(type) {}
    
    void accept(ASTVisitor* visitor) override;
    
    // 获取目标类型名称
    string getTargetTypeName() const {
        return targetType;
    }
    
    // 获取目标类型优先级
    int getTargetTypePriority() const {
        if (targetType == "int") return 2;
        if (targetType == "double") return 3;
        if (targetType == "bool") return 0;
        if (targetType == "char") return 1;
        if (targetType == "string") return 4;
        return 0;
    }
    
    int getTypePriority() const override {
        return getTargetTypePriority();
    }
    
    string getLocation() const override {
        return "cast to " + targetType;
    }
};

// ==================== 容器表达式 ====================
// 注意：StringLiteral、ArrayNode、DictNode已移动到value.h中作为CompositeValue类型

// ==================== 访问和调用表达式 ====================
// 访问表达式节点
struct AccessExpression : public Expression {
    Expression* target;
    Expression* key;
    
    AccessExpression(Expression* t, Expression* k) : target(t), key(k) {}
    
    string getLocation() const override {
        return "access expression";
    }
    
    void accept(ASTVisitor* visitor) override;
    
    int getTypePriority() const override {
        return target ? target->getTypePriority() : 0;
    }
};

// 函数调用表达式节点
struct CallExpression : public Expression {
    string functionName;
    vector<Expression*> arguments;
    
    CallExpression(const string& name, const vector<Expression*>& args) 
        : functionName(name), arguments(args) {}
    
    string getLocation() const override {
        return "function call: " + functionName;
    }
    
    void accept(ASTVisitor* visitor) override;
    
    int getTypePriority() const override {
        return 0;
    }
};

// ==================== 内置函数表达式 ====================
// 内置函数表达式
struct BuiltinFunctionExpression : public Expression {
    string functionName;
    vector<Expression*> arguments;
    
    BuiltinFunctionExpression(const string& name, const vector<Expression*>& args) 
        : functionName(name), arguments(args) {}
    
    string getLocation() const override {
        return "builtin function: " + functionName;
    }
    
    void accept(ASTVisitor* visitor) override;
    
    int getTypePriority() const override {
        return 0;
    }
};

// ==================== 结构体和类相关表达式 ====================
// 注意：StructInstantiationExpression和ClassInstantiationExpression已移除，使用CallExpression替代

// 成员访问表达式
struct MemberAccessExpression : public Expression {
    Expression* object;
    string memberName;
    
    MemberAccessExpression(Expression* obj, const string& member) 
        : object(obj), memberName(member) {}
    
    string getLocation() const override {
        return "member access: " + memberName;
    }
    
    void accept(ASTVisitor* visitor) override;
    
    int getTypePriority() const override {
        return object ? object->getTypePriority() : 0;
    }
};

// 方法调用表达式
struct MethodCallExpression : public Expression {
    Expression* object;
    string methodName;
    vector<Expression*> arguments;
    
    MethodCallExpression(Expression* obj, const string& method, const vector<Expression*>& args)
        : object(obj), methodName(method), arguments(args) {}
    
    string getLocation() const override {
        return "method call: " + methodName;
    }
    
    void accept(ASTVisitor* visitor) override;
    
    int getTypePriority() const override {
        return 0;
    }
};

// 类型转换模板特化已删除，使用Value类型的运算符重载替代

#endif // EXPRESSION_H
