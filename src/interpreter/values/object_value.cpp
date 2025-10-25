#include "interpreter/values/value.h"
#include "interpreter/types/types.h"
#include "common/logger.h"

ObjectValue::ObjectValue(const std::string& name, ObjectType* vt) 
    : Value(vt), className(name) {
}

ObjectValue::ObjectValue(const ObjectValue& other) 
    : Value(other), className(other.className) {
    // 深拷贝属性
    for (const auto& pair : other.properties) {
        properties[pair.first] = pair.second ? pair.second->clone() : nullptr;
    }
    // 方法不需要深拷贝，只是引用
    methods = other.methods;
}

ObjectValue::~ObjectValue() {
    // 清理属性
    for (auto& pair : properties) {
        if (pair.second) {
            delete pair.second;
        }
    }
    properties.clear();
    methods.clear();
}

ObjectValue ObjectValue::operator=(const ObjectValue& other) {
    if (this != &other) {
        // 清理现有属性
        for (auto& pair : properties) {
            if (pair.second) {
                delete pair.second;
            }
        }
        properties.clear();
        methods.clear();
        
        // 复制新属性
        className = other.className;
        for (const auto& pair : other.properties) {
            properties[pair.first] = pair.second ? pair.second->clone() : nullptr;
        }
        methods = other.methods;
    }
    return *this;
}

void ObjectValue::setProperty(const std::string& name, Value* value) {
    // 如果属性已存在，先删除旧值
    if (properties.find(name) != properties.end() && properties[name]) {
        delete properties[name];
    }
    properties[name] = value;
}

Value* ObjectValue::getProperty(const std::string& name) const {
    auto it = properties.find(name);
    if (it != properties.end()) {
        return it->second;
    }
    return nullptr;
}

void ObjectValue::removeProperty(const std::string& name) {
    auto it = properties.find(name);
    if (it != properties.end()) {
        if (it->second) {
            delete it->second;
        }
        properties.erase(it);
    }
}

bool ObjectValue::hasProperty(const std::string& name) const {
    return properties.find(name) != properties.end();
}

void ObjectValue::setMethod(const std::string& name, Function* method) {
    methods[name] = method;
}

Function* ObjectValue::getMethod(const std::string& name) const {
    auto it = methods.find(name);
    if (it != methods.end()) {
        return it->second;
    }
    return nullptr;
}

void ObjectValue::removeMethod(const std::string& name) {
    methods.erase(name);
}

bool ObjectValue::hasMethod(const std::string& name) const {
    return methods.find(name) != methods.end();
}

std::string ObjectValue::toString() const {
    std::string result = className + " {";
    bool first = true;
    for (const auto& pair : properties) {
        if (!first) {
            result += ", ";
        }
        result += pair.first + ": ";
        if (pair.second) {
            result += pair.second->toString();
        } else {
            result += "null";
        }
        first = false;
    }
    result += "}";
    return result;
}

bool ObjectValue::toBool() const {
    return true; // 对象总是为真
}

Value* ObjectValue::clone() const {
    return new ObjectValue(*this);
}

std::string ObjectValue::getBuiltinTypeName() const {
    return className;
}

std::string ObjectValue::getClassName() const {
    return className;
}

bool ObjectValue::isInstance() const {
    return true;
}

const std::map<std::string, Value*>& ObjectValue::getProperties() const {
    return properties;
}

const std::map<std::string, Function*>& ObjectValue::getMethods() const {
    return methods;
}
