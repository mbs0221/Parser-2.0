#include "interpreter/types/types.h"
#include "interpreter/values/value.h"

#include "interpreter/utils/logger.h"
#include <algorithm>
#include <sstream>

using namespace std;

// ==================== ObjectType实现 ====================
ObjectType::ObjectType(const string& name, bool primitive, bool mutable_, bool reference, bool container, bool userDefined, ObjectType* parent)
    : typeName(name), isPrimitive(primitive), isMutable(mutable_), 
      isReference(reference), isContainer(container), isUserDefined(userDefined),
      parentType(parent) {
    
    // 自动注册构造函数方法
    registerConstructor({}, [this](Value* instance, vector<Value*>& args) -> Value* {
        return createValueWithArgs(args);
    });
    
    // 自动注册成员访问方法
    registerMemberAccess();
}

string ObjectType::getTypeName() const {
    return typeName;
}

bool ObjectType::isPrimitiveType() const {
    return isPrimitive;
}

bool ObjectType::isMutableType() const {
    return isMutable;
}

bool ObjectType::isReferenceType() const {
    return isReference;
}

bool ObjectType::isContainerType() const {
    return isContainer;
}

bool ObjectType::isUserDefinedType() const {
    return isUserDefined;
}

ObjectType* ObjectType::getParentType() const {
    return parentType;
}

void ObjectType::setParentType(ObjectType* parent) {
    parentType = parent;
}

const vector<ObjectType*>& ObjectType::getInterfaces() const {
    return interfaces;
}

void ObjectType::addInterface(ObjectType* interface) {
    interfaces.push_back(interface);
}

bool ObjectType::isSubtypeOf(ObjectType* other) const {
    if (!other) return false;
    
    // 检查直接父类
    if (parentType == other) return true;
    
    // 递归检查父类
    if (parentType) return parentType->isSubtypeOf(other);
    
    return false;
}

bool ObjectType::implements(ObjectType* interface) const {
    if (!interface) return false;
    
    // 检查直接实现的接口
    for (ObjectType* iface : interfaces) {
        if (iface == interface) return true;
    }
    
    // 递归检查父类实现的接口
    if (parentType) return parentType->implements(interface);
    
    return false;
}

void ObjectType::registerMethod(const string& name, function<Value*(Value*, vector<Value*>&)> method) {
    // 注册无参数方法
    methods[FunctionSignature(name)] = method;
}

void ObjectType::registerMethod(const string& name,                          function<Value*(Value*, vector<Value*>&)> method, const vector<string>& parameterTypes) {
    methods[FunctionSignature(name, parameterTypes)] = method;
}

void ObjectType::registerMethod(const FunctionSignature& signature, 
                               function<Value*(Value*, vector<Value*>&)> method) {
    methods[signature] = method;
}

void ObjectType::registerMethod(const FunctionPrototype* prototype, 
                               function<Value*(Value*, vector<Value*>&)> method) {
    if (prototype) {
        FunctionSignature signature(prototype);
        registerMethod(signature, method);
    }
}

// ==================== 静态方法注册 ====================
void ObjectType::registerStaticMethod(const string& name, function<Value*(vector<Value*>&)> method) {
    // 注册无参数静态方法
    staticMethods[FunctionSignature(name)] = method;
}

void ObjectType::registerStaticMethod(const string& name, function<Value*(vector<Value*>&)> method, 
                                     const vector<string>& parameterTypes) {
    staticMethods[FunctionSignature(name, parameterTypes)] = method;
}

void ObjectType::registerStaticMethod(const FunctionSignature& signature, 
                                     function<Value*(vector<Value*>&)> method) {
    staticMethods[signature] = method;
}

void ObjectType::registerStaticMethod(const FunctionPrototype* prototype, 
                                     function<Value*(vector<Value*>&)> method) {
    if (prototype) {
        FunctionSignature signature(prototype);
        registerStaticMethod(signature, method);
    }
}

void ObjectType::registerConstructor(const vector<string>& parameterTypes, 
                                   function<Value*(Value*, vector<Value*>&)> constructor) {
    if (!constructor) {
        // 默认构造函数
        constructor = [this](Value* instance, vector<Value*>& args) -> Value* {
            return createValueWithArgs(args);
        };
    }
    methods[FunctionSignature(typeName, parameterTypes)] = constructor;
}

void ObjectType::registerConstructor(const FunctionSignature& signature, 
                                   function<Value*(Value*, vector<Value*>&)> constructor) {
    if (!constructor) {
        // 默认构造函数
        constructor = [this](Value* instance, vector<Value*>& args) -> Value* {
            return createValueWithArgs(args);
        };
    }
    methods[signature] = constructor;
}

void ObjectType::registerConstructor(const FunctionPrototype* prototype, 
                                   function<Value*(Value*, vector<Value*>&)> constructor) {
    if (prototype) {
        FunctionSignature signature(prototype);
        registerConstructor(signature, constructor);
    }
}

const map<FunctionSignature, function<Value*(Value*, vector<Value*>&)>>& ObjectType::getMethods() const {
    return methods;
}

const map<FunctionSignature, function<Value*(vector<Value*>&)>>& ObjectType::getStaticMethods() const {
    return staticMethods;
}

bool ObjectType::hasMethod(const string& methodName, const vector<Value*>& args) const {
    LOG_DEBUG("ObjectType::hasMethod: checking method '" + methodName + "' with " + to_string(args.size()) + " args");
    
    // 在当前类型中查找匹配的方法
    for (const auto& pair : methods) {
        LOG_DEBUG("  checking signature: " + pair.first.toString());
        if (pair.first.matches(methodName, args)) {
            LOG_DEBUG("  found matching method: " + pair.first.toString());
            return true;
        }
    }
    
    // 在父类中查找方法
    if (parentType && parentType->hasMethod(methodName, args)) {
        return true;
    }
    
    // 在接口中查找方法
    for (ObjectType* interface : interfaces) {
        if (interface->hasMethod(methodName, args)) {
            return true;
        }
    }
    
    LOG_DEBUG("  method '" + methodName + "' not found");
    return false;
}

bool ObjectType::hasMethodName(const string& methodName) const {
    for (const auto& pair : methods) {
        if (pair.first.getName() == methodName) {
            return true;
        }
    }
    
    // 在父类中查找方法
    if (parentType && parentType->hasMethodName(methodName)) {
        return true;
    }
    
    // 在接口中查找方法
    for (ObjectType* interface : interfaces) {
        if (interface->hasMethodName(methodName)) {
            return true;
        }
    }
    
    return false;
}

// ==================== 静态方法检查 ====================
bool ObjectType::hasStaticMethod(const string& methodName, const vector<Value*>& args) const {
    LOG_DEBUG("ObjectType::hasStaticMethod: checking static method '" + methodName + "' with " + to_string(args.size()) + " args");
    
    // 在当前类型中查找匹配的静态方法
    for (const auto& pair : staticMethods) {
        LOG_DEBUG("  checking static signature: " + pair.first.toString());
        if (pair.first.matches(methodName, args)) {
            LOG_DEBUG("  found matching static method: " + pair.first.toString());
            return true;
        }
    }
    
    // 在父类中查找静态方法
    if (parentType && parentType->hasStaticMethod(methodName, args)) {
        return true;
    }
    
    // 在接口中查找静态方法
    for (ObjectType* interface : interfaces) {
        if (interface->hasStaticMethod(methodName, args)) {
            return true;
        }
    }
    
    LOG_DEBUG("  static method '" + methodName + "' not found");
    return false;
}

bool ObjectType::hasStaticMethodName(const string& methodName) const {
    for (const auto& pair : staticMethods) {
        if (pair.first.getName() == methodName) {
            return true;
        }
    }
    
    // 在父类中查找静态方法
    if (parentType && parentType->hasStaticMethodName(methodName)) {
        return true;
    }
    
    // 在接口中查找静态方法
    for (ObjectType* interface : interfaces) {
        if (interface->hasStaticMethodName(methodName)) {
            return true;
        }
    }
    
    return false;
}

Value* ObjectType::callMethod(Value* instance, const string& methodName, vector<Value*>& args) {
    // 在当前类型中查找匹配的方法
    for (const auto& pair : methods) {
        if (pair.first.matches(methodName, args)) {
            return pair.second(instance, args);
        }
    }
    
    // 在父类中查找方法
    if (parentType) {
        return parentType->callMethod(instance, methodName, args);
    }
    
    // 在接口中查找方法
    for (ObjectType* interface : interfaces) {
        Value* result = interface->callMethod(instance, methodName, args);
        if (result) return result;
    }
    
    return nullptr;
}

// ==================== 静态方法调用 ====================
Value* ObjectType::callStaticMethod(const string& methodName, vector<Value*>& args) {
    // 在当前类型中查找匹配的静态方法
    for (const auto& pair : staticMethods) {
        if (pair.first.matches(methodName, args)) {
            return pair.second(args);
        }
    }
    
    // 在父类中查找静态方法
    if (parentType) {
        return parentType->callStaticMethod(methodName, args);
    }
    
    // 在接口中查找静态方法
    for (ObjectType* interface : interfaces) {
        Value* result = interface->callStaticMethod(methodName, args);
        if (result) return result;
    }
    
    return nullptr;
}

Value* ObjectType::convertTo(ObjectType* targetType, Value* value) {
    if (!targetType || !value) return nullptr;
    
    // 如果目标类型是当前类型，直接返回
    if (targetType == this) return value;
    
    // 尝试调用目标类型的转换方法
    vector<Value*> args = {value};
    return callMethod(nullptr, "convertTo", args);
}

bool ObjectType::isCompatibleWith(ObjectType* other) const {
    if (!other) return false;
    return typeName == other->getTypeName();
}

Value* ObjectType::createDefaultValue() {
    return createValueWithArgs({});
}

Value* ObjectType::createValueWithArgs(const vector<Value*>& args) {
    // 默认实现：创建空值
    return new Null();
}

void ObjectType::bindMethodsToInstance(Value* instance) {
    // 默认实现：将类型的方法绑定到实例
    // 子类可以重写此方法来自定义绑定逻辑
}

void ObjectType::initializeInstance(Value* instance, vector<Value*>& args) {
    // 默认实现：空操作
    // 子类可以重写此方法来自定义初始化逻辑
}

void ObjectType::registerConstructor() {
    // 为所有类型自动注册同名构造函数（使用类型名称作为方法名）
    // 注册默认构造函数，使用类型名称作为方法名
    registerConstructor({}, [this](Value* instance, vector<Value*>& args) -> Value* {
        return createValueWithArgs(args);
    });
}

void ObjectType::registerMemberAccess() {
    // 为所有类型自动注册成员访问方法
    registerMethod("operator.", [this](Value* instance, vector<Value*>& args) -> Value* {
        if (args.size() >= 1 && args[0]) {
            // 成员访问操作
            string memberName = args[0]->toString();
            return accessMember(instance, memberName);
        }
        return nullptr;
    });
}

Value* ObjectType::accessMember(Value* instance, const string& memberName) {
    // 默认实现：查找并返回实例的成员
    if (!instance) return nullptr;
    
    // 首先检查是否有同名的方法
    if (hasMethodName(memberName)) {
        // 返回一个方法调用器或者方法引用
        // 这里可以返回一个包装了方法的对象
        return nullptr; // 暂时返回nullptr，后续可以实现方法引用
    }
    
    // 检查是否有同名的成员变量
    // 这里需要根据具体的Value类型来实现
    // 暂时返回nullptr
    return nullptr;
}

bool ObjectType::hasMember(const string& memberName) const {
    // 默认实现：检查是否有同名的方法或成员变量
    // 首先检查是否有同名的方法
    if (hasMethodName(memberName)) {
        return true;
    }
    
    // 检查是否有同名的成员变量
    // 这里需要根据具体的类型来实现
    // 暂时返回false
    return false;
}

string ObjectType::getTypeDescription() const {
    stringstream ss;
    ss << "Type: " << typeName;
    if (isPrimitive) ss << " (Primitive)";
    if (isMutable) ss << " (Mutable)";
    if (isReference) ss << " (Reference)";
    if (isContainer) ss << " (Container)";
    if (isUserDefined) ss << " (UserDefined)";
    if (parentType) ss << " extends " << parentType->getTypeName();
    return ss.str();
}

bool ObjectType::operator==(const ObjectType& other) const {
    return typeName == other.typeName;
}

bool ObjectType::operator!=(const ObjectType& other) const {
    return !(*this == other);
}
