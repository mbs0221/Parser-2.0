#include "parser/scope.h"
#include "parser/function.h"
#include "parser/inter.h"
#include "lexer/value.h"
#include <iostream>

using namespace std;

// ==================== Scope实现 ====================

void Scope::cleanup() {
    // 清理所有标识符
    for (auto& pair : identifiers) {
        if (pair.second) {
            delete pair.second;
        }
    }
    identifiers.clear();
}

void Scope::print() const {
    cout << "Identifiers:" << endl;
    for (const auto& pair : identifiers) {
        cout << "  " << pair.first << " (" << pair.second->getIdentifierType() << ")" << endl;
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

void ScopeManager::defineIdentifier(const string& name, Identifier* identifier) {
    if (currentScope) {
        // 如果标识符已存在，先删除旧值
        auto it = currentScope->identifiers.find(name);
        if (it != currentScope->identifiers.end()) {
            delete it->second;
        }
        
        currentScope->identifiers[name] = identifier;
    }
}

Identifier* ScopeManager::lookupIdentifier(const string& name) {
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
    Variable* varDef = new Variable(name, type, value);
    defineIdentifier(name, varDef);
}

void ScopeManager::defineVariable(const string& name, Value* value) {
    Variable* varDef = new Variable(name, "auto", value);
    defineIdentifier(name, varDef);
}

Variable* ScopeManager::lookupVariable(const string& name) {
    Identifier* identifier = lookupIdentifier(name);
    if (identifier && identifier->getIdentifierType() == "Variable") {
        return dynamic_cast<Variable*>(identifier);
    }
    return nullptr;
}

void ScopeManager::updateVariable(const string& name, Value* value) {
    Variable* variable = lookupVariable(name);
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
            return it->second->getIdentifierType() == "Variable";
        }
    }
    return false;
}

bool ScopeManager::isFunctionDefinedInCurrentScope(const string& name) const {
    if (currentScope) {
        auto it = currentScope->identifiers.find(name);
        if (it != currentScope->identifiers.end()) {
            string type = it->second->getIdentifierType();
            return type == "UserFunction" || type == "BuiltinFunction";
        }
    }
    return false;
}



