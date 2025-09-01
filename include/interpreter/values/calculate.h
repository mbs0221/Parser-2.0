#ifndef INTERPRETER_CALCULATE_H
#define INTERPRETER_CALCULATE_H

#include "interpreter/values/value.h"
#include <string>
#include <vector>
#include <unordered_map>

// using namespace std; // 已移除，使用显式std前缀

// ==================== 操作符映射 ====================
// 从parser模块获取操作符到方法名的映射

    


// ==================== 类型转换优先级系统 ====================
namespace TypeConversionPriority {
    // 类型转换优先级（数值越大优先级越高，表示类型越大）
    static const std::unordered_map<std::string, int> CONVERSION_PRIORITY = {
        // 数值类型转换优先级
        {"bool", 1},     // 最小类型 - 布尔类型
        {"char", 2},     // 字符类型
        {"int", 3},      // 整数类型
        {"double", 4},   // 浮点类型（最大数值类型）
        {"string", 5},   // 字符串类型（最大基础类型）
        
        // 容器类型转换优先级
        {"array", 10},   // 数组类型
        {"dict", 11},    // 字典类型
        
        // 用户定义类型转换优先级
        {"struct", 20},  // 结构体类型
        {"class", 21},   // 类类型
        {"interface", 22} // 接口类型
    };
    
    // 获取类型转换优先级
    inline int getPriority(const std::string& typeName) {
        auto it = CONVERSION_PRIORITY.find(typeName);
        return it != CONVERSION_PRIORITY.end() ? it->second : 100; // 默认低优先级
    }
    
    // 检查类型转换的合理性
    inline bool isReasonableConversion(const std::string& fromType, const std::string& toType) {
        int fromPriority = getPriority(fromType);
        int toPriority = getPriority(toType);
        
        // 允许向上转换（优先级降低）和同级转换
        return toPriority >= fromPriority;
    }
    
    // 获取最佳中间类型
    inline std::string getBestIntermediateType(const std::string& fromType, const std::string& toType) {
        int fromPriority = getPriority(fromType);
        int toPriority = getPriority(toType);
        
        // 如果目标类型优先级更高，直接转换
        if (toPriority <= fromPriority) {
            return toType;
        }
        
        // 否则选择优先级最低的中间类型
        if (fromPriority <= 3 && toPriority >= 5) {
            return "string"; // 数值类型到字符串类型
        }
        
        return fromType; // 保持原类型
    }
}

// ==================== 高级计算器 ====================

// 前向声明
class Interpreter;

// 高级计算器 - 完全基于类型系统实现所有运算
class Calculator {
public:
    // 构造函数，接收解释器依赖
    Calculator(Interpreter* interpreter) : interpreter_(interpreter) {}
    
    // 执行二元运算
    Value* executeBinaryOperation(Value* left, Value* right, Operator* op);
    
    // 执行一元运算
    Value* executeUnaryOperation(Value* operand, Operator* op);
    
    // 执行三元运算
    Value* executeTernaryOperation(Value* condition, Value* trueValue, Value* falseValue);

protected:
    // 执行基础类型的一元运算（使用运算符重载）
    Value* executeBasicTypeUnaryOperation(Value* operand, Operator* op);

    // 执行基础类型的二元运算（使用运算符重载）
    Value* performBasicTypeBinaryOperation(Value* left, Value* right, Operator* op);

    // 执行用户类型的一元运算（使用类型系统方法）
    Value* performUserTypeUnaryOperation(Value* operand, Operator* op);

    // 执行用户类型的二元运算（使用类型系统方法）
    Value* performUserTypeBinaryOperation(Value* left, Value* right, Operator* op);

    // 辅助函数
    // 检查是否为基础类型
    bool isBasicType(const string& typeName);

    // 确定兼容类型（选择优先级更高的类型）
    string determineCompatibleType(Value* left, Value* right);

    // 创建类型转换表达式
    CastExpression* createCastExpression(Value* value, const string& targetTypeName);

    // 获取类型名称
    string getTypeName(Value* value);

    // 检查类型转换可能性
    bool canConvertTo(ObjectType* sourceType, ObjectType* targetType);

private:
    Interpreter* interpreter_; // 解释器依赖
};

#endif // INTERPRETER_CALCULATE_H 