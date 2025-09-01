#ifndef ACCESS_HANDLER_H
#define ACCESS_HANDLER_H

#include <string>
#include <utility>
#include "interpreter/scope/scope.h"

using namespace std;

// 前向声明
class AccessExpression;
class Value;
class ScopeManager;
class ObjectType;
class ClassType;

// 访问类型枚举
enum class AccessType {
    METHOD_CALL,      // 方法调用
    PROPERTY_ACCESS,  // 属性访问
    STATIC_ACCESS,    // 静态访问
    ARRAY_ACCESS,     // 数组访问
    GENERAL_ACCESS    // 一般访问
};

// 访问上下文
struct AccessContext {
    AccessType accessType;
    std::string contextInfo;
    
    AccessContext(AccessType type = AccessType::GENERAL_ACCESS, const std::string& info = "")
        : accessType(type), contextInfo(info) {}
};

// 统一访问处理器
class AccessHandler {
public:
    // ==================== 核心访问处理 ====================
    
    // 处理成员访问表达式
    static Value* handleAccess(AccessExpression* access, ScopeManager& scopeManager);
    
    // 提取目标对象和成员名称
    static std::pair<Value*, std::string> extractTargetAndMember(AccessExpression* access, ScopeManager& scopeManager);
    
    // 使用统一接口处理访问
    static Value* handleUnifiedAccess(Value* target, const std::string& memberName, ScopeManager& scopeManager);
    
    // ==================== 类型化访问处理 ====================
    
    // 处理类型名称访问（静态方法/静态成员）
    static Value* handleTypeNameAccess(Value* target, const std::string& memberName, ScopeManager& scopeManager);
    
    // 处理实例成员访问
    static Value* handleInstanceAccess(Value* target, const std::string& memberName, ScopeManager& scopeManager);
    
    // 处理类静态成员访问
    static Value* handleClassStaticAccess(ClassType* classType, const std::string& memberName, ScopeManager& scopeManager);
    
    // ==================== 高级访问功能 ====================
    
    // 模糊成员查找
    static Value* fuzzyMemberLookup(Value* target, const std::string& memberName, ScopeManager& scopeManager);
    
    // 选择最佳匹配
    static Value* selectBestMatch(const ScopeManager::LookupResult& result, Value* target, ScopeManager& scopeManager);
    
    // 上下文感知的成员访问
    static Value* contextAwareAccess(Value* target, const std::string& memberName, 
                                    ScopeManager& scopeManager, const AccessContext& context);
    
    // ==================== 专用访问处理 ====================
    
    // 处理方法访问
    static Value* handleMethodAccess(Value* target, const std::string& methodName, ScopeManager& scopeManager);
    
    // 处理属性访问
    static Value* handlePropertyAccess(Value* target, const std::string& propertyName, ScopeManager& scopeManager);
    
    // 处理静态访问
    static Value* handleStaticAccess(Value* target, const std::string& staticMemberName, ScopeManager& scopeManager);
    
    // 处理数组访问
    static Value* handleArrayAccess(Value* target, const std::string& indexName, ScopeManager& scopeManager);

private:
    // 私有辅助方法
    
    // 求值目标表达式
    static Value* evaluateTarget(Expression* targetExpr, ScopeManager& scopeManager);
    
    // 提取成员名称
    static std::string extractMemberName(Expression* keyExpr, ScopeManager& scopeManager);
};

#endif // ACCESS_HANDLER_H
