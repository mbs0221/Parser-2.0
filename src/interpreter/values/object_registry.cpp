#include "interpreter/values/value.h"
#include <iostream>

using namespace std;

// ==================== ObjectRegistry实现 ====================
ObjectRegistry::ObjectRegistry() {
}

ObjectRegistry::~ObjectRegistry() {
    cleanup();
}

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

void ObjectRegistry::defineCallable(const string& name, Value* callable) {
    callableObjects[name] = callable;
}

Value* ObjectRegistry::lookupCallable(const string& name) const {
    auto it = callableObjects.find(name);
    return (it != callableObjects.end()) ? it->second : nullptr;
}

bool ObjectRegistry::hasCallable(const string& name) const {
    return callableObjects.find(name) != callableObjects.end();
}

void ObjectRegistry::defineInstance(const string& name, Value* instance) {
    instanceObjects[name] = instance;
}

Value* ObjectRegistry::lookupInstance(const string& name) const {
    auto it = instanceObjects.find(name);
    return (it != variableObjects.end()) ? it->second : nullptr;
}

bool ObjectRegistry::hasInstance(const string& name) const {
    return instanceObjects.find(name) != instanceObjects.end();
}

tuple<Value*, Value*, Value*> ObjectRegistry::lookupObject(const string& name) const {
    Value* var = lookupVariable(name);
    Value* callable = lookupCallable(name);
    Value* instance = lookupInstance(name);
    return make_tuple(var, callable, instance);
}

bool ObjectRegistry::hasObject(const string& name) const {
    return hasVariable(name) || hasCallable(name) || hasInstance(name);
}

void ObjectRegistry::cleanup() {
    // 注意：这里不删除Value对象，因为它们是共享的
    variableObjects.clear();
    callableObjects.clear();
    instanceObjects.clear();
}

void ObjectRegistry::print() const {
    cout << "=== ObjectRegistry Content ===" << endl;
    cout << "Variables: " << variableObjects.size() << endl;
    for (const auto& pair : variableObjects) {
        cout << "  " << pair.first << " -> " << (pair.second ? pair.second->toString() : "null") << endl;
    }
    cout << "Callables: " << callableObjects.size() << endl;
    for (const auto& pair : callableObjects) {
        cout << "  " << pair.first << " -> " << (pair.second ? pair.second->toString() : "null") << endl;
    }
    cout << "Instances: " << instanceObjects.size() << endl;
    for (const auto& pair : instanceObjects) {
        cout << "  " << pair.first << " -> " << (pair.second ? pair.second->toString() : "null") << endl;
    }
}
