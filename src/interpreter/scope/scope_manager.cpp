#include "interpreter/scope/scope.h"
#include "interpreter/types/types.h"
#include "interpreter/values/value.h"
#include "interpreter/utils/logger.h"
#include "interpreter/core/function_call.h"
// #include "interpreter/values/method_value.h"
// #include "interpreter/values/class_method_value.h"

#include <iostream>

using namespace std;

// ==================== ScopeManager实现 ====================

ScopeManager::ScopeManager() {
    // 创建全局作用域
    globalScope = new Scope();
    currentScope = globalScope;
    scopes.push_back(globalScope);
}

ScopeManager::~ScopeManager() {
    // 清理所有作用域
    for (Scope* scope : scopes) {
        if (scope) {
            scope->cleanup();
            delete scope;
        }
    }
    scopes.clear();
}

void ScopeManager::enterScope() {
    Scope* newScope = new Scope();
    scopes.push_back(newScope);
    currentScope = newScope;
}

void ScopeManager::exitScope() {
    if (scopes.size() > 1) {  // 保留全局作用域
        Scope* scope = scopes.back();
        scopes.pop_back();
        
        if (scope) {
            scope->cleanup();
            delete scope;
        }
        
        currentScope = scopes.back();
    }
}

// ==================== 私有辅助方法实现 ====================

// ==================== 统一查询接口实现 ====================

// 主要查询方法 - 按优先级查找标识符
Value* ScopeManager::lookup(const string& name) {
    LOG_DEBUG("ScopeManager::lookup: looking for '" + name + "'");
    
    // 1. 首先查找变量（最高优先级）
    Value* variable = lookupVariable(name);
    if (variable) {
        LOG_DEBUG("Found variable: " + name);
        return variable;
    }
    
    // 2. 查找函数
    Value* function = lookupFunction(name);
    if (function) {
        LOG_DEBUG("Found function: " + name);
        return function;
    }
    
    // 3. 查找方法
    Value* method = lookupMethod(name);
    if (method) {
        LOG_DEBUG("Found method: " + name);
        return method;
    }
    
    // 4. 查找实例
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        Scope* scope = *it;
        if (scope && scope->objectRegistry) {
            Value* instance = scope->objectRegistry->lookupInstance(name);
            if (instance) {
                LOG_DEBUG("Found instance: " + name);
                return instance;
            }
        }
    }
    
    // 5. 最后查找类型名称
    ObjectType* type = lookupType(name);
    if (type) {
        LOG_DEBUG("Found type: " + name);
        // 返回一个表示类型的特殊值
        return new String(name);
    }
    
    LOG_DEBUG("Nothing found for: " + name);
    return nullptr;
}

// 类型化查询方法 - 明确指定要查找的类型
ObjectType* ScopeManager::lookupType(const string& name) {
    // 1. 首先在作用域中查找用户定义的类型
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        Scope* scope = *it;
        if (scope && scope->typeRegistry && scope->typeRegistry->hasType(name)) {
            ObjectType* type = scope->typeRegistry->getType(name);
            if (type) return type;
        }
    }
    
    // 2. 然后在全局类型系统中查找内置类型
    TypeRegistry* globalRegistry = TypeRegistry::getGlobalInstance();
    ObjectType* globalType = globalRegistry->getType(name);
    if (globalType) return globalType;
    
    return nullptr;
}

Value* ScopeManager::lookupVariable(const string& name) {
    // 在作用域中查找变量
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        Scope* scope = *it;
        if (scope && scope->objectRegistry) {
            Value* variable = scope->objectRegistry->lookupVariable(name);
            if (variable) return variable;
        }
    }
    
    // 如果变量不存在，检查是否有this指针，尝试访问实例成员
    Value* thisPtr = getThisPointer();
    if (thisPtr) {
        ObjectType* objectType = thisPtr->getValueType();
        if (objectType && objectType->hasMethodName(name)) {
            return objectType->accessMember(thisPtr, name);
        }
    }
    
    return nullptr;
}

Value* ScopeManager::lookupCallable(const string& name) {
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        Scope* scope = *it;
        if (scope && scope->objectRegistry) {
            Value* callable = scope->objectRegistry->lookupCallable(name);
            if (callable) {
                return callable;
            }
        }
    }
    return nullptr;
}

Value* ScopeManager::lookupTypeMethod(const string& typeName, const string& methodName) {
    LOG_DEBUG("ScopeManager::lookupTypeMethod: looking for method '" + methodName + "' in type '" + typeName + "'");
    
    // 查找类型
    ObjectType* type = TypeRegistry::getGlobalInstance()->getType(typeName);
    if (!type) {
        LOG_DEBUG("ScopeManager::lookupTypeMethod: type '" + typeName + "' not found");
        return nullptr;
    }
    
    // 检查静态方法
    if (type->hasStaticMethodName(methodName)) {
        LOG_DEBUG("ScopeManager::lookupTypeMethod: found static method '" + methodName + "' in type '" + typeName + "'");
        return new ClassMethodValue(type, methodName);
    }
    
    // 检查实例方法
    if (type->hasMethodName(methodName)) {
        LOG_DEBUG("ScopeManager::lookupTypeMethod: found instance method '" + methodName + "' in type '" + typeName + "'");
        return new MethodValue(type, nullptr, methodName);
    }
    
    LOG_DEBUG("ScopeManager::lookupTypeMethod: method '" + methodName + "' not found in type '" + typeName + "'");
    return nullptr;
}

// 简单聚合查询方法 - 一次性查询所有类型的标识符，返回第一个找到的
Value* ScopeManager::lookupAny(const string& name) {
    cout << "DEBUG: ScopeManager::lookupAny: looking for '" + name + "'" << endl;
    LOG_DEBUG("ScopeManager::lookupAny: looking for '" + name + "'");
    
    // 1. 首先尝试查找变量
    cout << "DEBUG: ScopeManager::lookupAny: trying lookupVariable" << endl;
    Value* value = lookupVariable(name);
    if (value) {
        cout << "DEBUG: ScopeManager::lookupAny: found variable '" + name + "'" << endl;
        LOG_DEBUG("ScopeManager::lookupAny: found variable '" + name + "'");
        return value;
    }
    
    // 2. 然后尝试查找可调用对象（函数等）
    cout << "DEBUG: ScopeManager::lookupAny: trying lookupCallable" << endl;
    value = lookupCallable(name);
    if (value) {
        cout << "DEBUG: ScopeManager::lookupAny: found callable '" + name + "'" << endl;
        LOG_DEBUG("ScopeManager::lookupAny: found callable '" + name + "'");
        return value;
    }
    
    // 3. 然后尝试查找实例（暂时跳过，直接进行类型名称查找）
    cout << "DEBUG: ScopeManager::lookupAny: skipping lookupInstance for now" << endl;
    
    // 4. 最后尝试查找类型名称
    cout << "DEBUG: ScopeManager::lookupAny: trying TypeRegistry::getGlobalInstance()->getType" << endl;
    ObjectType* type = TypeRegistry::getGlobalInstance()->getType(name);
    if (type) {
        cout << "DEBUG: ScopeManager::lookupAny: found type '" + name + "'" << endl;
        LOG_DEBUG("ScopeManager::lookupAny: found type '" + name + "'");
        // 返回一个表示类型的特殊值，这样AccessExpression就能识别它是类型名称
        // 创建一个没有运行时类型的String对象，这样AccessExpression就会将其识别为类型名称
        String* typeNameObj = new String(name);
        typeNameObj->setValueType(nullptr); // 清除运行时类型，这样AccessExpression就会将其识别为类型名称
        return typeNameObj;
    }
    
    cout << "DEBUG: ScopeManager::lookupAny: nothing found for '" + name + "'" << endl;
    LOG_DEBUG("ScopeManager::lookupAny: nothing found for '" + name + "'");
    return nullptr;
}

Value* ScopeManager::lookupFunction(const string& name) {
    LOG_DEBUG("Looking for function: " + name);
    
    // 1. 首先尝试直接查找
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        Scope* scope = *it;
        if (scope && scope->objectRegistry) {
            Value* callable = scope->objectRegistry->lookupCallable(name);
            if (callable && callable->isCallable()) {
                LOG_DEBUG("Found function by direct lookup: " + name);
                return callable;
            }
        }
    }
    
    // 2. 尝试通过函数名查找
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        Scope* scope = *it;
        if (scope && scope->objectRegistry) {
            const auto& callables = scope->objectRegistry->getCallableObjects();
            for (const auto& pair : callables) {
                Value* callable = pair.second;
                if (callable && callable->isCallable()) {
                    if (Function* func = dynamic_cast<Function*>(callable)) {
                        if (func->getName() == name) {
                            LOG_DEBUG("Found function by name lookup: " + name);
                            return callable;
                        }
                    }
                }
            }
        }
    }
    
    // 3. 尝试在类型系统中查找同名类型（构造函数）
    ObjectType* type = TypeRegistry::getGlobalInstance()->getType(name);
    if (type) {
        if (type->hasStaticMethodName(name)) {
            LOG_DEBUG("Found constructor: " + name);
            return new ClassMethodValue(type, name);
        }
        if (type->hasMethodName(name)) {
            LOG_DEBUG("Found instance method: " + name);
            return new MethodValue(type, nullptr, name);
        }
    }
    
    LOG_DEBUG("Function not found: " + name);
    return nullptr;
}

Value* ScopeManager::lookupMethod(const string& name) {
    // 查找当前类上下文中的方法
    ClassType* currentClass = getCurrentClassContext();
    if (currentClass) {
        if (currentClass->hasMethodName(name)) {
            return new MethodValue(currentClass, nullptr, name);
        }
    }
    
    // 查找this指针指向对象的方法
    Value* thisPtr = getThisPointer();
    if (thisPtr) {
        ObjectType* objectType = thisPtr->getValueType();
        if (objectType && objectType->hasMethodName(name)) {
            return new MethodValue(objectType, thisPtr, name);
        }
    }
    
    return nullptr;
}

// 批量查询方法 - 一次性获取所有匹配的标识符
ScopeManager::LookupResult ScopeManager::lookupAll(const string& name) {
    LookupResult result;
    
    // 查找类型
    result.type = lookupType(name);
    
    // 查找变量
    result.variable = lookupVariable(name);
    
    // 查找函数
    result.function = lookupFunction(name);
    
    // 查找方法
    result.method = lookupMethod(name);
    
    // 查找实例
    // 查找实例
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        Scope* scope = *it;
        if (scope && scope->objectRegistry) {
            Value* instance = scope->objectRegistry->lookupInstance(name);
            if (instance) {
                result.instance = instance;
                break;
            }
        }
    }
    
    return result;
}

// ==================== 定义接口实现 ====================

void ScopeManager::defineType(const string& name, ObjectType* type) {
    if (currentScope && currentScope->typeRegistry) {
        currentScope->typeRegistry->registerType(name, type);
    }
}

void ScopeManager::defineVariable(const string& name, Value* value) {
    if (!value) {
        ObjectType* nullType = TypeRegistry::getGlobalInstance()->getType("null");
        defineVariable(name, nullType, value);
        return;
    }
    
    ObjectType* valueType = value->getValueType();
    if (valueType) {
        defineVariable(name, valueType, value);
    } else {
        string typeName = value->getBuiltinTypeName();
        ObjectType* inferredType = TypeRegistry::getGlobalInstance()->getType(typeName);
        if (inferredType) {
            value->setValueType(inferredType);
            defineVariable(name, inferredType, value);
        } else {
            ObjectType* unknownType = TypeRegistry::getGlobalInstance()->getType("unknown");
            defineVariable(name, unknownType, value);
        }
    }
}

void ScopeManager::defineVariable(const string& name, ObjectType* type, Value* value) {
    if (currentScope && currentScope->objectRegistry) {
        currentScope->objectRegistry->defineVariable(name, value);
    }
}

void ScopeManager::defineVariable(const string& name, const string& type, Value* value) {
    ObjectType* typeObj = TypeRegistry::getGlobalInstance()->createTypeFromString(type);
    defineVariable(name, typeObj, value);
}

void ScopeManager::defineFunction(const string& name, Value* function) {
    if (currentScope && currentScope->objectRegistry && function) {
        currentScope->objectRegistry->defineCallable(name, function);
    }
}

void ScopeManager::defineFunction(const string& name, const vector<string>& parameterTypes, Value* function) {
    if (currentScope && currentScope->objectRegistry && function) {
        // 使用参数类型信息创建函数签名
        string key = name + "(";
        for (size_t i = 0; i < parameterTypes.size(); ++i) {
            if (i > 0) key += ",";
            key += parameterTypes[i];
        }
        key += ")";
        currentScope->objectRegistry->defineCallable(key, function);
    }
}

void ScopeManager::defineMethod(const string& name, Value* method) {
    if (currentScope && currentScope->objectRegistry && method) {
        currentScope->objectRegistry->defineCallable(name, method);
    }
}

void ScopeManager::defineInstance(const string& name, Value* instance) {
    if (currentScope && currentScope->objectRegistry) {
        currentScope->objectRegistry->defineInstance(name, instance);
    }
}

// ==================== 更新接口实现 ====================

void ScopeManager::updateVariable(const string& name, Value* value) {
    if (currentScope && currentScope->objectRegistry) {
        if (currentScope->objectRegistry->hasVariable(name)) {
            Value* oldValue = currentScope->objectRegistry->lookupVariable(name);
            if (oldValue) {
                delete oldValue;
            }
        }
        currentScope->objectRegistry->defineVariable(name, value);
    }
}

// ==================== 检查接口实现 ====================

bool ScopeManager::has(const string& name) const {
    return hasVariable(name) || hasFunction(name) || hasType(name);
}

bool ScopeManager::hasVariable(const string& name) const {
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        Scope* scope = *it;
        if (scope && scope->objectRegistry && scope->objectRegistry->hasVariable(name)) {
            return true;
        }
    }
    return false;
}

bool ScopeManager::hasFunction(const string& name) const {
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        Scope* scope = *it;
        if (scope && scope->objectRegistry && scope->objectRegistry->hasCallable(name)) {
            return true;
        }
    }
    return false;
}

bool ScopeManager::hasType(const string& name) const {
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        Scope* scope = *it;
        if (scope && scope->typeRegistry && scope->typeRegistry->hasType(name)) {
            return true;
        }
    }
    
    TypeRegistry* globalRegistry = TypeRegistry::getGlobalInstance();
    return globalRegistry->getType(name) != nullptr;
}

// ==================== 上下文管理实现 ====================

void ScopeManager::setThisPointer(Value* thisPtr) {
    if (currentScope) {
        currentScope->thisPointer = thisPtr;
        LOG_DEBUG("Set this pointer for current scope");
    }
}

Value* ScopeManager::getThisPointer() const {
    if (currentScope) {
        return currentScope->thisPointer;
    }
    return nullptr;
}

void ScopeManager::clearThisPointer() {
    if (currentScope) {
        currentScope->thisPointer = nullptr;
        LOG_DEBUG("Cleared this pointer for current scope");
    }
}

void ScopeManager::setCurrentClassContext(ClassType* classType) {
    if (currentScope) {
        currentScope->currentClassContext = classType;
        LOG_DEBUG("Set current class context for current scope");
    }
}

ClassType* ScopeManager::getCurrentClassContext() const {
    if (currentScope) {
        return currentScope->currentClassContext;
    }
    return nullptr;
}

void ScopeManager::clearCurrentClassContext() {
    if (currentScope) {
        currentScope->currentClassContext = nullptr;
        LOG_DEBUG("Cleared current class context for current scope");
    }
}

// ==================== 作用域信息实现 ====================

Scope* ScopeManager::getCurrentScope() const {
    return currentScope;
}

Scope* ScopeManager::getGlobalScope() const {
    return globalScope;
}

size_t ScopeManager::getScopeDepth() const {
    return scopes.size();
}

// ==================== 调试接口实现 ====================

void ScopeManager::printCurrentScope() const {
    if (currentScope) {
        currentScope->print();
    }
}

void ScopeManager::printAllScopes() const {
    cout << "=== All Scopes ===" << endl;
    size_t i = 0;
    for (auto scope : scopes) {
        cout << "Scope " << i << ":" << endl;
        scope->print();
        cout << endl;
        ++i;
    }
}

// ==================== LookupResult方法实现 ====================

string ScopeManager::LookupResult::toString() const {
    string result = "LookupResult{";
    if (type) result += "type:" + type->getTypeName() + ",";
    if (variable) result += "variable:" + variable->toString() + ",";
    if (function) result += "function:" + function->toString() + ",";
    if (method) result += "method:" + method->toString() + ",";
    if (instance) result += "instance:" + instance->toString() + ",";
    if (result.back() == ',') result.pop_back();
    result += "}";
    return result;
} 