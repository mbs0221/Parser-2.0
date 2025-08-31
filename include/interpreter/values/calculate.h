#ifndef INTERPRETER_CALCULATE_H
#define INTERPRETER_CALCULATE_H

#include "interpreter/value.h"
#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

// ==================== 操作符映射 ====================
// 从parser模块获取操作符到方法名的映射

    


// ==================== 类型转换优先级系统 ====================
namespace TypeConversionPriority {
    // 类型转换优先级（数值越小优先级越高）
    static const unordered_map<string, int> CONVERSION_PRIORITY = {
        // 数值类型转换优先级
        {"int", 1},      // 最高优先级 - 基础整数类型
        {"char", 2},     // 字符类型
        {"double", 3},   // 浮点类型
        {"bool", 4},     // 布尔类型
        {"string", 5},   // 字符串类型
        
        // 容器类型转换优先级
        {"array", 10},   // 数组类型
        {"dict", 11},    // 字典类型
        
        // 用户定义类型转换优先级
        {"struct", 20},  // 结构体类型
        {"class", 21},   // 类类型
        {"interface", 22} // 接口类型
    };
    
    // 获取类型转换优先级
    inline int getPriority(const string& typeName) {
        auto it = CONVERSION_PRIORITY.find(typeName);
        return it != CONVERSION_PRIORITY.end() ? it->second : 100; // 默认低优先级
    }
    
    // 检查类型转换的合理性
    inline bool isReasonableConversion(const string& fromType, const string& toType) {
        int fromPriority = getPriority(fromType);
        int toPriority = getPriority(toType);
        
        // 允许向上转换（优先级降低）和同级转换
        return toPriority >= fromPriority;
    }
    
    // 获取最佳中间类型
    inline string getBestIntermediateType(const string& fromType, const string& toType) {
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

// 高级计算器 - 完全基于类型系统实现所有运算
class Calculator {
public:
    // 执行二元运算
    static Value* executeBinaryOperation(Value* left, Value* right, int op);
    
    // 执行一元运算
    static Value* executeUnaryOperation(Value* operand, int op);
    
    // 智能类型转换 - 自动选择最佳转换策略
    static Value* smartTypeConversion(Value* source, const string& targetType);
    
    // 基于继承关系的类型转换
    static Value* convertUsingInheritance(Value* source, const string& targetType);
    static Value* findCommonAncestor(ObjectType* type1, ObjectType* type2);
    static bool canConvertTo(ObjectType* sourceType, ObjectType* targetType);
    
    // 智能二元运算 - 自动处理类型转换
    static Value* smartBinaryOperation(Value* left, Value* right, int op);
    
    // 处理赋值操作符（复合赋值）
    static Value* executeAssignmentOperation(Value* left, Value* right, int op);
    
    // 处理访问操作符（成员访问、数组访问、函数调用）
    static Value* executeAccessOperation(Value* target, Value* index, int op);
    
    // 处理特殊操作符（如逗号操作符、三元操作符）
    static Value* executeSpecialOperation(Value* left, Value* right, int op);
    
    // 处理访问操作符（成员访问、数组访问）
    static Value* executeAccessOperation(Value* target, Value* index, int op);
    static Value* executeMemberAccess(Value* target, const string& memberName);
    static Value* executeArrayAccess(Value* target, Value* index);
    
private:
    // 尝试类型转换后执行运算
    static Value* tryWithTypeConversion(Value* left, Value* right, const string& methodName);
    
    // 尝试将值转换为指定类型
    static Value* tryConvertValue(Value* value, const string& targetType);
    
    // 尝试通过通用类型进行转换
    static Value* tryConvertThroughCommonType(Value* source, const string& targetType);
    
    // 智能类型转换策略
    static Value* trySmartTypeConversion(Value* left, Value* right, const string& methodName);
    
    // 处理赋值操作符的特殊逻辑
    static Value* handleAssignmentOperator(Value* left, Value* right, const string& methodName);
    
    // 处理访问操作符的特殊逻辑
    static Value* handleAccessOperator(Value* target, Value* index, const string& methodName);
};

#endif // INTERPRETER_CALCULATE_H 