#ifndef TYPE_REGISTRY_H
#define TYPE_REGISTRY_H

#include "interpreter/value.h"
#include "interpreter/builtin_type.h"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <stdexcept>

using namespace std;



// 基本类型定义已移动到 builtin_type.h

// ==================== 类型注册表 ====================
// 专注于运行时类型的注册和管理
// 职责：类型定义、类型注册、类型查询
// 不负责：对象创建、变量管理、作用域管理
class TypeRegistry {
private:
    map<string, ObjectType*> types;
    
    TypeRegistry() {
        // 不再自动初始化内置类型，改为通过宏注册
    }
    
    // 移除initializeBuiltinTypes方法，改为通过宏注册

public:
    // 自动注册宏系统
    static void registerBuiltinType(const string& name, ObjectType* type);
    
    // 静态注册器类，用于在main()之前自动注册
    class AutoRegistrar {
    public:
        AutoRegistrar(const string& name, ObjectType* type) {
            TypeRegistry::getInstance()->registerBuiltinType(name, type);
        }
    };
    
    static TypeRegistry* getInstance() {
        static TypeRegistry instance;
        return &instance;
    }
    
    // 获取类型
    ObjectType* getType(const string& name) {
        auto it = types.find(name);
        return (it != types.end()) ? it->second : nullptr;
    }
    
    // 根据ObjectType指针获取类型名称
    string getTypeName(ObjectType* type) const {
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
    
    // 注册结构体类型
    void registerStructType(const string& name, StructType* type) {
        types[name] = type;
    }
    
    // 注册类类型
    void registerClassType(const string& name, ClassType* type) {
        types[name] = type;
    }
    
    // 创建用户定义类型的辅助方法
    Interface* createInterface(const string& name);
    StructType* createStructType(const string& name);
    ClassType* createClassType(const string& name);
    ClassType* createClassType(const string& name, const string& parentTypeName);
    ClassType* createClassType(const string& name, const vector<string>& interfaceNames);
    
    // 检查类型是否存在
    bool hasType(const string& name) const {
        return types.find(name) != types.end();
    }
    
    // 获取所有类型名称
    vector<string> getAllTypeNames() const {
        vector<string> names;
        for (const auto& pair : types) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    // 获取内置类型名称
    vector<string> getBuiltinTypeNames() const {
        vector<string> names;
        for (const auto& pair : types) {
            if (!pair.second->isUserDefinedType()) {
                names.push_back(pair.first);
            }
        }
        return names;
    }
    
    // 获取类类型名称
    vector<string> getClassTypeNames() const {
        vector<string> names;
        for (const auto& pair : types) {
            if (pair.second->isUserDefinedType()) {
                names.push_back(pair.first);
            }
        }
        return names;
    }
    
    // 获取类型信息
    string getTypeInfo(const string& name) const {
        auto it = types.find(name);
        if (it != types.end()) {
            return it->second->getTypeInfo();
        }
        return "Type '" + name + "' not found";
    }
    
    ~TypeRegistry() {
        for (auto& pair : types) {
            delete pair.second;
        }
        types.clear();
    }
};

// 全局访问函数
inline TypeRegistry* getTypeRegistry() {
    return TypeRegistry::getInstance();
}

// ==================== 自动注册宏系统 ====================
// 在main()之前自动注册内置类型的宏

// 注册基本类型的宏
#define REGISTER_BUILTIN_TYPE(typeName, typeClass) \
    static TypeRegistry::AutoRegistrar typeName##_registrar(#typeName, new typeClass());

// 注册带继承关系的类型宏
#define REGISTER_BUILTIN_TYPE_WITH_PARENT(typeName, typeClass, parentTypeName, parentTypeClass) \
    static TypeRegistry::AutoRegistrar typeName##_registrar(#typeName, []() -> ObjectType* { \
        typeClass* type = new typeClass(); \
        parentTypeClass* parent = new parentTypeClass(); \
        type->setParentType(parent); \
        return static_cast<ObjectType*>(type); \
    }());

// 注册带接口的类型宏
#define REGISTER_BUILTIN_TYPE_WITH_INTERFACE(typeName, typeClass, interfaceName, interfaceClass) \
    static TypeRegistry::AutoRegistrar typeName##_registrar(#typeName, []() -> ObjectType* { \
        typeClass* type = new typeClass(); \
        interfaceClass* interface = new interfaceClass(); \
        type->addInterface(interface); \
        return static_cast<ObjectType*>(type); \
    }());

#endif // TYPE_REGISTRY_H
