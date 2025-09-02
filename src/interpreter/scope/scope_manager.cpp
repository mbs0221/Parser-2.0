#include "interpreter/scope/scope.h"
#include "interpreter/types/types.h"
#include "interpreter/values/value.h"
#include "interpreter/utils/logger.h"
#include "interpreter/core/function_call.h"

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
            
            delete scope;
        }
        
        currentScope = scopes.back();
    }
}

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
    // 现在使用this变量绑定，不再需要getThisPointer
    
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
    if (type->supportsMethods()) {
        IMethodSupport* methodSupport = dynamic_cast<IMethodSupport*>(type);
        if (methodSupport && methodSupport->hasStaticMethod(methodName)) {
            LOG_DEBUG("ScopeManager::lookupTypeMethod: found static method '" + methodName + "' in type '" + typeName + "'");
            return new StaticMethodReference(type, methodName);
        }
        
        // 检查实例方法
        if (methodSupport && methodSupport->hasUserMethod(methodName)) {
            LOG_DEBUG("ScopeManager::lookupTypeMethod: found instance method '" + methodName + "' in type '" + typeName + "'");
            return new InstanceMethodReference(type, nullptr, methodName);
        }
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
    if (type && type->supportsMethods()) {
        IMethodSupport* methodSupport = dynamic_cast<IMethodSupport*>(type);
        if (methodSupport && methodSupport->hasStaticMethod(name)) {
            LOG_DEBUG("Found constructor: " + name);
            return new StaticMethodReference(type, name);
        }
        if (methodSupport && methodSupport->hasUserMethod(name)) {
            LOG_DEBUG("Found instance method: " + name);
            return new InstanceMethodReference(type, nullptr, name);
        }
    }
    
    LOG_DEBUG("Function not found: " + name);
    return nullptr;
}

Value* ScopeManager::lookupMethod(const string& name) {
    // 现在使用this变量绑定，不再需要getCurrentClassContext和getThis
    // 方法查找通过其他机制处理
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
    // 自动处理类成员和普通变量的注册
    if (isInClassContext()) {
        // 在类定义中，注册为类成员
        registerAsClassMember(name, value);
    } else {
        // 不在类定义中，注册为普通变量
        if (currentScope && currentScope->objectRegistry) {
            currentScope->objectRegistry->defineVariable(name, value);
            LOG_DEBUG("ScopeManager: variable '" + name + "' registered as regular variable");
        }
    }
}

void ScopeManager::defineVariable(const string& name, const string& type, Value* value) {
    ObjectType* typeObj = TypeRegistry::getGlobalInstance()->createTypeFromString(type);
    defineVariable(name, typeObj, value);
}

void ScopeManager::defineFunction(const string& name, Function* function) {
    if (!function) return;
    
    // 自动处理类方法和普通函数的注册
    if (isInClassContext()) {
        // 在类定义中，注册为类方法
        registerAsClassMethod(name, function);
    } else {
        // 不在类定义中，注册为普通函数
        if (currentScope && currentScope->objectRegistry) {
            currentScope->objectRegistry->defineCallable(name, function);
            LOG_DEBUG("ScopeManager: function '" + name + "' registered as regular function");
        }
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

// ==================== 用户函数定义实现 ====================

void ScopeManager::defineUserFunction(const string& name, UserFunction* userFunc) {
    if (currentScope && currentScope->objectRegistry && userFunc) {
        currentScope->objectRegistry->defineCallable(name, userFunc);
    }
}

void ScopeManager::defineUserFunction(const string& name, const vector<string>& parameterTypes, UserFunction* userFunc) {
    if (currentScope && currentScope->objectRegistry && userFunc) {
        currentScope->objectRegistry->defineCallable(name, userFunc);
    }
} 

// ==================== 自动上下文管理辅助方法实现 ====================

bool ScopeManager::isInClassContext() const {
    if (!currentScope) return false;
    
    // 检查当前作用域中是否有__class__变量
    if (String* classNameValue = currentScope->getVariable<String>("__class__")) {
        return true;
    }
    
    // 检查当前作用域中是否有__struct__变量
    if (String* structNameValue = currentScope->getVariable<String>("__struct__")) {
        return true;
    }
    
    return false;
}

VisibilityType ScopeManager::getCurrentVisibility() const {
    if (!currentScope) return VIS_PUBLIC;
    
    // 获取当前可见性
    if (String* visibilityValue = currentScope->getVariable<String>("__visibility__")) {
        string visStr = visibilityValue->getValue();
        if (visStr == "private") {
            return VIS_PRIVATE;
        } else if (visStr == "protected") {
            return VIS_PROTECTED;
        }
    }
    
    return VIS_PUBLIC; // 默认可见性
}

void ScopeManager::registerAsClassMethod(const string& name, Function* function) {
    if (!function) return;
    
    // 获取当前类名或结构体名
    String* classNameValue = currentScope->getVariable<String>("__class__");
    String* structNameValue = currentScope->getVariable<String>("__struct__");
    
    string typeName;
    if (classNameValue) {
        typeName = classNameValue->getValue();
    } else if (structNameValue) {
        typeName = structNameValue->getValue();
    } else {
        LOG_ERROR("ScopeManager: no __class__ or __struct__ variable found when registering class/struct method");
        return;
    }
    
    ObjectType* type = lookupType(typeName);
    if (!type) {
        LOG_ERROR("ScopeManager: failed to find type '" + typeName + "'");
        return;
    }
    
    ClassType* currentClass = dynamic_cast<ClassType*>(type);
    if (!currentClass) {
        LOG_ERROR("ScopeManager: type '" + typeName + "' is not a ClassType or StructType");
        return;
    }
    
    // 获取当前可见性
    VisibilityType visibility = getCurrentVisibility();
    
    // 添加到类或结构体中
    currentClass->addUserMethod(function, visibility);
    
    string typeType = classNameValue ? "class" : "struct";
    LOG_DEBUG("ScopeManager: function '" + name + "' registered as " + typeType + " method in " + typeType + " '" + typeName + 
             "' with visibility '" + (visibility == VIS_PUBLIC ? "public" : visibility == VIS_PRIVATE ? "private" : "protected") + "'");
}

void ScopeManager::registerAsClassMember(const string& name, Value* value) {
    if (!value) return;
    
    // 获取当前类名或结构体名
    String* classNameValue = currentScope->getVariable<String>("__class__");
    String* structNameValue = currentScope->getVariable<String>("__struct__");
    
    string typeName;
    if (classNameValue) {
        typeName = classNameValue->getValue();
    } else if (structNameValue) {
        typeName = structNameValue->getValue();
    } else {
        LOG_ERROR("ScopeManager: no __class__ or __struct__ variable found when registering class/struct member");
        return;
    }
    
    ObjectType* type = lookupType(typeName);
    if (!type) {
        LOG_ERROR("ScopeManager: failed to find type '" + typeName + "'");
        return;
    }
    
    ClassType* currentClass = dynamic_cast<ClassType*>(type);
    if (!currentClass) {
        LOG_ERROR("ScopeManager: type '" + typeName + "' is not a ClassType or StructType");
        return;
    }
    
    // 获取当前可见性
    VisibilityType visibility = getCurrentVisibility();
    
    // 添加到类或结构体中
    currentClass->addMember(name, value->getValueType(), visibility);
    
    // 如果有初始值，设置初始值
    currentClass->setMemberInitialValue(name, value);
    
    string typeType = classNameValue ? "class" : "struct";
    LOG_DEBUG("ScopeManager: variable '" + name + "' registered as " + typeType + " member in " + typeType + " '" + typeName + 
             "' with visibility '" + (visibility == VIS_PUBLIC ? "public" : visibility == VIS_PRIVATE ? "private" : "protected") + "'");
}

void ScopeManager::registerAsStructMember(const string& name, Value* value) {
    if (!value) return;
    
    // 获取当前结构体名
    String* structNameValue = currentScope->getVariable<String>("__struct__");
    if (!structNameValue) {
        LOG_ERROR("ScopeManager: no __struct__ variable found when registering struct member");
        return;
    }
    
    string structName = structNameValue->getValue();
    ObjectType* type = lookupType(structName);
    if (!type) {
        LOG_ERROR("ScopeManager: failed to find struct type '" + structName + "'");
        return;
    }
    
    StructType* currentStruct = dynamic_cast<StructType*>(type);
    if (!currentStruct) {
        LOG_ERROR("ScopeManager: type '" + structName + "' is not a StructType");
        return;
    }
    
    // 结构体成员默认都是 public，不需要考虑 visibility
    // 添加到结构体中（StructType会自动强制为公有）
    currentStruct->addMember(name, value->getValueType(), VIS_PUBLIC);
    
    // 如果有初始值，设置初始值
    currentStruct->setMemberInitialValue(name, value);
    
    LOG_DEBUG("ScopeManager: variable '" + name + "' registered as struct member in struct '" + structName + "' (public)");
}