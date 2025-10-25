#ifndef ADVANCED_EXPRESSIONS_H
#define ADVANCED_EXPRESSIONS_H

#include "parser/expression.h"
#include <vector>
#include <map>

// 高级表达式类型 - 扩展Parser的表达式支持

// ==================== 字面量表达式 ====================

// 数组字面量表达式
struct ArrayLiteralExpression : public Expression {
    std::vector<Expression*> elements;
    
    ArrayLiteralExpression(const std::vector<Expression*>& elems) : elements(elems) {}
    
    ~ArrayLiteralExpression() {
        for (Expression* elem : elements) {
            if (elem) {
                delete elem;
            }
        }
        elements.clear();
    }
    
    std::string getLocation() const override {
        return "array literal with " + std::to_string(elements.size()) + " elements";
    }
    
    template<typename ReturnType>
    ReturnType accept(ExpressionVisitor<ReturnType>* visitor);
    
    int getTypePriority() const override {
        return 0; // 字面量优先级最高
    }
    
    const std::vector<Expression*>& getElements() const {
        return elements;
    }
    
    size_t getElementCount() const {
        return elements.size();
    }
};

// 字典字面量表达式
struct DictLiteralExpression : public Expression {
    std::vector<std::pair<Expression*, Expression*>> pairs; // key-value pairs
    
    DictLiteralExpression(const std::vector<std::pair<Expression*, Expression*>>& kvPairs) 
        : pairs(kvPairs) {}
    
    ~DictLiteralExpression() {
        for (auto& pair : pairs) {
            if (pair.first) delete pair.first;
            if (pair.second) delete pair.second;
        }
        pairs.clear();
    }
    
    std::string getLocation() const override {
        return "dict literal with " + std::to_string(pairs.size()) + " pairs";
    }
    
    template<typename ReturnType>
    ReturnType accept(ExpressionVisitor<ReturnType>* visitor);
    
    int getTypePriority() const override {
        return 0; // 字面量优先级最高
    }
    
    const std::vector<std::pair<Expression*, Expression*>>& getPairs() const {
        return pairs;
    }
    
    size_t getPairCount() const {
        return pairs.size();
    }
};

// 集合字面量表达式
struct SetLiteralExpression : public Expression {
    std::vector<Expression*> elements;
    
    SetLiteralExpression(const std::vector<Expression*>& elems) : elements(elems) {}
    
    ~SetLiteralExpression() {
        for (Expression* elem : elements) {
            if (elem) {
                delete elem;
            }
        }
        elements.clear();
    }
    
    std::string getLocation() const override {
        return "set literal with " + std::to_string(elements.size()) + " elements";
    }
    
    template<typename ReturnType>
    ReturnType accept(ExpressionVisitor<ReturnType>* visitor);
    
    int getTypePriority() const override {
        return 0; // 字面量优先级最高
    }
    
    const std::vector<Expression*>& getElements() const {
        return elements;
    }
    
    size_t getElementCount() const {
        return elements.size();
    }
};

// ==================== 范围表达式 ====================

// 范围表达式 (start..end, start..<end, etc.)
struct RangeExpression : public Expression {
    Expression* start;
    Expression* end;
    bool inclusive; // true for .., false for ..<
    
    RangeExpression(Expression* s, Expression* e, bool incl = true) 
        : start(s), end(e), inclusive(incl) {}
    
    ~RangeExpression() {
        if (start) {
            delete start;
            start = nullptr;
        }
        if (end) {
            delete end;
            end = nullptr;
        }
    }
    
    std::string getLocation() const override {
        return "range expression";
    }
    
    template<typename ReturnType>
    ReturnType accept(ExpressionVisitor<ReturnType>* visitor);
    
    int getTypePriority() const override {
        return 0; // 字面量优先级最高
    }
    
    Expression* getStart() const { return start; }
    Expression* getEnd() const { return end; }
    bool isInclusive() const { return inclusive; }
};

// ==================== 切片表达式 ====================

// 切片表达式 (array[start:end:step])
struct SliceExpression : public Expression {
    Expression* target;  // 被切片的对象
    Expression* start;   // 起始索引
    Expression* end;     // 结束索引
    Expression* step;    // 步长
    
    SliceExpression(Expression* tgt, Expression* s, Expression* e, Expression* st = nullptr)
        : target(tgt), start(s), end(e), step(st) {}
    
    ~SliceExpression() {
        if (target) { delete target; target = nullptr; }
        if (start) { delete start; start = nullptr; }
        if (end) { delete end; end = nullptr; }
        if (step) { delete step; step = nullptr; }
    }
    
    std::string getLocation() const override {
        return "slice expression";
    }
    
    template<typename ReturnType>
    ReturnType accept(ExpressionVisitor<ReturnType>* visitor);
    
    int getTypePriority() const override {
        return target ? target->getTypePriority() : 0;
    }
    
    Expression* getTarget() const { return target; }
    Expression* getStart() const { return start; }
    Expression* getEnd() const { return end; }
    Expression* getStep() const { return step; }
};

// ==================== 条件表达式扩展 ====================

// 空值合并表达式 (value ?? defaultValue)
struct NullCoalescingExpression : public BinaryExpression {
    NullCoalescingExpression(Expression* left, Expression* right) 
        : BinaryExpression(left, right, nullptr) {}
    
    std::string getLocation() const override {
        return "null coalescing expression";
    }
    
    template<typename ReturnType>
    ReturnType accept(ExpressionVisitor<ReturnType>* visitor);
    
    int getTypePriority() const override {
        return 1; // 优先级较低
    }
};

// ==================== 模式匹配表达式 ====================

// 模式匹配表达式 (match value { pattern => result, ... })
struct MatchExpression : public Expression {
    Expression* value;
    std::vector<std::pair<Expression*, Expression*>> patterns; // pattern => result pairs
    
    MatchExpression(Expression* val, const std::vector<std::pair<Expression*, Expression*>>& pats)
        : value(val), patterns(pats) {}
    
    ~MatchExpression() {
        if (value) {
            delete value;
            value = nullptr;
        }
        for (auto& pattern : patterns) {
            if (pattern.first) delete pattern.first;
            if (pattern.second) delete pattern.second;
        }
        patterns.clear();
    }
    
    std::string getLocation() const override {
        return "match expression with " + std::to_string(patterns.size()) + " patterns";
    }
    
    template<typename ReturnType>
    ReturnType accept(ExpressionVisitor<ReturnType>* visitor);
    
    int getTypePriority() const override {
        return 0; // 字面量优先级最高
    }
    
    Expression* getValue() const { return value; }
    const std::vector<std::pair<Expression*, Expression*>>& getPatterns() const {
        return patterns;
    }
};

// ==================== 异步表达式 ====================

// 异步表达式 (async expression)
struct AsyncExpression : public Expression {
    Expression* expression;
    
    AsyncExpression(Expression* expr) : expression(expr) {}
    
    ~AsyncExpression() {
        if (expression) {
            delete expression;
            expression = nullptr;
        }
    }
    
    std::string getLocation() const override {
        return "async expression";
    }
    
    template<typename ReturnType>
    ReturnType accept(ExpressionVisitor<ReturnType>* visitor);
    
    int getTypePriority() const override {
        return expression ? expression->getTypePriority() : 0;
    }
    
    Expression* getExpression() const { return expression; }
};

// 等待表达式 (await expression)
struct AwaitExpression : public Expression {
    Expression* expression;
    
    AwaitExpression(Expression* expr) : expression(expr) {}
    
    ~AwaitExpression() {
        if (expression) {
            delete expression;
            expression = nullptr;
        }
    }
    
    std::string getLocation() const override {
        return "await expression";
    }
    
    template<typename ReturnType>
    ReturnType accept(ExpressionVisitor<ReturnType>* visitor);
    
    int getTypePriority() const override {
        return expression ? expression->getTypePriority() : 0;
    }
    
    Expression* getExpression() const { return expression; }
};

// ==================== 类型检查表达式 ====================

// 类型检查表达式 (value is Type)
struct TypeCheckExpression : public BinaryExpression {
    TypeCheckExpression(Expression* value, Expression* type) 
        : BinaryExpression(value, type, nullptr) {}
    
    std::string getLocation() const override {
        return "type check expression";
    }
    
    template<typename ReturnType>
    ReturnType accept(ExpressionVisitor<ReturnType>* visitor);
    
    int getTypePriority() const override {
        return 0; // 字面量优先级最高
    }
};

// 类型转换表达式 (value as Type)
struct TypeCastExpression : public BinaryExpression {
    TypeCastExpression(Expression* value, Expression* type) 
        : BinaryExpression(value, type, nullptr) {}
    
    std::string getLocation() const override {
        return "type cast expression";
    }
    
    template<typename ReturnType>
    ReturnType accept(ExpressionVisitor<ReturnType>* visitor);
    
    int getTypePriority() const override {
        return 0; // 字面量优先级最高
    }
};

// ==================== 复合赋值表达式 ====================

// 复合赋值表达式 (variable += value, variable *= value, etc.)
struct CompoundAssignExpression : public BinaryExpression {
    std::string operation; // "+=", "-=", "*=", "/=", etc.
    
    CompoundAssignExpression(Expression* left, Expression* right, const std::string& op)
        : BinaryExpression(left, right, nullptr), operation(op) {}
    
    std::string getLocation() const override {
        return "compound assign expression: " + operation;
    }
    
    template<typename ReturnType>
    ReturnType accept(ExpressionVisitor<ReturnType>* visitor);
    
    int getTypePriority() const override {
        return 0; // 赋值优先级最低
    }
    
    const std::string& getOperation() const { return operation; }
};

#endif // ADVANCED_EXPRESSIONS_H
