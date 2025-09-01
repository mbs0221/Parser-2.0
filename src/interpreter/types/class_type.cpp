#include "interpreter/types/types.h"
#include "interpreter/values/value.h"

#include "interpreter/utils/logger.h"
#include <algorithm>

using namespace std;

// ==================== ClassType实现 ====================
ClassType::ClassType(const string& name, bool isPrimitive, bool isMutable, bool isReference)
    : ObjectType(name, isPrimitive, isMutable, isReference, false, true) {
    // 类类型自动注册成员访问方法
    registerMemberAccess();
}

void ClassType::addMember(const string& name, ObjectType* type, VisibilityType visibility, Value* initialValue) {
    memberTypes[name] = type;
    memberVisibility[name] = visibility;
    
    // 如果提供了初始值，存储它
    if (initialValue) {
        setMemberInitialValue(name, initialValue);
    }
    
    // 注册成员访问方法
    registerMemberAccessMethod(name, type);
}

void ClassType::addStaticMember(const string& name, ObjectType* type, Value* value, VisibilityType visibility) {
    staticMemberTypes[name] = type;
    staticMemberValues[name] = value;
    staticMemberVisibility[name] = visibility;
}

void ClassType::addUserMethod(const string& name, UserFunction* userFunc, VisibilityType visibility) {
    userFunctionMethods[name] = userFunc;
    memberVisibility[name] = visibility;
    
    // 注册用户方法
    registerMethod(name, [userFunc](Value* instance, vector<Value*>& args) -> Value* {
        return userFunc->call(args);
    });
}

void ClassType::addUserMethod(const FunctionPrototype* prototype, UserFunction* userFunc) {
    if (prototype && userFunc) {
        addUserMethod(prototype->name, userFunc, VIS_PUBLIC);
    }
}

void ClassType::addStaticMethod(const string& name, UserFunction* userFunc, VisibilityType visibility) {
    staticMethods[name] = userFunc;
    staticMemberVisibility[name] = visibility;
    
    // 注册静态方法
    registerMethod(name, [userFunc](Value* instance, vector<Value*>& args) -> Value* {
        return userFunc->call(args);
    });
}

void ClassType::addStaticMethod(const FunctionPrototype* prototype, UserFunction* userFunc) {
    if (prototype && userFunc) {
        addStaticMethod(prototype->name, userFunc, VIS_PUBLIC);
    }
}

void ClassType::setMemberInitialValue(const string& memberName, Value* initialValue) {
    // 如果已有初始值，先删除旧的
    auto it = memberInitialValues.find(memberName);
    if (it != memberInitialValues.end()) {
        delete it->second;
    }
    
    if (initialValue == nullptr) {
        // 如果传入nullptr，自动创建该成员类型的默认值
        ObjectType* memberType = getMemberType(memberName);
        if (memberType) {
            initialValue = memberType->createDefaultValue();
            LOG_DEBUG("Auto-created default value for member '" + memberName + "' of type " + memberType->getTypeName());
        } else {
            // 如果找不到成员类型，创建null值
            initialValue = new Null();
            LOG_DEBUG("Auto-created null value for member '" + memberName + "' (type not found)");
        }
    }
    
    memberInitialValues[memberName] = initialValue;
}

void ClassType::setAllMemberInitialValues(const map<string, Value*>& initialValues) {
    // 清除旧的初始值
    for (auto& pair : memberInitialValues) {
        delete pair.second;
    }
    memberInitialValues.clear();
    
    // 设置新的初始值
    for (const auto& pair : initialValues) {
        memberInitialValues[pair.first] = pair.second->clone();
    }
}

map<string, function<Value*(Value*, vector<Value*>&)>> ClassType::getMethods() const {
    map<string, function<Value*(Value*, vector<Value*>&)>> allMethods;
    
    // 添加从ObjectType基类继承的方法
    const map<FunctionSignature, function<Value*(Value*, vector<Value*>&)>>& baseMethods = ObjectType::getMethods();
    for (const auto& pair : baseMethods) {
        allMethods[pair.first.getName()] = pair.second;
    }
    
    return allMethods;
}

void ClassType::setDefaultConstructor(function<Value*(vector<Value*>&)> constructor) {
    defaultConstructor = constructor;
}

function<Value*(vector<Value*>&)> ClassType::getDefaultConstructor() const {
    return defaultConstructor;
}

bool ClassType::hasDefaultConstructor() const {
    return defaultConstructor != nullptr;
}

Value* ClassType::convertTo(ObjectType* targetType, Value* value) {
    // 用户定义类型的转换逻辑
    return nullptr;
}

bool ClassType::isCompatibleWith(ObjectType* other) const {
    return typeName == other->getTypeName();
}

Value* ClassType::createDefaultValue() {
    // 创建用户定义类型的默认值，使用存储的成员初始值
    Dict* instance = new Dict();
    
    // 使用存储的初始值初始化成员
    for (const auto& member : memberTypes) {
        const string& memberName = member.first;
        Value* initialValue = getMemberInitialValue(memberName);
        
        if (initialValue) {
            // 如果有初始值，使用初始值
            instance->setEntry(memberName, initialValue->clone());
        } else {
            // 如果没有初始值，使用成员类型的默认值
            ObjectType* memberType = member.second;
            if (memberType) {
                Value* memberDefault = memberType->createDefaultValue();
                if (memberDefault) {
                    instance->setEntry(memberName, memberDefault);
                }
            }
        }
    }
    
    return instance;
}

void ClassType::registerMemberAccessMethod(const string& memberName, ObjectType* memberType) {
    // 注册成员访问方法
    registerMethod("get_" + memberName, [this, memberName](Value* instance, vector<Value*>& args) -> Value* {
        if (instance && instance->getValueType() == this) {
            // 这里应该调用实例的成员访问方法
            // 暂时返回nullptr，实际实现需要更复杂的逻辑
            return nullptr;
        }
        return nullptr;
    });
    
    // 注册成员设置方法
    registerMethod("set_" + memberName, [this, memberName, memberType](Value* instance, vector<Value*>& args) -> Value* {
        if (instance && instance->getValueType() == this && args.size() >= 1) {
            // 这里应该调用实例的成员设置方法
            // 暂时返回nullptr，实际实现需要更复杂的逻辑
            return nullptr;
        }
        return nullptr;
    });
}

// 获取静态方法映射
const map<string, UserFunction*>& ClassType::getStaticMethods() const {
    return staticMethods;
}

// 获取静态成员类型映射
const map<string, ObjectType*>& ClassType::getStaticMemberTypes() const {
    return staticMemberTypes;
}

// 获取静态成员值映射
const map<string, Value*>& ClassType::getStaticMembers() const {
    return staticMemberValues;
}

// ==================== 缺失方法的实现 ====================

// 成员查询方法
ObjectType* ClassType::getMemberType(const string& name) const {
    auto it = memberTypes.find(name);
    if (it != memberTypes.end()) {
        return it->second;
    }
    return nullptr;
}

bool ClassType::hasMember(const string& name) const {
    return memberTypes.find(name) != memberTypes.end();
}

vector<string> ClassType::getMemberNames() const {
    vector<string> names;
    for (const auto& pair : memberTypes) {
        names.push_back(pair.first);
    }
    return names;
}

map<string, ObjectType*> ClassType::getMemberTypes() const {
    return memberTypes;
}

const map<string, UserFunction*>& ClassType::getUserFunctionMethods() const {
    return userFunctionMethods;
}

// 静态成员查询方法
Value* ClassType::getStaticMemberValue(const string& name) const {
    auto it = staticMemberValues.find(name);
    if (it != staticMemberValues.end()) {
        return it->second;
    }
    return nullptr;
}

void ClassType::setStaticMemberValue(const string& name, Value* value) {
    staticMemberValues[name] = value;
}

// 成员初始值查询方法
Value* ClassType::getMemberInitialValue(const string& memberName) const {
    auto it = memberInitialValues.find(memberName);
    if (it != memberInitialValues.end()) {
        return it->second;
    }
    return nullptr;
}

bool ClassType::hasMemberInitialValue(const string& memberName) const {
    return memberInitialValues.find(memberName) != memberInitialValues.end();
}

const map<string, Value*>& ClassType::getAllMemberInitialValues() const {
    return memberInitialValues;
}
