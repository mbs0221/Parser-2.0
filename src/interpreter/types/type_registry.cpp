#include "interpreter/types/types.h"
#include "interpreter/values/value.h"
#include <iostream>

using namespace std;

// ==================== TypeRegistry实现 ====================

// 根据ObjectType指针获取类型名称
string TypeRegistry::getTypeName(ObjectType* type) const {
    if (!type) return "unknown";
    
    // 遍历所有类型，找到匹配的ObjectType指针
    for (const auto& pair : types) {
        if (pair.second == type) {
            return pair.first;
        }
    }
    
    // 如果没有找到，尝试调用ObjectType的getTypeName方法
    // 这里需要包含builtin_type.h，但为了避免循环依赖，我们暂时返回"unknown"
    return "unknown";
}

// 创建用户定义类型的辅助方法
Interface* TypeRegistry::createInterface(const string& name) {
    Interface* interface = new Interface(name);
    registerType(name, interface);
    return interface;
}

StructType* TypeRegistry::createStructType(const string& name) {
    StructType* structType = new StructType(name);
    registerType(name, structType);
    return structType;
}

ClassType* TypeRegistry::createClassType(const string& name) {
    ClassType* classType = new ClassType(name);
    registerType(name, classType);
    return classType;
}

ClassType* TypeRegistry::createClassType(const string& name, const string& parentTypeName) {
    ClassType* parentType = dynamic_cast<ClassType*>(getType(parentTypeName));
    ClassType* classType = new ClassType(name);
    if (parentType) {
        classType->setParentType(parentType);
    }
    registerType(name, classType);
    return classType;
}

ClassType* TypeRegistry::createClassType(const string& name, const vector<string>& interfaceNames) {
    ClassType* classType = new ClassType(name);
    
    // 添加接口
    for (const string& interfaceName : interfaceNames) {
        Interface* interface = dynamic_cast<Interface*>(getType(interfaceName));
        if (interface) {
            classType->addInterface(interface);
        }
    }
    
    registerType(name, classType);
    return classType;
}

// 自动注册宏系统（仅用于全局注册表）
void TypeRegistry::registerBuiltinType(const string& name, ObjectType* type) {
    TypeRegistry* globalRegistry = getGlobalInstance();
    if (globalRegistry) {
        globalRegistry->registerType(name, type);
    }
}

// 从字符串创建类型
ObjectType* TypeRegistry::createTypeFromString(const string& typeName) {
    // 首先检查是否已经存在该类型
    ObjectType* existingType = getType(typeName);
    if (existingType) {
        return existingType;
    }
    
    // 检查全局注册表中是否有该类型
    TypeRegistry* globalRegistry = getGlobalInstance();
    if (globalRegistry && globalRegistry != this) {
        ObjectType* globalType = globalRegistry->getType(typeName);
        if (globalType) {
            return globalType;
        }
    }
    
    // 根据类型名称创建相应的类型
    if (typeName == "int" || typeName == "integer") {
        // 从全局注册表获取内置的int类型
        if (globalRegistry) {
            return globalRegistry->getType("int");
        }
        return nullptr;
    } else if (typeName == "double" || typeName == "float") {
        // 从全局注册表获取内置的double类型
        if (globalRegistry) {
            return globalRegistry->getType("double");
        }
        return nullptr;
    } else if (typeName == "bool" || typeName == "boolean") {
        // 从全局注册表获取内置的bool类型
        if (globalRegistry) {
            return globalRegistry->getType("bool");
        }
        return nullptr;
    } else if (typeName == "string") {
        // 从全局注册表获取内置的string类型
        if (globalRegistry) {
            return globalRegistry->getType("string");
        }
        return nullptr;
    } else if (typeName == "char") {
        // 从全局注册表获取内置的char类型
        if (globalRegistry) {
            return globalRegistry->getType("char");
        }
        return nullptr;
    } else if (typeName == "auto") {
        // auto类型，返回nullptr表示需要类型推断
        return nullptr;
    } else {
        // 用户定义类型，尝试查找
        return getType(typeName);
    }
}