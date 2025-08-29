#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "parser/statement.h"
#include "lexer/value.h"

using namespace std;

// ==================== 表达式基类 ====================
// 表达式基类 - 继承自Statement，这样表达式可以作为语句使用
struct Expression : public Statement {
    // 访问者模式：接受AST访问者
    virtual void accept(ASTVisitor* visitor) override = 0;
    virtual int getTypePriority() const = 0;
};

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
        if (value) {
            // 只有Char类型通过享元模式管理，不删除
            // Integer和Double可能是作用域中的常量，需要正常删除
            if (dynamic_cast<Char*>(value)) {
                // Char对象通过享元模式管理，不删除
                value = nullptr;
            } else {
                delete value;
                value = nullptr;
            }
        }
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
    
    ~VariableExpression() {
        // VariableExpression 没有需要清理的指针成员
    }
    
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
    
    void accept(ASTVisitor* visitor) override;
    
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
    
    void accept(ASTVisitor* visitor) override;
    
    int getTypePriority() const override {
        return 1;  // 赋值操作优先级最低
    }
};

// 类型转换表达式 - 支持Value类型转换
// 类型转换表达式 - 支持Value类型转换
template<typename T>
struct CastExpression : public Expression {
    Expression* operand;     // 被转换的表达式
    
    CastExpression(Expression* expr) : operand(expr) {}
    
    ~CastExpression() {
        if (operand) {
            delete operand;
            operand = nullptr;
        }
    }
    
    void accept(ASTVisitor* visitor) override;
    
    // 获取目标类型名称
    string getTargetTypeName() const {
        if (std::is_same<T, Integer>::value) return "int";
        else if (std::is_same<T, Double>::value) return "double";
        else if (std::is_same<T, Bool>::value) return "bool";
        else if (std::is_same<T, Char>::value) return "char";
        else if (std::is_same<T, String>::value) return "string";
        else return "unknown";
    }
    
    // 获取目标类型对象
    Type* getTargetType() const {
        if (std::is_same<T, Integer>::value) return Type::Int;
        else if (std::is_same<T, Double>::value) return Type::Double;
        else if (std::is_same<T, Bool>::value) return Type::Bool;
        else if (std::is_same<T, Char>::value) return Type::Char;
        else if (std::is_same<T, String>::value) return Type::String;
        else return nullptr;
    }
    
    // 获取目标类型优先级
    int getTargetTypePriority() const {
        if (std::is_same<T, Integer>::value) return 2;
        else if (std::is_same<T, Double>::value) return 3;
        else if (std::is_same<T, Bool>::value) return 0;
        else if (std::is_same<T, Char>::value) return 1;
        else if (std::is_same<T, String>::value) return 4;
        else return 0;
    }
    
    int getTypePriority() const override {
        return getTargetTypePriority();
    }
    
    string getLocation() const override {
        return "cast to " + getTargetTypeName();
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
    
    ~BuiltinFunctionExpression() {
        for (Expression* arg : arguments) {
            if (arg) {
                delete arg;
            }
        }
        arguments.clear();
    }
    
    string getLocation() const override {
        return "builtin function: " + functionName;
    }
    
    void accept(ASTVisitor* visitor) override;
    
    int getTypePriority() const override {
        return 0;
    }
};

// ==================== 结构体和类相关表达式 ====================
// MemberAccessExpression已合并到AccessExpression中

// 方法调用表达式
struct MethodCallExpression : public Expression {
    Expression* object;
    string methodName;
    vector<Expression*> arguments;
    
    MethodCallExpression(Expression* obj, const string& method, const vector<Expression*>& args)
        : object(obj), methodName(method), arguments(args) {}
    
    ~MethodCallExpression() {
        if (object) {
            delete object;
            object = nullptr;
        }
        for (Expression* arg : arguments) {
            if (arg) {
                delete arg;
            }
        }
        arguments.clear();
    }
    
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
