#include "interpreter/object_factory.h"
#include "interpreter/value.h"
#include "interpreter/builtin_type.h"
#include "interpreter/type_registry.h"
#include "interpreter/logger.h"
#include <algorithm>
#include <stdexcept>

using namespace std;

// ==================== ObjectFactory实现 ====================

// 构造函数
ObjectFactory::ObjectFactory() : initialized(false), typeRegistry(nullptr), interpreter(nullptr) {
    initializeCreators();
}

// 析构函数
ObjectFactory::~ObjectFactory() {
    // 清理资源 - 清空指针避免访问已销毁的对象
    typeRegistry = nullptr;
    interpreter = nullptr;
}

// 初始化工厂映射 - 现在只是设置标志
void ObjectFactory::initializeCreators() {
    if (initialized) return;
    initialized = true;
}

// 根据Object类型创建对应的Value对象
Value* ObjectFactory::createValue(ObjectType* type) {
    if (!type) return new Null();
    
    string typeName = type->getTypeName();
    return createValue(typeName);
}

// 根据类型名称创建对应的Value对象
Value* ObjectFactory::createValue(const string& typeName) {
    // 使用switch语句替代map，避免lambda函数析构问题
    if (typeName == "int") {
        Integer* val = new Integer(0);
        TypeRegistry* registry = TypeRegistry::getInstance();
        if (registry) {
            val->setValueType(registry->getType("int"));
        }
        return val;
    } else if (typeName == "double") {
        Double* val = new Double(0.0);
        TypeRegistry* registry = TypeRegistry::getInstance();
        if (registry) {
            val->setValueType(registry->getType("double"));
        }
        return val;
    } else if (typeName == "char") {
        Char* val = new Char('\0');
        TypeRegistry* registry = TypeRegistry::getInstance();
        if (registry) {
            val->setValueType(registry->getType("char"));
        }
        return val;
    } else if (typeName == "bool") {
        Bool* val = new Bool(false);
        TypeRegistry* registry = TypeRegistry::getInstance();
        if (registry) {
            val->setValueType(registry->getType("bool"));
        }
        return val;
    } else if (typeName == "null") {
        Null* val = new Null();
        TypeRegistry* registry = TypeRegistry::getInstance();
        if (registry) {
            val->setValueType(registry->getType("null"));
        }
        return val;
    } else if (typeName == "string") {
        String* val = new String("");
        TypeRegistry* registry = TypeRegistry::getInstance();
        if (registry) {
            val->setValueType(registry->getType("string"));
        }
        return val;
    } else if (typeName == "array") {
        return new Array();
    } else if (typeName == "dict") {
        return new Dict();
    }
    
    // 如果找不到对应的创建函数，通过TypeRegistry获取类型对象
    ObjectType* type = TypeRegistry::getInstance()->getType(typeName);
    if (type) {
        return type->createDefaultValue();
    }
    
    // 如果找不到类型，返回null
    return new Null();
}

// 根据Object类型创建默认值
Value* ObjectFactory::createDefaultValue(ObjectType* type) {
    return createValue(type);
}

// 根据类型名称创建默认值
Value* ObjectFactory::createDefaultValue(const string& typeName) {
    return createValue(typeName);
}

// 创建null值
Value* ObjectFactory::createNull() {
    return new Null();
}

// 创建指定类型的null值（用于类型转换）
Value* ObjectFactory::createNullForType(ObjectType* type) {
    if (!type) return new Null();
    
    string typeName = type->getTypeName();
    return createNullForType(typeName);
}

// 创建指定类型的null值（用于类型转换）
Value* ObjectFactory::createNullForType(const string& typeName) {
    if (typeName == "null") {
        return new Null();
    } else if (typeName == "string") {
        return new String("null");
    } else if (typeName == "bool") {
        return new Bool(false);
    } else if (typeName == "int") {
        return new Integer(0);
    } else if (typeName == "double") {
        return new Double(0.0);
    } else if (typeName == "char") {
        return new Char('\0');
    } else if (typeName == "array") {
        return new Array();
    } else if (typeName == "dict") {
        return new Dict();
    }
    
    // 对于其他类型，返回null
    return new Null();
}

// 检查类型是否支持null值
bool ObjectFactory::supportsNull(ObjectType* type) {
    if (!type) return true;
    return supportsNull(type->getTypeName());
}

// 检查类型名称是否支持null值
bool ObjectFactory::supportsNull(const string& typeName) {
    // 所有类型都支持null值
    return true;
}

// ==================== 静态便捷方法实现 ====================

Value* ObjectFactory::createValueStatic(ObjectType* type) {
    if (!type) return new Null();
    return type->createDefaultValue();
}

Value* ObjectFactory::createValueStatic(const string& typeName) {
    ObjectType* type = TypeRegistry::getInstance()->getType(typeName);
    if (type) {
        return type->createDefaultValue();
    }
    return new Null();
}

Value* ObjectFactory::createDefaultValueStatic(ObjectType* type) {
    if (!type) return new Null();
    return type->createDefaultValue();
}

Value* ObjectFactory::createDefaultValueStatic(const string& typeName) {
    ObjectType* type = TypeRegistry::getInstance()->getType(typeName);
    if (type) {
        return type->createDefaultValue();
    }
    return new Null();
}

Value* ObjectFactory::createNullStatic() {
    return new Null();
}

Value* ObjectFactory::createNullForTypeStatic(ObjectType* type) {
    if (!type) return new Null();
    string typeName = type->getTypeName();
    return createNullForTypeStatic(typeName);
}

Value* ObjectFactory::createNullForTypeStatic(const string& typeName) {
    if (typeName == "null") {
        return new Null();
    } else if (typeName == "string") {
        return new String("null");
    } else if (typeName == "bool") {
        return new Bool(false);
    } else if (typeName == "int") {
        return new Integer(0);
    } else if (typeName == "double") {
        return new Double(0.0);
    } else if (typeName == "char") {
        return new Char('\0');
    } else if (typeName == "array") {
        return new Array();
    } else if (typeName == "dict") {
        return new Dict();
    }
    return new Null();
}

bool ObjectFactory::supportsNullStatic(ObjectType* type) {
    return true; // 所有类型都支持null值
}

bool ObjectFactory::supportsNullStatic(const string& typeName) {
    return true; // 所有类型都支持null值
}

// ==================== 结构体和类实例化方法实现 ====================

Value* ObjectFactory::createStructInstance(StructType* structType, const vector<Value*>& args) {
    if (!structType) {
        LOG_ERROR("ObjectFactory::createStructInstance: structType is null");
        return nullptr;
    }
    
    LOG_DEBUG("ObjectFactory::createStructInstance: creating instance for struct '" + structType->getTypeName() + "'");
    
    // 尝试调用结构体的构造函数（同名方法）
    string constructorName = structType->getTypeName();
    const auto& methods = structType->getMethods();
    auto it = methods.find(constructorName);
    if (it != methods.end()) {
        function<Value*(Value*, vector<Value*>&)> constructor = it->second;
        
        // 如果有构造函数，调用它
        LOG_DEBUG("ObjectFactory::createStructInstance: calling constructor '" + constructorName + "'");
        return constructor(nullptr, const_cast<vector<Value*>&>(args));
    } else {
        // 如果没有构造函数，使用默认的实例化逻辑
        LOG_DEBUG("ObjectFactory::createStructInstance: no constructor found, using default instantiation");
        
        // 创建字典来存储实例的成员
        Dict* instance = new Dict();
        
        // 获取结构体的所有成员信息
        vector<string> memberNames = structType->getMemberNames();
        
        if (memberNames.empty()) {
            LOG_WARN("ObjectFactory::createStructInstance: struct '" + structType->getTypeName() + "' has no members");
            return instance;
        }
        
        // 处理参数初始化
        if (!args.empty() && dynamic_cast<Dict*>(args[0])) {
            // 结构体实例化语法：Person {name: "Alice", age: 25}
            Dict* memberDict = dynamic_cast<Dict*>(args[0]);
            initializeFromDict(instance, structType, memberDict);
        } else {
            // 构造函数语法：Person("Alice", 25) - 按参数顺序初始化
            initializeFromArgs(instance, structType, args);
        }
        
        LOG_DEBUG("ObjectFactory::createStructInstance: successfully created struct instance with " + 
                  to_string(memberNames.size()) + " members");
        
        return instance;
    }
}

Value* ObjectFactory::createClassInstance(ClassType* classType, const vector<Value*>& args) {
    if (!classType) {
        LOG_ERROR("ObjectFactory::createClassInstance: classType is null");
        return nullptr;
    }
    
    LOG_DEBUG("ObjectFactory::createClassInstance: creating instance for class '" + classType->getTypeName() + "'");
    
    // 尝试调用类的构造函数（同名方法）
    string constructorName = classType->getTypeName();
    const auto& methods = classType->getMethods();
    auto it = methods.find(constructorName);
    if (it != methods.end()) {
        function<Value*(Value*, vector<Value*>&)> constructor = it->second;
        
        // 如果有构造函数，调用它
        LOG_DEBUG("ObjectFactory::createClassInstance: calling constructor '" + constructorName + "'");
        return constructor(nullptr, const_cast<vector<Value*>&>(args));
    } else {
        // 如果没有构造函数，使用默认的实例化逻辑
        LOG_DEBUG("ObjectFactory::createClassInstance: no constructor found, using default instantiation");
        
        // 创建字典来存储实例的成员
        Dict* instance = new Dict();
        
        // 获取类的所有成员信息
        vector<string> memberNames = classType->getMemberNames();
        
        if (memberNames.empty()) {
            LOG_WARN("ObjectFactory::createClassInstance: class '" + classType->getTypeName() + "' has no members");
            return instance;
        }
        
        // 处理参数初始化
        if (!args.empty() && dynamic_cast<Dict*>(args[0])) {
            // 类实例化语法：Student {name: "Bob", grade: 10}
            Dict* memberDict = dynamic_cast<Dict*>(args[0]);
            initializeFromDict(instance, classType, memberDict);
        } else {
            // 构造函数语法：Student("Bob", 10) - 按参数顺序初始化
            initializeFromArgs(instance, classType, args);
        }
        
        // 初始化类的方法（结构体不需要）
        initializeMethods(instance, classType);
        
        LOG_DEBUG("ObjectFactory::createClassInstance: successfully created class instance with " + 
                  to_string(memberNames.size()) + " members");
        
        return instance;
    }
}

// 根据结构体类型名称创建实例
Value* ObjectFactory::createStructInstance(const string& structTypeName, const vector<Value*>& args) {
    if (structTypeName.empty()) {
        LOG_ERROR("ObjectFactory::createStructInstance: structTypeName is empty");
        return nullptr;
    }
    
    // 通过TypeRegistry获取结构体类型
    StructType* structType = nullptr;
    if (typeRegistry) {
        ObjectType* type = typeRegistry->getType(structTypeName);
        structType = dynamic_cast<StructType*>(type);
    }
    
    if (!structType) {
        LOG_ERROR("ObjectFactory::createStructInstance: struct type '" + structTypeName + "' not found");
        return nullptr;
    }
    
    return createStructInstance(structType, args);
}

// 根据类类型名称创建实例
Value* ObjectFactory::createClassInstance(const string& classTypeName, const vector<Value*>& args) {
    if (classTypeName.empty()) {
        LOG_ERROR("ObjectFactory::createClassInstance: classTypeName is empty");
        return nullptr;
    }
    
    // 通过TypeRegistry获取类类型
    ClassType* classType = nullptr;
    if (typeRegistry) {
        ObjectType* type = typeRegistry->getType(classTypeName);
        classType = dynamic_cast<ClassType*>(type);
    }
    
    if (!classType) {
        LOG_ERROR("ObjectFactory::createClassInstance: class type '" + classTypeName + "' not found");
        return nullptr;
    }
    
    return createClassInstance(classType, args);
}

// 静态便捷方法实现
Value* ObjectFactory::createStructInstanceStatic(StructType* structType, const vector<Value*>& args) {
    // 直接返回nullptr，避免创建栈上的ObjectFactory实例
    return nullptr;
}

Value* ObjectFactory::createStructInstanceStatic(const string& structTypeName, const vector<Value*>& args) {
    // 直接返回nullptr，避免创建栈上的ObjectFactory实例
    return nullptr;
}

Value* ObjectFactory::createClassInstanceStatic(ClassType* classType, const vector<Value*>& args) {
    // 直接返回nullptr，避免创建栈上的ObjectFactory实例
    return nullptr;
}

Value* ObjectFactory::createClassInstanceStatic(const string& classTypeName, const vector<Value*>& args) {
    // 直接返回nullptr，避免创建栈上的ObjectFactory实例
    return nullptr;
}

// ==================== 依赖注入实现 ====================

void ObjectFactory::setTypeRegistry(TypeRegistry* registry) {
    typeRegistry = registry;
}

void ObjectFactory::setInterpreter(Interpreter* interp) {
    interpreter = interp;
}

// ==================== 通用实例化辅助方法实现 ====================

void ObjectFactory::initializeFromDict(Dict* instance, ClassType* type, Dict* memberDict) {
    if (!instance || !type || !memberDict) {
        LOG_ERROR("ObjectFactory::initializeFromDict: null parameter");
        return;
    }
    
    string typeName = type->getTypeName();
    LOG_DEBUG("ObjectFactory::initializeFromDict: initializing " + typeName + " from dictionary");
    
    // 获取成员名称
    vector<string> memberNames = type->getMemberNames();
    
    // 按照定义的成员顺序初始化
    for (const string& memberName : memberNames) {
        // 检查可见性（结构体所有成员都是公有的，类可能有私有成员）
        VisibilityType visibility = type->getMemberVisibility(memberName);
        bool isStruct = dynamic_cast<StructType*>(type) != nullptr;
        
        // 结构体所有成员都是公有的，类需要检查可见性
        if (isStruct || visibility == VIS_PUBLIC) {
            Value* memberValue = memberDict->getEntry(memberName);
            if (memberValue) {
                // 如果提供了该成员的值，使用提供的值
                instance->setEntry(memberName, memberValue);
                LOG_DEBUG("ObjectFactory::initializeFromDict: set member '" + memberName + "' from dict");
            } else {
                // 使用默认值
                Value* defaultVal = createMemberDefaultValue(type, memberName);
                instance->setEntry(memberName, defaultVal);
                LOG_DEBUG("ObjectFactory::initializeFromDict: set member '" + memberName + "' to default value");
            }
        } else {
            LOG_DEBUG("ObjectFactory::initializeFromDict: skipping private/protected member '" + memberName + "'");
        }
    }
}

void ObjectFactory::initializeFromArgs(Dict* instance, ClassType* type, const vector<Value*>& args) {
    if (!instance || !type) {
        LOG_ERROR("ObjectFactory::initializeFromArgs: null parameter");
        return;
    }
    
    string typeName = type->getTypeName();
    LOG_DEBUG("ObjectFactory::initializeFromArgs: initializing " + typeName + " from arguments");
    
    // 获取成员名称
    vector<string> memberNames = type->getMemberNames();
    
    // 按参数顺序初始化成员
    for (size_t i = 0; i < memberNames.size() && i < args.size(); ++i) {
        const string& memberName = memberNames[i];
        instance->setEntry(memberName, args[i]);
        LOG_DEBUG("ObjectFactory::initializeFromArgs: set member '" + memberName + "' from arg[" + to_string(i) + "]");
    }
    
    // 为未初始化的成员设置默认值
    for (size_t i = args.size(); i < memberNames.size(); ++i) {
        const string& memberName = memberNames[i];
        Value* defaultVal = createMemberDefaultValue(type, memberName);
        instance->setEntry(memberName, defaultVal);
        LOG_DEBUG("ObjectFactory::initializeFromArgs: set member '" + memberName + "' to default value");
    }
}

Value* ObjectFactory::createMemberDefaultValue(ClassType* type, const string& memberName) {
    if (!type) {
        LOG_ERROR("ObjectFactory::createMemberDefaultValue: type is null");
        return new Null();
    }
    
    // 获取成员类型
    ObjectType* memberType = type->getMemberType(memberName);
    if (!memberType) {
        LOG_WARN("ObjectFactory::createMemberDefaultValue: member '" + memberName + "' not found in " + type->getTypeName());
        return new Null();
    }
    
    // 使用ObjectFactory创建该类型的默认值
    return createDefaultValue(memberType);
}

void ObjectFactory::initializeMethods(Dict* instance, ClassType* type) {
    if (!instance || !type) {
        LOG_ERROR("ObjectFactory::initializeMethods: null parameter");
        return;
    }
    
    string typeName = type->getTypeName();
    LOG_DEBUG("ObjectFactory::initializeMethods: initializing methods for " + typeName);
    
    // 检查是否为结构体（结构体不需要初始化方法）
    if (dynamic_cast<StructType*>(type) != nullptr) {
        LOG_DEBUG("ObjectFactory::initializeMethods: skipping method initialization for struct " + typeName);
        return;
    }
    
    // 获取类型的所有方法
    const map<string, function<Value*(Value*, vector<Value*>&)>>& methods = type->getMethods();
    
    // 将方法绑定到实例
    for (const auto& methodPair : methods) {
        const string& methodName = methodPair.first;
        const auto& methodFunc = methodPair.second;
        
        // 创建方法包装器
        // TODO: 这里需要实现方法绑定逻辑
        // 目前只是记录日志，实际实现需要创建方法对象
        LOG_DEBUG("ObjectFactory::initializeMethods: binding method '" + methodName + "' to instance");
    }
    
    LOG_DEBUG("ObjectFactory::initializeMethods: initialized " + to_string(methods.size()) + " methods for " + typeName);
}

// ==================== 全局工厂方法（向后兼容） ====================
// 全局工厂方法：根据ObjectType*类型创建默认值
Value* createDefaultValue(ObjectType* type) {
    return ObjectFactory::createDefaultValueStatic(type);
}

// 全局工厂方法：根据类型名称创建默认值
Value* createDefaultValue(const string& typeName) {
    return ObjectFactory::createDefaultValueStatic(typeName);
} 