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

// 特化实现
template<>
string ConstantExpression<int>::getLocation() const {
    return to_string(value);
}

template<>
string ConstantExpression<double>::getLocation() const {
    return to_string(value);
}

template<>
string ConstantExpression<bool>::getLocation() const {
    return value ? "true" : "false";
}

template<>
string ConstantExpression<char>::getLocation() const {
    return string(1, value);
}

template<>
string ConstantExpression<string>::getLocation() const {
    return "\"" + value + "\"";
}

// 类型名称特化
template<>
string ConstantExpression<int>::getTypeName() const {
    return "int";
}

template<>
string ConstantExpression<double>::getTypeName() const {
    return "double";
}

template<>
string ConstantExpression<bool>::getTypeName() const {
    return "bool";
}

template<>
string ConstantExpression<char>::getTypeName() const {
    return "char";
}

template<>
string ConstantExpression<string>::getTypeName() const {
    return "string";
}



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
};

// 赋值表达式节点 - 继承自BinaryExpression
struct AssignExpression : public BinaryExpression {
    AssignExpression(Expression* l, Expression* r) : BinaryExpression(l, r, Operator::Assign) {}
    
    ~AssignExpression() {
        // 基类 BinaryExpression 的析构函数会处理 left 和 right
        // 不需要删除 operator_，因为它是静态分配的
    }
    
    string getLocation() const override {
        return "assignment expression";
    }
    
    // 重写泛型accept方法声明
    template<typename ReturnType>
    ReturnType accept(ExpressionVisitor<ReturnType>* visitor);
    
    int getTypePriority() const override {
        return 1;  // 赋值操作优先级最低
    }
};

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

// 函数调用表达式节点
struct CallExpression : public Expression {
    string functionName;
    vector<Expression*> arguments;
    
    CallExpression(const string& name, const vector<Expression*>& args) 
        : functionName(name), arguments(args) {}
    
    ~CallExpression() {
        for (Expression* arg : arguments) {
            if (arg) {
                delete arg;
            }
        }
        arguments.clear();
    }
    
    string getLocation() const override {
        return "function call: " + functionName;
    }
    
    // 重写泛型accept方法声明
    template<typename ReturnType>
    ReturnType accept(ExpressionVisitor<ReturnType>* visitor);
    
    int getTypePriority() const override {
        return 0;
    }
};

// 方法调用表达式节点 - 继承自CallExpression
struct MethodCallExpression : public CallExpression {
    Expression* object;  // 调用方法的对象
    
    MethodCallExpression(Expression* obj, const string& methodName, const vector<Expression*>& args) 
        : CallExpression(methodName, args), object(obj) {}
    
    ~MethodCallExpression() {
        if (object) {
            delete object;
            object = nullptr;
        }
    }
    
    string getLocation() const override {
        return "method call: " + functionName;
    }
    
    // 重写泛型accept方法声明
    template<typename ReturnType>
    ReturnType accept(ExpressionVisitor<ReturnType>* visitor);
    
    int getTypePriority() const override {
        return 0;
    }
};



// ==================== 结构体和类相关表达式 ====================
// MemberAccessExpression已合并到AccessExpression中



// 类型转换模板特化已删除，使用Value类型的运算符重载替代

#endif // EXPRESSION_H
