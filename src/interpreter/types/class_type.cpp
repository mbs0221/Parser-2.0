#include "interpreter/types/types.h"
#include "interpreter/values/value.h"
#include "interpreter/scope/scope.h"

#include "common/logger.h"
#include <algorithm>

using namespace std;

// ==================== ClassType实现 ====================
ClassType::ClassType(const string& name, bool isPrimitive, bool isMutable, bool isReference)
    : ObjectType(name, isPrimitive, isMutable, isReference, false, true) {
    // 类类型自动注册成员访问方法
    registerMemberAccess();
}

void ClassType::addMember(const string& name, ObjectType* type, VisibilityType visibility) {
    memberTypes[name] = type;
    memberVisibility[name] = visibility;
    
    // 注册成员访问方法
    registerMemberGetter(name, type);
    registerMemberSetter(name, type);
}

void ClassType::addStaticMember(const string& name, ObjectType* type, Value* value, VisibilityType visibility) {
    staticMemberTypes[name] = type;
    staticMemberValues[name] = value;
    staticMemberVisibility[name] = visibility;
}

void ClassType::addUserMethod(Function* func, VisibilityType visibility) {
    if (!func) return;
    
    // 从Function获取函数签名
    FunctionSignature signature = func->getSignature();
    
    // 直接存储到函数签名映射，支持所有Function类型
    userFunctionMethods[signature] = func;
    
    // 更新名称到签名的映射
    string methodName = signature.getName();
    methodNameToSignatures[methodName].push_back(signature);
    LOG_DEBUG("ClassType::ClassType: adding class method: " + signature.toString());

    // 设置可见性
    if (memberVisibility.find(methodName) == memberVisibility.end()) {
        memberVisibility[methodName] = visibility;
    }
}

void ClassType::addStaticMethod(Function* func, VisibilityType visibility) {
    if (!func) return;
    
    // 从Function获取函数签名
    FunctionSignature signature = func->getSignature();
    // 直接存储到函数签名映射
    staticMethods[signature] = dynamic_cast<UserFunction*>(func);
    
    // 更新名称到签名的映射
    string methodName = signature.getName();
    staticMethodNameToSignatures[methodName].push_back(signature);
    LOG_DEBUG("ClassType::ClassType: adding class method: " + signature.toString());

    // 设置可见性
    if (staticMemberVisibility.find(methodName) == staticMemberVisibility.end()) {
        staticMemberVisibility[methodName] = visibility;
    }
}

// ==================== Trait 支持实现 ====================
// 暂时注释掉，专注于基础功能
/*
void ClassType::addTrait(ITrait* trait) {
    if (trait && !hasTrait(trait->getName())) {
        traits.push_back(trait);
    }
}

void ClassType::removeTrait(const string& traitName) {
    traits.erase(
        remove_if(traits.begin(), traits.end(),
            [&traitName](ITrait* trait) { return trait->getName() == traitName; }),
        traits.end()
    );
}

bool ClassType::hasTrait(const string& traitName) const {
    return any_of(traits.begin(), traits.end(),
        [&traitName](ITrait* trait) { return trait->getName() == traitName; });
}

ITrait* ClassType::getTrait(const string& traitName) const {
    auto it = find_if(traits.begin(), traits.end(),
        [&traitName](ITrait* trait) { return trait->getName() == traitName; });
    return (it != traits.end()) ? *it : nullptr;
}

const vector<ITrait*>& ClassType::getTraits() const {
    return traits;
}

bool ClassType::supportsStringOperations() const {
    return hasTrait("StringOperations");
}

bool ClassType::supportsArrayOperations() const {
    return hasTrait("ArrayOperations");
}

bool ClassType::supportsDictOperations() const {
    return hasTrait("DictOperations");
}
*/

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
    
    // 返回空的方法映射，因为ClassType使用新的函数签名系统
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

void ClassType::registerMemberGetter(const string& memberName, ObjectType* memberType) {
    // 注册成员访问方法 - 使用addUserMethod
    auto getMethod = [this, memberName](Scope* scope) -> Value* {
        // 从scope中获取this参数
        Value* instance = scope->getVariable<Value>("this");
        if (instance && instance->getValueType() == this) {
            // 获取实例的成员值
            if (ObjectValue* objValue = dynamic_cast<ObjectValue*>(instance)) {
                return objValue->getProperty(memberName);
            }
        }
        return nullptr;
    };
    
    // 使用函数原型构造函数创建BuiltinFunction对象
    string prototype = "get_" + memberName + "()";
    BuiltinFunction* getFunc = new BuiltinFunction(getMethod, prototype.c_str());
    addUserMethod(getFunc, VIS_PUBLIC);
}

void ClassType::registerMemberSetter(const string& memberName, ObjectType* memberType) {
    // 注册成员设置方法
    auto setMethod = [this, memberName, memberType](Scope* scope) -> Value* {
        // 从scope中获取this和value参数
        Value* instance = scope->getVariable<Value>("this");
        Value* value = scope->getVariable<Value>("value");
        if (instance && instance->getValueType() == this && value) {
            // 设置实例的成员值
            if (ObjectValue* objValue = dynamic_cast<ObjectValue*>(instance)) {
                objValue->setProperty(memberName, value);
                return value; // 返回设置的值
            }
        }
        return nullptr;
    };
    
    // 使用函数原型构造函数创建BuiltinFunction对象
    string memberTypeName = memberType ? memberType->getTypeName() : "any";
    string prototype = "set_" + memberName + "(value:" + memberTypeName + ")";
    BuiltinFunction* setFunc = new BuiltinFunction(setMethod, prototype.c_str());
    addUserMethod(setFunc, VIS_PUBLIC);
}

// 获取静态方法映射
const map<FunctionSignature, Function*>& ClassType::getStaticMethods() const {
    return staticMethods;
}

// 重写hasStaticMethodName方法，检查ClassType特有的静态方法存储
bool ClassType::hasStaticMethodName(const string& methodName) const {
    // 使用新的函数签名系统检查
    return hasStaticMethod(methodName);
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

const map<FunctionSignature, Function*>& ClassType::getUserFunctionMethods() const {
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

bool ClassType::hasStaticMember(const string& name) const {
    return staticMemberValues.find(name) != staticMemberValues.end();
}

Value* ClassType::accessStaticMember(const string& name) const {
    auto it = staticMemberValues.find(name);
    if (it != staticMemberValues.end()) {
        return it->second;
    }
    return nullptr;
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

// ==================== 函数签名支持的新方法实现 ====================
// 这些方法已经在上面的 addUserMethod(Function*) 和 addStaticMethod(Function*) 中实现

Function* ClassType::findUserMethod(const FunctionSignature& signature) const {
    LOG_DEBUG("ClassType::findUserMethod: Looking for signature: " + signature.toString());
    LOG_DEBUG("ClassType::findUserMethod: Available methods count: " + to_string(userFunctionMethods.size()));
    
    // 打印所有可用的方法签名
    for (const auto& pair : userFunctionMethods) {
        LOG_DEBUG("ClassType::findUserMethod: Available signature: " + pair.first.toString());
    }
    
    // 首先尝试精确匹配
    auto it = userFunctionMethods.find(signature);
    if (it != userFunctionMethods.end()) {
        LOG_DEBUG("ClassType::findUserMethod: Found exact match!");
        return it->second;
    }
    
    // 如果精确匹配失败，且签名为空（只有方法名），则基于方法名查找
    if (signature.getParameterCount() == 0) {
        string methodName = signature.getName();
        LOG_DEBUG("ClassType::findUserMethod: Trying name-based lookup for: " + methodName);
        
        auto nameIt = methodNameToSignatures.find(methodName);
        if (nameIt != methodNameToSignatures.end() && !nameIt->second.empty()) {
            // 返回第一个匹配的方法（通常是无参数方法）
            FunctionSignature firstSignature = nameIt->second[0];
            auto funcIt = userFunctionMethods.find(firstSignature);
            if (funcIt != userFunctionMethods.end()) {
                LOG_DEBUG("ClassType::findUserMethod: Found name-based match: " + firstSignature.toString());
                return funcIt->second;
            }
        }
    }
    
    LOG_DEBUG("ClassType::findUserMethod: No match found");
    return nullptr;
}

Function* ClassType::findStaticMethod(const FunctionSignature& signature) const {
    auto it = staticMethods.find(signature);
    return (it != staticMethods.end()) ? it->second : nullptr;
}

Function* ClassType::findUserMethod(const string& name, const vector<Value*>& args) const {
    auto it = methodNameToSignatures.find(name);
    if (it == methodNameToSignatures.end()) {
        return nullptr;
    }
    
    // 使用匹配分数系统找到最佳匹配
    Function* bestMatch = nullptr;
    int bestScore = -1;
    
    for (const FunctionSignature& signature : it->second) {
        int score = signature.calculateMatchScore(args);
        if (score > bestScore) {
            bestScore = score;
            bestMatch = findUserMethod(signature);
        }
    }
    
    return bestMatch;
}

Function* ClassType::findStaticMethod(const string& name, const vector<Value*>& args) const {
    auto it = staticMethodNameToSignatures.find(name);
    if (it == staticMethodNameToSignatures.end()) {
        return nullptr;
    }
    
    // 使用匹配分数系统找到最佳匹配
    Function* bestMatch = nullptr;
    int bestScore = -1;
    
    for (const FunctionSignature& signature : it->second) {
        int score = signature.calculateMatchScore(args);
        if (score > bestScore) {
            bestScore = score;
            bestMatch = findStaticMethod(signature);
        }
    }
    
    return bestMatch;
}

vector<FunctionSignature> ClassType::getUserMethodOverloads(const string& name) const {
    auto it = methodNameToSignatures.find(name);
    return (it != methodNameToSignatures.end()) ? it->second : vector<FunctionSignature>();
}

vector<FunctionSignature> ClassType::getStaticMethodOverloads(const string& name) const {
    auto it = staticMethodNameToSignatures.find(name);
    return (it != staticMethodNameToSignatures.end()) ? it->second : vector<FunctionSignature>();
}

bool ClassType::hasUserMethod(const string& name) const {
    return methodNameToSignatures.find(name) != methodNameToSignatures.end();
}

bool ClassType::hasStaticMethod(const string& name) const {
    return staticMethodNameToSignatures.find(name) != staticMethodNameToSignatures.end();
}

map<string, Function*> ClassType::getUserFunctionMethodsByName() const {
    map<string, Function*> result;
    for (const auto& pair : userFunctionMethods) {
        result[pair.first.getName()] = pair.second;
    }
    return result;
}

map<string, Function*> ClassType::getStaticMethodsByName() const {
    map<string, Function*> result;
    for (const auto& pair : staticMethods) {
        result[pair.first.getName()] = pair.second;
    }
    return result;
}

// ==================== 缺失方法的实现 ====================

// 重写 ObjectType 的 registerMemberAccess 方法
void ClassType::registerMemberAccess() {
    // 为所有成员注册访问方法
    for (const auto& member : memberTypes) {
        registerMemberGetter(member.first, member.second);
        registerMemberSetter(member.first, member.second);
    }
}

void ClassType::registerSubscriptAccess() {
    // 注册下标访问方法（如果需要的话）
    // 这个方法可以根据需要实现
}

Value* ClassType::createValueWithArgs(const vector<Value*>& args) {
    // 创建带参数的值的默认实现
    // 子类可以重写这个方法
    return createDefaultValue();
}

// 成员可见性管理
VisibilityType ClassType::getMemberVisibility(const string& name) const {
    auto it = memberVisibility.find(name);
    if (it != memberVisibility.end()) {
        return it->second;
    }
    return VIS_PUBLIC; // 默认返回公有
}

void ClassType::setMemberVisibility(const string& name, VisibilityType visibility) {
    memberVisibility[name] = visibility;
}

