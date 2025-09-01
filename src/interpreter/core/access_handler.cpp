#include "interpreter/core/access_handler.h"
#include "interpreter/scope/scope_manager.h"
#include "interpreter/values/value.h"
#include "interpreter/values/string.h"
#include "interpreter/values/method_value.h"
#include "interpreter/values/class_method_value.h"
#include "interpreter/types/types.h"
#include "interpreter/utils/logger.h"
#include "parser/expression.h"
#include <iostream>

using namespace std;

// ==================== 统一访问处理器实现 ====================

// 处理成员访问表达式
Value* AccessHandler::handleAccess(AccessExpression* access, ScopeManager& scopeManager) {
    if (!access || !access->target || !access->key) {
        LOG_ERROR("Invalid access expression");
        return nullptr;
    }
    
    // 获取目标对象和成员名称
    auto [target, memberName] = extractTargetAndMember(access, scopeManager);
    if (!target || memberName.empty()) {
        return nullptr;
    }
    
    LOG_DEBUG("AccessHandler: accessing member '" + memberName + "' from target: " + target->toString());
    
    // 使用统一接口处理访问
    return handleUnifiedAccess(target, memberName, scopeManager);
}

// 提取目标对象和成员名称
pair<Value*, string> AccessHandler::extractTargetAndMember(AccessExpression* access, ScopeManager& scopeManager) {
    // 求值目标
    Value* target = evaluateTarget(access->target, scopeManager);
    if (!target) {
        LOG_ERROR("Failed to evaluate access target");
        return {nullptr, ""};
    }
    
    // 求值键并转换为成员名称
    string memberName = extractMemberName(access->key, scopeManager);
    if (memberName.empty()) {
        LOG_ERROR("Failed to extract member name from access key");
        return {nullptr, ""};
    }
    
    return {target, memberName};
}

// 求值目标表达式
Value* AccessHandler::evaluateTarget(Expression* targetExpr, ScopeManager& scopeManager) {
    // 这里需要调用解释器的visit方法
    // 为了演示，我们假设targetExpr已经被求值
    // 实际实现中应该通过解释器上下文来访问
    
    // 检查是否是常量表达式
    if (ConstantExpression<string>* stringExpr = dynamic_cast<ConstantExpression<string>*>(targetExpr)) {
        // 如果是字符串常量，可能是类型名称
        return new String(stringExpr->getValue());
    }
    
    // 其他类型的表达式需要实际求值
    // 这里返回nullptr表示需要实际实现
    return nullptr;
}

// 提取成员名称
string AccessHandler::extractMemberName(Expression* keyExpr, ScopeManager& scopeManager) {
    // 这里需要调用解释器的visit方法
    // 为了演示，我们假设keyExpr已经被求值
    
    // 检查是否是字符串常量表达式
    if (ConstantExpression<string>* stringExpr = dynamic_cast<ConstantExpression<string>*>(keyExpr)) {
        return stringExpr->getValue();
    }
    
    // 其他类型的表达式需要实际求值
    // 这里返回空字符串表示需要实际实现
    return "";
}

// 使用统一接口处理访问
Value* AccessHandler::handleUnifiedAccess(Value* target, const string& memberName, ScopeManager& scopeManager) {
    // 情况1: 目标没有运行时类型，尝试作为类型名称处理
    if (!target->getValueType()) {
        return handleTypeNameAccess(target, memberName, scopeManager);
    }
    
    // 情况2: 目标有运行时类型，处理实例成员访问
    return handleInstanceAccess(target, memberName, scopeManager);
}

// 处理类型名称访问（静态方法/静态成员）
Value* AccessHandler::handleTypeNameAccess(Value* target, const string& memberName, ScopeManager& scopeManager) {
    if (String* typeNameStr = dynamic_cast<String*>(target)) {
        string typeName = typeNameStr->getValue();
        LOG_DEBUG("AccessHandler: target appears to be a type name: '" + typeName + "'");
        
        // 使用新的统一接口查找类型方法
        Value* typeMethod = scopeManager.lookupTypeMethod(typeName, memberName);
        if (typeMethod) {
            LOG_DEBUG("AccessHandler: found type method '" + memberName + "' in type '" + typeName + "'");
            return typeMethod;
        }
        
        // 如果找不到方法，尝试查找类型本身
        ObjectType* type = scopeManager.lookupType(typeName);
        if (type) {
            LOG_DEBUG("AccessHandler: found type '" + typeName + "', returning type reference");
            return new String(typeName); // 返回类型引用
        }
        
        LOG_ERROR("Type '" + typeName + "' not found");
        return nullptr;
    }
    
    LOG_ERROR("Cannot access object without runtime type");
    return nullptr;
}

// 处理实例成员访问
Value* AccessHandler::handleInstanceAccess(Value* target, const string& memberName, ScopeManager& scopeManager) {
    ObjectType* targetType = target->getValueType();
    LOG_DEBUG("AccessHandler: target has runtime type: " + targetType->getTypeName());
    
    // 使用新的统一接口查找方法
    Value* method = scopeManager.lookupMethod(memberName);
    if (method) {
        LOG_DEBUG("AccessHandler: found method '" + memberName + "'");
        return method;
    }
    
    // 检查成员变量
    if (targetType->hasMember(memberName)) {
        LOG_DEBUG("AccessHandler: found member variable '" + memberName + "'");
        Value* memberValue = targetType->accessMember(target, memberName);
        if (memberValue) {
            return memberValue;
        }
        LOG_ERROR("Failed to access member variable '" + memberName + "'");
        return nullptr;
    }
    
    // 如果是类类型，检查静态成员
    if (ClassType* classType = dynamic_cast<ClassType*>(targetType)) {
        return handleClassStaticAccess(classType, memberName, scopeManager);
    }
    
    LOG_ERROR("Member '" + memberName + "' not found in type '" + targetType->getTypeName() + "'");
    return nullptr;
}

// 处理类静态成员访问
Value* AccessHandler::handleClassStaticAccess(ClassType* classType, const string& memberName, ScopeManager& scopeManager) {
    // 检查静态方法
    if (classType->hasStaticMethodName(memberName)) {
        LOG_DEBUG("AccessHandler: found static method '" + memberName + "' in class type");
        return new ClassMethodValue(classType, memberName);
    }
    
    // 检查静态变量（如果实现了的话）
    // 这里可以扩展支持静态变量访问
    
    LOG_DEBUG("AccessHandler: no static member '" + memberName + "' found in class type");
    return nullptr;
}

// ==================== 高级访问功能 ====================

// 模糊成员查找
Value* AccessHandler::fuzzyMemberLookup(Value* target, const string& memberName, ScopeManager& scopeManager) {
    // 使用新的统一接口进行模糊查找
    auto result = scopeManager.lookupAll(memberName);
    
    if (result.hasAny()) {
        // 根据目标类型和上下文选择最佳匹配
        return selectBestMatch(result, target, scopeManager);
    }
    
    return nullptr;
}

// 选择最佳匹配
Value* AccessHandler::selectBestMatch(const ScopeManager::LookupResult& result, Value* target, ScopeManager& scopeManager) {
    // 优先级：方法 > 变量 > 函数 > 类型
    if (result.method) return result.method;
    if (result.variable) return result.variable;
    if (result.function) return result.function;
    if (result.type) return new String(result.type->getTypeName());
    if (result.instance) return result.instance;
    
    return nullptr;
}

// 上下文感知的成员访问
Value* AccessHandler::contextAwareAccess(Value* target, const string& memberName, 
                                        ScopeManager& scopeManager, const AccessContext& context) {
    // 根据访问上下文调整查找策略
    switch (context.accessType) {
        case AccessType::METHOD_CALL:
            return handleMethodAccess(target, memberName, scopeManager);
        case AccessType::PROPERTY_ACCESS:
            return handlePropertyAccess(target, memberName, scopeManager);
        case AccessType::STATIC_ACCESS:
            return handleStaticAccess(target, memberName, scopeManager);
        case AccessType::ARRAY_ACCESS:
            return handleArrayAccess(target, memberName, scopeManager);
        default:
            return handleUnifiedAccess(target, memberName, scopeManager);
    }
}

// 处理方法访问
Value* AccessHandler::handleMethodAccess(Value* target, const string& methodName, ScopeManager& scopeManager) {
    // 优先查找实例方法
    Value* method = scopeManager.lookupMethod(methodName);
    if (method) {
        return method;
    }
    
    // 然后查找函数
    Value* function = scopeManager.lookupFunction(methodName);
    if (function) {
        return function;
    }
    
    return nullptr;
}

// 处理属性访问
Value* AccessHandler::handlePropertyAccess(Value* target, const string& propertyName, ScopeManager& scopeManager) {
    // 优先查找变量
    Value* variable = scopeManager.lookupVariable(propertyName);
    if (variable) {
        return variable;
    }
    
    // 然后查找实例
    // 这里需要实现实例查找逻辑
    
    return nullptr;
}

// 处理静态访问
Value* AccessHandler::handleStaticAccess(Value* target, const string& staticMemberName, ScopeManager& scopeManager) {
    // 处理静态成员访问
    if (String* typeNameStr = dynamic_cast<String*>(target)) {
        string typeName = typeNameStr->getValue();
        return scopeManager.lookupTypeMethod(typeName, staticMemberName);
    }
    
    return nullptr;
}

// 处理数组访问
Value* AccessHandler::handleArrayAccess(Value* target, const string& indexName, ScopeManager& scopeManager) {
    // 处理数组索引访问
    // 这里需要实现数组访问逻辑
    
    return nullptr;
}
