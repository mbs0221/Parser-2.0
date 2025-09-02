#include "interpreter/scope/object_registry.h"
#include "interpreter/values/callable.h"
#include "interpreter/types/builtin_type.h"
#include <iostream>

using namespace std;

ObjectRegistry::ObjectRegistry() {
    // 构造函数
}

ObjectRegistry::~ObjectRegistry() {
    cleanup();
}

// ==================== 变量对象管理 ====================

void ObjectRegistry::defineVariable(const string& name, Value* value) {
    variableObjects[name] = value;
}

Value* ObjectRegistry::lookupVariable(const string& name) const {
    auto it = variableObjects.find(name);
    return (it != variableObjects.end()) ? it->second : nullptr;
}

bool ObjectRegistry::hasVariable(const string& name) const {
    return variableObjects.find(name) != variableObjects.end();
}

// ==================== 可调用对象管理 ====================

void ObjectRegistry::defineCallable(const string& name, Value* callable) {
    // 检查是否为可调用对象
    if (callable && callable->isCallable()) {
        callableObjects[name] = callable;
    }
}

Value* ObjectRegistry::lookupCallable(const string& name) const {
    auto it = callableObjects.find(name);
    return (it != callableObjects.end()) ? it->second : nullptr;
}

bool ObjectRegistry::hasCallable(const string& name) const {
    return callableObjects.find(name) != callableObjects.end();
}

// ==================== 实例对象管理 ====================

void ObjectRegistry::defineInstance(const string& name, Value* instance) {
    instanceObjects[name] = instance;
}

Value* ObjectRegistry::lookupInstance(const string& name) const {
    auto it = instanceObjects.find(name);
    return (it != instanceObjects.end()) ? it->second : nullptr;
}

bool ObjectRegistry::hasInstance(const string& name) const {
    return instanceObjects.find(name) != instanceObjects.end();
}

// ==================== 统一对象查找 ====================

tuple<Value*, Value*, Value*> ObjectRegistry::lookupObject(const string& name) const {
    Value* variable = lookupVariable(name);
    Value* callable = lookupCallable(name);
    Value* instance = lookupInstance(name);
    
    return make_tuple(variable, callable, instance);
}

bool ObjectRegistry::hasObject(const string& name) const {
    return hasVariable(name) || hasCallable(name) || hasInstance(name);
}

// ==================== 资源管理 ====================

void ObjectRegistry::cleanup() {
    // 清理变量对象
    for (auto& pair : variableObjects) {
        if (pair.second) {
            delete pair.second;
            pair.second = nullptr;
        }
    }
    variableObjects.clear();
    
    // 清理可调用对象（不需要删除，因为它们是Value的一部分）
    callableObjects.clear();
    
    // 清理实例对象
    for (auto& pair : instanceObjects) {
        if (pair.second) {
            delete pair.second;
            pair.second = nullptr;
        }
    }
    instanceObjects.clear();
}

// ==================== 有序参数管理实现 ====================

// 按顺序获取所有变量（按定义顺序）
std::vector<Value*> ObjectRegistry::getAllVariablesInOrder() const {
    std::vector<Value*> orderedVars;
    // std::map保证插入顺序，所以直接遍历即可
    for (const auto& pair : variableObjects) {
        orderedVars.push_back(pair.second);
    }
    return orderedVars;
}

// 获取变量数量
size_t ObjectRegistry::getVariableCount() const {
    return variableObjects.size();
}

// 获取所有变量名称（按定义顺序）
std::vector<std::string> ObjectRegistry::getVariableNames() const {
    std::vector<std::string> names;
    // std::map保证插入顺序，所以直接遍历即可
    for (const auto& pair : variableObjects) {
        names.push_back(pair.first);
    }
    return names;
}

// ==================== 打印对象注册表内容 ====================

void ObjectRegistry::print() const {
    cout << "  Variables (" << variableObjects.size() << "):" << endl;
    for (const auto& pair : variableObjects) {
        if (pair.second) {
            cout << "    " << pair.first << " -> " << pair.second->getTypeName() << endl;
        }
    }
    
    cout << "  Callables (" << callableObjects.size() << "):" << endl;
    for (const auto& pair : callableObjects) {
        if (pair.second) {
            cout << "    " << pair.first << " -> " << pair.second->getTypeName() << endl;
        }
    }
    
    cout << "  Instances (" << instanceObjects.size() << "):" << endl;
    for (const auto& pair : instanceObjects) {
        if (pair.second) {
            cout << "    " << pair.first << " -> " << pair.second->getTypeName() << endl;
        }
    }
}
size_t ObjectRegistry::getCallableCount() const {
    return callableObjects.size();
}

size_t ObjectRegistry::getInstanceCount() const {
    return instanceObjects.size();
}

const map<string, Value*>& ObjectRegistry::getCallableObjects() const {
    return callableObjects;
}