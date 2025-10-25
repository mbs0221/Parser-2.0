#ifndef PATTERN_MATCHING_H
#define PATTERN_MATCHING_H

#include "interpreter/values/value.h"
#include "interpreter/types/types.h"
#include "parser/advanced_expressions.h"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

namespace InterpreterCore {

// 模式匹配结果
struct MatchResult {
    bool matched;
    std::map<std::string, Value*> bindings;
    std::string errorMessage;
    
    MatchResult() : matched(false) {}
    MatchResult(bool m) : matched(m) {}
    MatchResult(bool m, const std::map<std::string, Value*>& b) 
        : matched(m), bindings(b) {}
    MatchResult(bool m, const std::string& error) 
        : matched(m), errorMessage(error) {}
};

// 模式基类
class Pattern {
public:
    virtual ~Pattern() = default;
    
    // 匹配模式
    virtual MatchResult match(Value* value) = 0;
    
    // 获取模式类型
    virtual std::string getPatternType() const = 0;
    
    // 获取模式描述
    virtual std::string toString() const = 0;
    
    // 检查模式是否包含变量绑定
    virtual bool hasBindings() const = 0;
    
    // 获取模式中的变量名
    virtual std::vector<std::string> getVariables() const = 0;
};

// 字面量模式
class LiteralPattern : public Pattern {
public:
    LiteralPattern(Value* literal);
    ~LiteralPattern();
    
    MatchResult match(Value* value) override;
    std::string getPatternType() const override { return "Literal"; }
    std::string toString() const override;
    bool hasBindings() const override { return false; }
    std::vector<std::string> getVariables() const override { return {}; }

private:
    Value* literal;
};

// 变量模式
class VariablePattern : public Pattern {
public:
    VariablePattern(const std::string& name, Type* type = nullptr);
    ~VariablePattern();
    
    MatchResult match(Value* value) override;
    std::string getPatternType() const override { return "Variable"; }
    std::string toString() const override;
    bool hasBindings() const override { return true; }
    std::vector<std::string> getVariables() const override { return {name}; }

private:
    std::string name;
    Type* type;
};

// 通配符模式
class WildcardPattern : public Pattern {
public:
    WildcardPattern();
    ~WildcardPattern();
    
    MatchResult match(Value* value) override;
    std::string getPatternType() const override { return "Wildcard"; }
    std::string toString() const override { return "_"; }
    bool hasBindings() const override { return false; }
    std::vector<std::string> getVariables() const override { return {}; }
};

// 类型模式
class TypePattern : public Pattern {
public:
    TypePattern(Type* type);
    ~TypePattern();
    
    MatchResult match(Value* value) override;
    std::string getPatternType() const override { return "Type"; }
    std::string toString() const override;
    bool hasBindings() const override { return false; }
    std::vector<std::string> getVariables() const override { return {}; }

private:
    Type* type;
};

// 数组模式
class ArrayPattern : public Pattern {
public:
    ArrayPattern(const std::vector<Pattern*>& elements, bool rest = false);
    ~ArrayPattern();
    
    MatchResult match(Value* value) override;
    std::string getPatternType() const override { return "Array"; }
    std::string toString() const override;
    bool hasBindings() const override;
    std::vector<std::string> getVariables() const override;

private:
    std::vector<Pattern*> elements;
    bool hasRest;
};

// 字典模式
class DictPattern : public Pattern {
public:
    DictPattern(const std::map<std::string, Pattern*>& fields, bool rest = false);
    ~DictPattern();
    
    MatchResult match(Value* value) override;
    std::string getPatternType() const override { return "Dict"; }
    std::string toString() const override;
    bool hasBindings() const override;
    std::vector<std::string> getVariables() const override;

private:
    std::map<std::string, Pattern*> fields;
    bool hasRest;
};

// 结构体模式
class StructPattern : public Pattern {
public:
    StructPattern(const std::string& typeName, const std::map<std::string, Pattern*>& fields);
    ~StructPattern();
    
    MatchResult match(Value* value) override;
    std::string getPatternType() const override { return "Struct"; }
    std::string toString() const override;
    bool hasBindings() const override;
    std::vector<std::string> getVariables() const override;

private:
    std::string typeName;
    std::map<std::string, Pattern*> fields;
};

// 范围模式
class RangePattern : public Pattern {
public:
    RangePattern(Value* start, Value* end, bool inclusive = true);
    ~RangePattern();
    
    MatchResult match(Value* value) override;
    std::string getPatternType() const override { return "Range"; }
    std::string toString() const override;
    bool hasBindings() const override { return false; }
    std::vector<std::string> getVariables() const override { return {}; }

private:
    Value* start;
    Value* end;
    bool inclusive;
};

// 守卫模式
class GuardPattern : public Pattern {
public:
    GuardPattern(Pattern* pattern, std::function<bool(const std::map<std::string, Value*>&)> guard);
    ~GuardPattern();
    
    MatchResult match(Value* value) override;
    std::string getPatternType() const override { return "Guard"; }
    std::string toString() const override;
    bool hasBindings() const override { return pattern->hasBindings(); }
    std::vector<std::string> getVariables() const override { return pattern->getVariables(); }

private:
    Pattern* pattern;
    std::function<bool(const std::map<std::string, Value*>&)> guard;
};

// 或模式
class OrPattern : public Pattern {
public:
    OrPattern(const std::vector<Pattern*>& patterns);
    ~OrPattern();
    
    MatchResult match(Value* value) override;
    std::string getPatternType() const override { return "Or"; }
    std::string toString() const override;
    bool hasBindings() const override;
    std::vector<std::string> getVariables() const override;

private:
    std::vector<Pattern*> patterns;
};

// 与模式
class AndPattern : public Pattern {
public:
    AndPattern(const std::vector<Pattern*>& patterns);
    ~AndPattern();
    
    MatchResult match(Value* value) override;
    std::string getPatternType() const override { return "And"; }
    std::string toString() const override;
    bool hasBindings() const override;
    std::vector<std::string> getVariables() const override;

private:
    std::vector<Pattern*> patterns;
};

// 模式匹配器
class PatternMatcher {
public:
    PatternMatcher();
    ~PatternMatcher();
    
    // 匹配单个模式
    MatchResult match(Pattern* pattern, Value* value);
    
    // 匹配模式列表（返回第一个匹配的模式）
    MatchResult matchFirst(const std::vector<Pattern*>& patterns, Value* value);
    
    // 匹配所有模式
    std::vector<MatchResult> matchAll(const std::vector<Pattern*>& patterns, Value* value);
    
    // 创建模式
    Pattern* createLiteralPattern(Value* literal);
    Pattern* createVariablePattern(const std::string& name, Type* type = nullptr);
    Pattern* createWildcardPattern();
    Pattern* createTypePattern(Type* type);
    Pattern* createArrayPattern(const std::vector<Pattern*>& elements, bool rest = false);
    Pattern* createDictPattern(const std::map<std::string, Pattern*>& fields, bool rest = false);
    Pattern* createStructPattern(const std::string& typeName, const std::map<std::string, Pattern*>& fields);
    Pattern* createRangePattern(Value* start, Value* end, bool inclusive = true);
    Pattern* createGuardPattern(Pattern* pattern, std::function<bool(const std::map<std::string, Value*>&)> guard);
    Pattern* createOrPattern(const std::vector<Pattern*>& patterns);
    Pattern* createAndPattern(const std::vector<Pattern*>& patterns);
    
    // 模式优化
    void optimizePattern(Pattern* pattern);
    
    // 模式验证
    bool validatePattern(Pattern* pattern);

private:
    // 辅助方法
    bool isArrayValue(Value* value);
    bool isDictValue(Value* value);
    bool isStructValue(Value* value);
    bool isNumericValue(Value* value);
    
    // 数值比较
    bool compareValues(Value* a, Value* b, const std::string& op);
    bool isInRange(Value* value, Value* start, Value* end, bool inclusive);
};

// 模式匹配表达式求值器
class MatchExpressionEvaluator {
public:
    MatchExpressionEvaluator(PatternMatcher* matcher);
    ~MatchExpressionEvaluator();
    
    // 求值MatchExpression
    Value* evaluate(MatchExpression* expr, Value* input);
    
    // 求值模式分支
    Value* evaluateBranch(const std::vector<Pattern*>& patterns, 
                         const std::vector<Expression*>& expressions, 
                         Value* input);

private:
    PatternMatcher* matcher;
};

} // namespace InterpreterCore

#endif // PATTERN_MATCHING_H
