#include "interpreter/scope.h"
#include "interpreter/value.h"
#include "interpreter/builtin_type.h"
#include "interpreter/type_registry.h"
#include <iostream>

using namespace std;

// 辅助函数：从字符串创建ObjectType*对象
ObjectType* createTypeFromString(const string& typeName) {
    TypeRegistry* registry = TypeRegistry::getInstance();
    
    if (typeName == "int" || typeName == "integer") {
        return registry->getType("Int");
    } else if (typeName == "double" || typeName == "float") {
        return registry->getType("Double");
    } else if (typeName == "char") {
        return registry->getType("Char");
    } else if (typeName == "bool" || typeName == "boolean") {
        return registry->getType("Bool");
    } else if (typeName == "string") {
        return registry->getType("String");
    } else if (typeName == "auto") {
        // 对于auto类型，暂时返回Int作为默认值
        return registry->getType("Int");
    } else {
        // 未知类型，返回Int作为默认值
        return registry->getType("Int");
    }
}

// ==================== Scope实现 ====================

void Scope::cleanup() {
    // 清理所有运行时标识符
    for (auto& pair : identifiers) {
        if (pair.second) {
            delete pair.second;
            pair.second = nullptr;
        }
    }
    identifiers.clear();
}

void Scope::print() const {
    cout << "Runtime Identifiers:" << endl;
    for (const auto& pair : identifiers) {
        if (pair.second) {
            cout << "  " << pair.first << " (" << pair.second->getIdentifierType() << ")" << endl;
        } else {
            cout << "  " << pair.first << " (null)" << endl;
        }
    }
}

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

void ScopeManager::defineIdentifier(const string& name, RuntimeIdentifier* identifier) {
    if (currentScope) {
        // 如果标识符已存在，先删除旧值
        auto it = currentScope->identifiers.find(name);
        if (it != currentScope->identifiers.end()) {
            delete it->second;
        }
        
        currentScope->identifiers[name] = identifier;
    }
}

RuntimeIdentifier* ScopeManager::lookupIdentifier(const string& name) {
    // 从当前作用域开始，向上查找
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        Scope* scope = *it;
        auto idIt = scope->identifiers.find(name);
        if (idIt != scope->identifiers.end()) {
            return idIt->second;
        }
    }
    return nullptr;
}

bool ScopeManager::isIdentifier(const string& name) const {
    // 从当前作用域开始，向上查找
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        Scope* scope = *it;
        if (scope->identifiers.find(name) != scope->identifiers.end()) {
            return true;
        }
    }
    return false;
}

void ScopeManager::defineVariable(const string& name, const string& type, Value* value) {
    ObjectType* typeObj = createTypeFromString(type);
    RuntimeVariable* varDef = new RuntimeVariable(name, value, typeObj);
    defineIdentifier(name, varDef);
}

void ScopeManager::defineVariable(const string& name, Value* value) {
    ObjectType* typeObj = createTypeFromString("auto");
    RuntimeVariable* varDef = new RuntimeVariable(name, value, typeObj);
    defineIdentifier(name, varDef);
}

void ScopeManager::defineVariable(const string& name, ObjectType* type, Value* value) {
    RuntimeVariable* varDef = new RuntimeVariable(name, value, type);
    defineIdentifier(name, varDef);
}

RuntimeVariable* ScopeManager::lookupVariable(const string& name) {
    RuntimeIdentifier* identifier = lookupIdentifier(name);
    if (identifier && identifier->getIdentifierType() == "RuntimeVariable") {
        return dynamic_cast<RuntimeVariable*>(identifier);
    }
    return nullptr;
}

void ScopeManager::updateVariable(const string& name, Value* value) {
    RuntimeVariable* variable = lookupVariable(name);
    if (variable) {
        variable->setValue(value);
    } else {
        // 如果变量不存在，创建一个新的
        defineVariable(name, value);
    }
}

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

bool ScopeManager::isVariableDefinedInCurrentScope(const string& name) const {
    if (currentScope) {
        auto it = currentScope->identifiers.find(name);
        if (it != currentScope->identifiers.end()) {
            return it->second->getIdentifierType() == "RuntimeVariable";
        }
    }
    return false;
}

bool ScopeManager::isFunctionDefinedInCurrentScope(const string& name) const {
    if (currentScope) {
        auto it = currentScope->identifiers.find(name);
        if (it != currentScope->identifiers.end()) {
            string type = it->second->getIdentifierType();
            return type == "RuntimeFunction";
        }
    }
    return false;
}

// 函数管理方法
void ScopeManager::defineFunction(const string& name, BuiltinFunction* function) {
    BuiltinFunctionWrapper* funcDef = new BuiltinFunctionWrapper(name, function);
    defineIdentifier(name, funcDef);
}

void ScopeManager::defineUserFunction(const string& name, FunctionDefinition* funcDef, int paramCount, 
                                     ScopeManagerPtr scopeMgr, ExecuteFunctionPtr execFunc) {
    // 创建用户函数包装器
    UserFunctionWrapper* wrapper = new UserFunctionWrapper(name, funcDef, paramCount, scopeMgr, execFunc);
    defineIdentifier(name, wrapper);
}

RuntimeFunction* ScopeManager::lookupFunction(const string& name) {
    RuntimeIdentifier* identifier = lookupIdentifier(name);
    if (identifier && identifier->getIdentifierType() == "RuntimeFunction") {
        return dynamic_cast<RuntimeFunction*>(identifier);
    }
    return nullptr;
}

// 类型管理方法已移除 - 类型管理由TypeRegistry统一负责




