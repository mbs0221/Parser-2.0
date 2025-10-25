#include "interpreter/core/pattern_matching.h"
#include "interpreter/values/value.h"
#include "interpreter/values/container_values.h"
#include "common/logger.h"
#include <algorithm>
#include <sstream>

namespace InterpreterCore {

// ==================== LiteralPattern 实现 ====================

LiteralPattern::LiteralPattern(Value* literal) : literal(literal) {
}

LiteralPattern::~LiteralPattern() {
    // 注意：不删除literal，因为它可能被其他地方使用
}

MatchResult LiteralPattern::match(Value* value) {
    if (!value || !literal) {
        return MatchResult(false, "LiteralPattern: 值或字面量为空");
    }
    
    if (value->equals(literal)) {
        return MatchResult(true);
    } else {
        return MatchResult(false);
    }
}

std::string LiteralPattern::toString() const {
    return literal ? literal->toString() : "null";
}

// ==================== VariablePattern 实现 ====================

VariablePattern::VariablePattern(const std::string& name, Type* type) 
    : name(name), type(type) {
}

VariablePattern::~VariablePattern() {
    // 注意：不删除type，因为它可能被其他地方使用
}

MatchResult VariablePattern::match(Value* value) {
    if (!value) {
        return MatchResult(false, "VariablePattern: 值为空");
    }
    
    // 检查类型约束
    if (type && !value->getType()->equals(type)) {
        return MatchResult(false, "VariablePattern: 类型不匹配");
    }
    
    std::map<std::string, Value*> bindings;
    bindings[name] = value;
    return MatchResult(true, bindings);
}

std::string VariablePattern::toString() const {
    std::string result = name;
    if (type) {
        result += " : " + type->toString();
    }
    return result;
}

// ==================== WildcardPattern 实现 ====================

WildcardPattern::WildcardPattern() {
}

WildcardPattern::~WildcardPattern() {
}

MatchResult WildcardPattern::match(Value* value) {
    // 通配符模式总是匹配
    return MatchResult(true);
}

// ==================== TypePattern 实现 ====================

TypePattern::TypePattern(Type* type) : type(type) {
}

TypePattern::~TypePattern() {
    // 注意：不删除type，因为它可能被其他地方使用
}

MatchResult TypePattern::match(Value* value) {
    if (!value || !type) {
        return MatchResult(false, "TypePattern: 值或类型为空");
    }
    
    if (value->getType()->equals(type) || value->getType()->isSubtypeOf(type)) {
        return MatchResult(true);
    } else {
        return MatchResult(false);
    }
}

std::string TypePattern::toString() const {
    return type ? type->toString() : "null";
}

// ==================== ArrayPattern 实现 ====================

ArrayPattern::ArrayPattern(const std::vector<Pattern*>& elements, bool rest) 
    : elements(elements), hasRest(rest) {
}

ArrayPattern::~ArrayPattern() {
    // 注意：不删除elements中的Pattern*，因为它们可能被其他地方使用
}

MatchResult ArrayPattern::match(Value* value) {
    if (!value) {
        return MatchResult(false, "ArrayPattern: 值为空");
    }
    
    // 检查是否为数组类型
    if (!isArrayValue(value)) {
        return MatchResult(false, "ArrayPattern: 值不是数组类型");
    }
    
    Array* array = dynamic_cast<Array*>(value);
    if (!array) {
        return MatchResult(false, "ArrayPattern: 无法转换为数组");
    }
    
    std::map<std::string, Value*> bindings;
    
    // 检查数组长度
    if (!hasRest && array->size() != elements.size()) {
        return MatchResult(false, "ArrayPattern: 数组长度不匹配");
    }
    
    if (hasRest && array->size() < elements.size()) {
        return MatchResult(false, "ArrayPattern: 数组长度不足");
    }
    
    // 匹配每个元素
    for (size_t i = 0; i < elements.size(); ++i) {
        Value* element = array->get(i);
        MatchResult result = elements[i]->match(element);
        
        if (!result.matched) {
            return MatchResult(false, "ArrayPattern: 元素 " + std::to_string(i) + " 匹配失败");
        }
        
        // 合并绑定
        for (const auto& binding : result.bindings) {
            bindings[binding.first] = binding.second;
        }
    }
    
    return MatchResult(true, bindings);
}

std::string ArrayPattern::toString() const {
    std::ostringstream oss;
    oss << "[";
    
    for (size_t i = 0; i < elements.size(); ++i) {
        if (i > 0) {
            oss << ", ";
        }
        oss << elements[i]->toString();
    }
    
    if (hasRest) {
        oss << "...";
    }
    
    oss << "]";
    return oss.str();
}

bool ArrayPattern::hasBindings() const {
    for (Pattern* pattern : elements) {
        if (pattern->hasBindings()) {
            return true;
        }
    }
    return false;
}

std::vector<std::string> ArrayPattern::getVariables() const {
    std::vector<std::string> variables;
    for (Pattern* pattern : elements) {
        auto patternVars = pattern->getVariables();
        variables.insert(variables.end(), patternVars.begin(), patternVars.end());
    }
    return variables;
}

// ==================== DictPattern 实现 ====================

DictPattern::DictPattern(const std::map<std::string, Pattern*>& fields, bool rest) 
    : fields(fields), hasRest(rest) {
}

DictPattern::~DictPattern() {
    // 注意：不删除fields中的Pattern*，因为它们可能被其他地方使用
}

MatchResult DictPattern::match(Value* value) {
    if (!value) {
        return MatchResult(false, "DictPattern: 值为空");
    }
    
    // 检查是否为字典类型
    if (!isDictValue(value)) {
        return MatchResult(false, "DictPattern: 值不是字典类型");
    }
    
    Dict* dict = dynamic_cast<Dict*>(value);
    if (!dict) {
        return MatchResult(false, "DictPattern: 无法转换为字典");
    }
    
    std::map<std::string, Value*> bindings;
    
    // 匹配每个字段
    for (const auto& field : fields) {
        Value* fieldValue = dict->get(field.first);
        if (!fieldValue) {
            return MatchResult(false, "DictPattern: 缺少字段 " + field.first);
        }
        
        MatchResult result = field.second->match(fieldValue);
        if (!result.matched) {
            return MatchResult(false, "DictPattern: 字段 " + field.first + " 匹配失败");
        }
        
        // 合并绑定
        for (const auto& binding : result.bindings) {
            bindings[binding.first] = binding.second;
        }
    }
    
    return MatchResult(true, bindings);
}

std::string DictPattern::toString() const {
    std::ostringstream oss;
    oss << "{";
    
    bool first = true;
    for (const auto& field : fields) {
        if (!first) {
            oss << ", ";
        }
        oss << field.first << ": " << field.second->toString();
        first = false;
    }
    
    if (hasRest) {
        oss << "...";
    }
    
    oss << "}";
    return oss.str();
}

bool DictPattern::hasBindings() const {
    for (const auto& field : fields) {
        if (field.second->hasBindings()) {
            return true;
        }
    }
    return false;
}

std::vector<std::string> DictPattern::getVariables() const {
    std::vector<std::string> variables;
    for (const auto& field : fields) {
        auto patternVars = field.second->getVariables();
        variables.insert(variables.end(), patternVars.begin(), patternVars.end());
    }
    return variables;
}

// ==================== StructPattern 实现 ====================

StructPattern::StructPattern(const std::string& typeName, const std::map<std::string, Pattern*>& fields) 
    : typeName(typeName), fields(fields) {
}

StructPattern::~StructPattern() {
    // 注意：不删除fields中的Pattern*，因为它们可能被其他地方使用
}

MatchResult StructPattern::match(Value* value) {
    if (!value) {
        return MatchResult(false, "StructPattern: 值为空");
    }
    
    // 检查类型名称
    if (value->getType()->toString() != typeName) {
        return MatchResult(false, "StructPattern: 类型不匹配，期望 " + typeName + "，得到 " + value->getType()->toString());
    }
    
    // 检查是否为结构体类型
    if (!isStructValue(value)) {
        return MatchResult(false, "StructPattern: 值不是结构体类型");
    }
    
    // 这里需要根据实际的结构体实现来访问字段
    // 简化实现：假设结构体有getField方法
    std::map<std::string, Value*> bindings;
    
    for (const auto& field : fields) {
        // 这里需要根据实际的结构体实现来获取字段值
        // Value* fieldValue = struct->getField(field.first);
        // 简化实现：跳过字段匹配
        MatchResult result = field.second->match(nullptr);
        if (!result.matched) {
            return MatchResult(false, "StructPattern: 字段 " + field.first + " 匹配失败");
        }
        
        // 合并绑定
        for (const auto& binding : result.bindings) {
            bindings[binding.first] = binding.second;
        }
    }
    
    return MatchResult(true, bindings);
}

std::string StructPattern::toString() const {
    std::ostringstream oss;
    oss << typeName << " {";
    
    bool first = true;
    for (const auto& field : fields) {
        if (!first) {
            oss << ", ";
        }
        oss << field.first << ": " << field.second->toString();
        first = false;
    }
    
    oss << "}";
    return oss.str();
}

bool StructPattern::hasBindings() const {
    for (const auto& field : fields) {
        if (field.second->hasBindings()) {
            return true;
        }
    }
    return false;
}

std::vector<std::string> StructPattern::getVariables() const {
    std::vector<std::string> variables;
    for (const auto& field : fields) {
        auto patternVars = field.second->getVariables();
        variables.insert(variables.end(), patternVars.begin(), patternVars.end());
    }
    return variables;
}

// ==================== RangePattern 实现 ====================

RangePattern::RangePattern(Value* start, Value* end, bool inclusive) 
    : start(start), end(end), inclusive(inclusive) {
}

RangePattern::~RangePattern() {
    // 注意：不删除start和end，因为它们可能被其他地方使用
}

MatchResult RangePattern::match(Value* value) {
    if (!value || !start || !end) {
        return MatchResult(false, "RangePattern: 值、开始或结束为空");
    }
    
    if (!isNumericValue(value)) {
        return MatchResult(false, "RangePattern: 值不是数值类型");
    }
    
    if (isInRange(value, start, end, inclusive)) {
        return MatchResult(true);
    } else {
        return MatchResult(false);
    }
}

std::string RangePattern::toString() const {
    std::ostringstream oss;
    oss << (start ? start->toString() : "null");
    oss << (inclusive ? ".." : "..<");
    oss << (end ? end->toString() : "null");
    return oss.str();
}

// ==================== GuardPattern 实现 ====================

GuardPattern::GuardPattern(Pattern* pattern, std::function<bool(const std::map<std::string, Value*>&)> guard) 
    : pattern(pattern), guard(guard) {
}

GuardPattern::~GuardPattern() {
    // 注意：不删除pattern，因为它可能被其他地方使用
}

MatchResult GuardPattern::match(Value* value) {
    if (!pattern) {
        return MatchResult(false, "GuardPattern: 模式为空");
    }
    
    MatchResult result = pattern->match(value);
    if (!result.matched) {
        return result;
    }
    
    // 检查守卫条件
    if (guard && !guard(result.bindings)) {
        return MatchResult(false, "GuardPattern: 守卫条件不满足");
    }
    
    return result;
}

std::string GuardPattern::toString() const {
    return pattern ? (pattern->toString() + " if condition") : "null if condition";
}

// ==================== OrPattern 实现 ====================

OrPattern::OrPattern(const std::vector<Pattern*>& patterns) : patterns(patterns) {
}

OrPattern::~OrPattern() {
    // 注意：不删除patterns中的Pattern*，因为它们可能被其他地方使用
}

MatchResult OrPattern::match(Value* value) {
    if (patterns.empty()) {
        return MatchResult(false, "OrPattern: 没有模式");
    }
    
    for (Pattern* pattern : patterns) {
        MatchResult result = pattern->match(value);
        if (result.matched) {
            return result;
        }
    }
    
    return MatchResult(false, "OrPattern: 所有模式都不匹配");
}

std::string OrPattern::toString() const {
    std::ostringstream oss;
    oss << "(";
    
    for (size_t i = 0; i < patterns.size(); ++i) {
        if (i > 0) {
            oss << " | ";
        }
        oss << patterns[i]->toString();
    }
    
    oss << ")";
    return oss.str();
}

bool OrPattern::hasBindings() const {
    for (Pattern* pattern : patterns) {
        if (pattern->hasBindings()) {
            return true;
        }
    }
    return false;
}

std::vector<std::string> OrPattern::getVariables() const {
    std::vector<std::string> variables;
    for (Pattern* pattern : patterns) {
        auto patternVars = pattern->getVariables();
        variables.insert(variables.end(), patternVars.begin(), patternVars.end());
    }
    return variables;
}

// ==================== AndPattern 实现 ====================

AndPattern::AndPattern(const std::vector<Pattern*>& patterns) : patterns(patterns) {
}

AndPattern::~AndPattern() {
    // 注意：不删除patterns中的Pattern*，因为它们可能被其他地方使用
}

MatchResult AndPattern::match(Value* value) {
    if (patterns.empty()) {
        return MatchResult(false, "AndPattern: 没有模式");
    }
    
    std::map<std::string, Value*> bindings;
    
    for (Pattern* pattern : patterns) {
        MatchResult result = pattern->match(value);
        if (!result.matched) {
            return MatchResult(false, "AndPattern: 模式匹配失败");
        }
        
        // 合并绑定
        for (const auto& binding : result.bindings) {
            bindings[binding.first] = binding.second;
        }
    }
    
    return MatchResult(true, bindings);
}

std::string AndPattern::toString() const {
    std::ostringstream oss;
    oss << "(";
    
    for (size_t i = 0; i < patterns.size(); ++i) {
        if (i > 0) {
            oss << " & ";
        }
        oss << patterns[i]->toString();
    }
    
    oss << ")";
    return oss.str();
}

bool AndPattern::hasBindings() const {
    for (Pattern* pattern : patterns) {
        if (pattern->hasBindings()) {
            return true;
        }
    }
    return false;
}

std::vector<std::string> AndPattern::getVariables() const {
    std::vector<std::string> variables;
    for (Pattern* pattern : patterns) {
        auto patternVars = pattern->getVariables();
        variables.insert(variables.end(), patternVars.begin(), patternVars.end());
    }
    return variables;
}

// ==================== PatternMatcher 实现 ====================

PatternMatcher::PatternMatcher() {
}

PatternMatcher::~PatternMatcher() {
}

MatchResult PatternMatcher::match(Pattern* pattern, Value* value) {
    if (!pattern) {
        return MatchResult(false, "PatternMatcher: 模式为空");
    }
    
    return pattern->match(value);
}

MatchResult PatternMatcher::matchFirst(const std::vector<Pattern*>& patterns, Value* value) {
    for (Pattern* pattern : patterns) {
        MatchResult result = match(pattern, value);
        if (result.matched) {
            return result;
        }
    }
    
    return MatchResult(false, "PatternMatcher: 没有模式匹配");
}

std::vector<MatchResult> PatternMatcher::matchAll(const std::vector<Pattern*>& patterns, Value* value) {
    std::vector<MatchResult> results;
    
    for (Pattern* pattern : patterns) {
        MatchResult result = match(pattern, value);
        results.push_back(result);
    }
    
    return results;
}

Pattern* PatternMatcher::createLiteralPattern(Value* literal) {
    return new LiteralPattern(literal);
}

Pattern* PatternMatcher::createVariablePattern(const std::string& name, Type* type) {
    return new VariablePattern(name, type);
}

Pattern* PatternMatcher::createWildcardPattern() {
    return new WildcardPattern();
}

Pattern* PatternMatcher::createTypePattern(Type* type) {
    return new TypePattern(type);
}

Pattern* PatternMatcher::createArrayPattern(const std::vector<Pattern*>& elements, bool rest) {
    return new ArrayPattern(elements, rest);
}

Pattern* PatternMatcher::createDictPattern(const std::map<std::string, Pattern*>& fields, bool rest) {
    return new DictPattern(fields, rest);
}

Pattern* PatternMatcher::createStructPattern(const std::string& typeName, const std::map<std::string, Pattern*>& fields) {
    return new StructPattern(typeName, fields);
}

Pattern* PatternMatcher::createRangePattern(Value* start, Value* end, bool inclusive) {
    return new RangePattern(start, end, inclusive);
}

Pattern* PatternMatcher::createGuardPattern(Pattern* pattern, std::function<bool(const std::map<std::string, Value*>&)> guard) {
    return new GuardPattern(pattern, guard);
}

Pattern* PatternMatcher::createOrPattern(const std::vector<Pattern*>& patterns) {
    return new OrPattern(patterns);
}

Pattern* PatternMatcher::createAndPattern(const std::vector<Pattern*>& patterns) {
    return new AndPattern(patterns);
}

void PatternMatcher::optimizePattern(Pattern* pattern) {
    // 模式优化逻辑
    // 这里可以实现各种优化策略
}

bool PatternMatcher::validatePattern(Pattern* pattern) {
    if (!pattern) {
        return false;
    }
    
    // 验证模式的逻辑
    return true;
}

bool PatternMatcher::isArrayValue(Value* value) {
    return dynamic_cast<Array*>(value) != nullptr;
}

bool PatternMatcher::isDictValue(Value* value) {
    return dynamic_cast<Dict*>(value) != nullptr;
}

bool PatternMatcher::isStructValue(Value* value) {
    // 这里需要根据实际的结构体实现来判断
    return false;
}

bool PatternMatcher::isNumericValue(Value* value) {
    return dynamic_cast<Integer*>(value) != nullptr || 
           dynamic_cast<Double*>(value) != nullptr;
}

bool PatternMatcher::compareValues(Value* a, Value* b, const std::string& op) {
    if (!a || !b) {
        return false;
    }
    
    // 简化的值比较
    return a->equals(b);
}

bool PatternMatcher::isInRange(Value* value, Value* start, Value* end, bool inclusive) {
    if (!value || !start || !end) {
        return false;
    }
    
    // 简化的范围检查
    // 这里需要根据实际的数值类型实现
    return true;
}

// ==================== MatchExpressionEvaluator 实现 ====================

MatchExpressionEvaluator::MatchExpressionEvaluator(PatternMatcher* matcher) : matcher(matcher) {
}

MatchExpressionEvaluator::~MatchExpressionEvaluator() {
    // 注意：不删除matcher，因为它可能被其他地方使用
}

Value* MatchExpressionEvaluator::evaluate(MatchExpression* expr, Value* input) {
    if (!expr || !input) {
        return nullptr;
    }
    
    // 这里需要根据实际的MatchExpression实现来求值
    // 简化实现：返回输入值
    return input;
}

Value* MatchExpressionEvaluator::evaluateBranch(const std::vector<Pattern*>& patterns, 
                                               const std::vector<Expression*>& expressions, 
                                               Value* input) {
    if (patterns.size() != expressions.size()) {
        return nullptr;
    }
    
    for (size_t i = 0; i < patterns.size(); ++i) {
        MatchResult result = matcher->match(patterns[i], input);
        if (result.matched) {
            // 这里需要根据实际的表达式求值实现
            // 简化实现：返回输入值
            return input;
        }
    }
    
    return nullptr;
}

} // namespace InterpreterCore
