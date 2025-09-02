#include "interpreter/values/value.h"
#include "interpreter/types/types.h"
#include "interpreter/scope/scope.h"
#include "interpreter/utils/logger.h"
#include <algorithm>
#include <stdexcept>

using namespace std;

// ==================== 全局工厂方法实现 ====================
Value* createDefaultValue(ObjectType* type) {
    if (!type) return nullptr;
    return type->createDefaultValue();
}

Value* createDefaultValue(const string& typeName) {
    // 这里需要TypeRegistry，暂时返回nullptr
    return nullptr;
}

ObjectFactory* getObjectFactory() {
    static ObjectFactory factory;
    return &factory;
}

// ==================== ObjectFactory实现 ====================

// 构造函数
ObjectFactory::ObjectFactory() : initialized(false), typeRegistry(nullptr) {
    initializeCreators();
}

// 简化构造函数 - 只需要TypeRegistry
ObjectFactory::ObjectFactory(TypeRegistry* registry) 
    : initialized(false), typeRegistry(registry) {
    initializeCreators();
}

// 析构函数
ObjectFactory::~ObjectFactory() {
    // 清理资源 - 清空指针避免访问已销毁的对象
    typeRegistry = nullptr;
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
    // 直接通过类型系统创建对象
    ObjectType* type = TypeRegistry::getGlobalInstance()->getType(typeName);
    if (type) {
        return type->createDefaultValue();
    }
    
    // 如果找不到类型，返回null
    return new Null();
}

// 根据ObjectType类型创建默认值
Value* ObjectFactory::createDefaultValue(ObjectType* type) {
    if (!type) return new Null();
    return type->createDefaultValue();
}

// 创建null值
Value* ObjectFactory::createNull() {
    return new Null();
}

// ==================== 静态便捷方法实现 ====================

Value* ObjectFactory::createValueStatic(ObjectType* type) {
    if (!type) return new Null();
    return type->createDefaultValue();
}

Value* ObjectFactory::createValueStatic(const string& typeName) {
    ObjectType* type = TypeRegistry::getGlobalInstance()->getType(typeName);
    if (type) {
        return type->createDefaultValue();
    }
    return new Null();
}

Value* ObjectFactory::createDefaultValueStatic(const string& typeName) {
    ObjectType* type = TypeRegistry::getGlobalInstance()->getType(typeName);
    if (type) {
        return type->createDefaultValue();
    }
    return new Null();
}

Value* ObjectFactory::createNullStatic() {
    return new Null();
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
// 注意：类型查找应该由调用者（类型系统）负责
Value* ObjectFactory::createStructInstance(const string& structTypeName, const vector<Value*>& args) {
    if (structTypeName.empty()) {
        LOG_ERROR("ObjectFactory::createStructInstance: structTypeName is empty");
        return nullptr;
    }
    
    // 类型查找应该由调用者负责，ObjectFactory只负责实例创建
    // 调用者应该先通过类型系统获取类型，然后调用createStructInstance(StructType*, args)
    LOG_ERROR("ObjectFactory::createStructInstance: 请使用 createStructInstance(StructType*, args) 方法");
    LOG_ERROR("类型查找应该由类型系统（TypeFactory/TypeRegistry）负责");
    return nullptr;
}

// 根据类类型名称创建实例
// 注意：类型查找应该由调用者（类型系统）负责
Value* ObjectFactory::createClassInstance(const string& classTypeName, const vector<Value*>& args) {
    if (classTypeName.empty()) {
        LOG_ERROR("ObjectFactory::createClassInstance: classTypeName is empty");
        return nullptr;
    }
    
    // 类型查找应该由调用者负责，ObjectFactory只负责实例创建
    // 调用者应该先通过类型系统获取类型，然后调用createClassInstance(ClassType*, args)
    LOG_ERROR("ObjectFactory::createClassInstance: 请使用 createClassInstance(ClassType*, args) 方法");
    LOG_ERROR("类型查找应该由类型系统（TypeFactory/TypeRegistry）负责");
    return nullptr;
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
        // 暂时假设所有成员都是公有的，因为ClassType没有提供getMemberVisibility方法
        bool isStruct = dynamic_cast<StructType*>(type) != nullptr;
        
        // 结构体所有成员都是公有的，类暂时也当作公有的
        if (isStruct || true) { // TODO: 需要添加getMemberVisibility方法到ClassType
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



// ==================== 函数创建方法实现 ====================











// ==================== 静态函数创建方法 ====================







// ==================== 基本类型创建方法实现 ====================

Bool* ObjectFactory::createBool(bool value) {
    Bool* val = new Bool(value);
    if (typeRegistry) {
        val->setValueType(typeRegistry->getType("bool"));
    }
    return val;
}

Integer* ObjectFactory::createInteger(int value) {
    Integer* val = new Integer(value);
    if (typeRegistry) {
        val->setValueType(typeRegistry->getType("int"));
    }
    return val;
}

Double* ObjectFactory::createDouble(double value) {
    Double* val = new Double(value);
    if (typeRegistry) {
        val->setValueType(typeRegistry->getType("double"));
    }
    return val;
}

Char* ObjectFactory::createChar(char value) {
    Char* val = new Char(value);
    if (typeRegistry) {
        val->setValueType(typeRegistry->getType("char"));
    }
    return val;
}

String* ObjectFactory::createString(const string& value) {
    String* val = new String(value);
    if (typeRegistry) {
        val->setValueType(typeRegistry->getType("string"));
    }
    return val;
}

Array* ObjectFactory::createArray(const vector<Value*>& elements) {
    return new Array(elements);
}

Dict* ObjectFactory::createDict(const vector<pair<string, Value*>>& pairs) {
    return new Dict();
}

Value* ObjectFactory::create(const string& typeName, const vector<Value*>& args) {
    if (typeName == "null") {
        return createNull();
    } else if (typeName == "bool") {
        if (args.empty()) {
            return createBool(false);
        } else if (args.size() == 1 && args[0]) {
            return createFromValue(args[0], "bool");
        }
        return createBool(false);
    } else if (typeName == "int") {
        if (args.empty()) {
            return createInteger(0);
        } else if (args.size() == 1 && args[0]) {
            return createFromValue(args[0], "int");
        }
        return createInteger(0);
    } else if (typeName == "double") {
        if (args.empty()) {
            return createDouble(0.0);
        } else if (args.size() == 1 && args[0]) {
            return createFromValue(args[0], "double");
        }
        return createDouble(0.0);
    } else if (typeName == "char") {
        if (args.empty()) {
            return createChar('\0');
        } else if (args.size() == 1 && args[0]) {
            return createFromValue(args[0], "char");
        }
        return createChar('\0');
    } else if (typeName == "string") {
        if (args.empty()) {
            return createString("");
        } else if (args.size() == 1 && args[0]) {
            return createFromValue(args[0], "string");
        }
        return createString("");
    } else if (typeName == "array") {
        return createArray();
    } else if (typeName == "dict") {
        return createDict();
    }
    
    // 如果找不到对应的创建函数，通过TypeRegistry获取类型对象
    if (typeRegistry) {
        ObjectType* type = typeRegistry->getType(typeName);
        if (type) {
            return type->createDefaultValue();
        }
    }
    
    return createNull();
}

Value* ObjectFactory::createFromValue(Value* source, const string& targetType) {
    if (!source) return nullptr;
    
    if (targetType == "bool") {
        return createBool(source->toBool());
    } else if (targetType == "int") {
        if (Integer* intVal = dynamic_cast<Integer*>(source)) {
            return createInteger(intVal->getValue());
        } else if (Double* doubleVal = dynamic_cast<Double*>(source)) {
            return createInteger(static_cast<int>(doubleVal->getValue()));
        } else if (String* strVal = dynamic_cast<String*>(source)) {
            try {
                int val = stoi(strVal->getValue());
                return createInteger(val);
            } catch (...) {
                return createInteger(0);
            }
        }
        return createInteger(0);
    } else if (targetType == "double") {
        if (Double* doubleVal = dynamic_cast<Double*>(source)) {
            return createDouble(doubleVal->getValue());
        } else if (Integer* intVal = dynamic_cast<Integer*>(source)) {
            return createDouble(static_cast<double>(intVal->getValue()));
        } else if (String* strVal = dynamic_cast<String*>(source)) {
            try {
                double val = stod(strVal->getValue());
                return createDouble(val);
            } catch (...) {
                return createInteger(0);
            }
        }
        return createDouble(0.0);
    } else if (targetType == "string") {
        return createString(source->toString());
    } else if (targetType == "char") {
        if (Char* charVal = dynamic_cast<Char*>(source)) {
            return createChar(charVal->getValue());
        } else if (String* strVal = dynamic_cast<String*>(source)) {
            string str = strVal->getValue();
            return createChar(str.empty() ? '\0' : str[0]);
        }
        return createChar('\0');
    }
    
    return nullptr;
}



vector<Value*> ObjectFactory::createMultiple(const string& typeName, int count, const vector<Value*>& args) {
    vector<Value*> results;
    for (int i = 0; i < count; ++i) {
        Value* value = create(typeName, args);
        if (value) {
            results.push_back(value);
        }
    }
    return results;
}



// ==================== 静态基本类型创建方法实现 ====================

Bool* ObjectFactory::createBoolStatic(bool value) {
    ObjectFactory factory;
    return factory.createBool(value);
}

Integer* ObjectFactory::createIntegerStatic(int value) {
    ObjectFactory factory;
    return factory.createInteger(value);
}

Double* ObjectFactory::createDoubleStatic(double value) {
    ObjectFactory factory;
    return factory.createDouble(value);
}

Char* ObjectFactory::createCharStatic(char value) {
    ObjectFactory factory;
    return factory.createChar(value);
}

String* ObjectFactory::createStringStatic(const string& value) {
    ObjectFactory factory;
    return factory.createString(value);
}

Array* ObjectFactory::createArrayStatic(const vector<Value*>& elements) {
    ObjectFactory factory;
    return factory.createArray(elements);
}

Dict* ObjectFactory::createDictStatic(const vector<pair<string, Value*>>& pairs) {
    ObjectFactory factory;
    return factory.createDict(pairs);
}

Value* ObjectFactory::createStatic(const string& typeName, const vector<Value*>& args) {
    ObjectFactory factory;
    return factory.create(typeName, args);
}

Value* ObjectFactory::createFromValueStatic(Value* source, const string& targetType) {
    ObjectFactory factory;
    return factory.createFromValue(source, targetType);
}

 