#include "interpreter/type_registry.h"
#include <algorithm>
#include <cmath>
#include <cctype>

using namespace std;

bool ObjectType::isSubtypeOf(ObjectType* other) const {
    if (!other) return false;
    
    // 检查自身
    if (this == other) return true;
    
    // 检查父类
    if (parentType && parentType->isSubtypeOf(other)) {
        return true;
    }
    
    // 检查接口
    for (ObjectType* interface : interfaces) {
        if (interface->isSubtypeOf(other)) {
            return true;
        }
    }
    
    return false;
}

bool ObjectType::implements(ObjectType* interface) const {
    if (!interface) return false;
    
    // 检查直接实现的接口
    for (ObjectType* impl : interfaces) {
        if (impl == interface || impl->isSubtypeOf(interface)) {
            return true;
        }
    }
    
    // 检查父类实现的接口
    if (parentType && parentType->implements(interface)) {
        return true;
    }
    
    return false;
}

// ==================== 类型注册表实现 ====================

void TypeRegistry::registerBuiltinType(const string& name, ObjectType* type) {
    TypeRegistry* instance = getInstance();
    if (instance) {
        instance->types[name] = type;
    }
}



// 创建用户定义类型的辅助方法实现
Interface* TypeRegistry::createInterface(const string& name) {
    if (hasType(name)) {
        throw runtime_error("Interface '" + name + "' already exists");
    }
    
    Interface* interface = new Interface(name);
    types[name] = interface;
    return interface;
}

StructType* TypeRegistry::createStructType(const string& name) {
    if (hasType(name)) {
        throw runtime_error("Type '" + name + "' already exists");
    }
    
    StructType* structType = new StructType(name);
    types[name] = structType;
    return structType;
}

ClassType* TypeRegistry::createClassType(const string& name) {
    if (hasType(name)) {
        throw runtime_error("Type '" + name + "' already exists");
    }
    
    ClassType* classType = new ClassType(name);
    types[name] = classType;
    return classType;
}

ClassType* TypeRegistry::createClassType(const string& name, const string& parentTypeName) {
    if (hasType(name)) {
        throw runtime_error("Type '" + name + "' already exists");
    }
    
            ObjectType* parentType = getType(parentTypeName);
    if (!parentType) {
        throw runtime_error("Parent type '" + parentTypeName + "' not found");
    }
    
    ClassType* classType = new ClassType(name);
    classType->setParentType(parentType);
    types[name] = classType;
    return classType;
}

ClassType* TypeRegistry::createClassType(const string& name, const vector<string>& interfaceNames) {
    if (hasType(name)) {
        throw runtime_error("Type '" + name + "' already exists");
    }
    
    ClassType* classType = new ClassType(name);
    
    for (const string& interfaceName : interfaceNames) {
        ObjectType* interface = getType(interfaceName);
        if (!interface) {
            throw runtime_error("Interface '" + interfaceName + "' not found");
        }
        classType->addInterface(interface);
    }
    
    types[name] = classType;
    return classType;
}











