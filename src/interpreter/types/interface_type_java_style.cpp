#include "interpreter/types/types.h"
#include "interpreter/core/interpreter.h"
#include "interpreter/values/value.h"
#include "interpreter/values/primitive_types.h"

using namespace std;

// ==================== InterfaceType Java风格实现 ====================

InterfaceType::InterfaceType(const string& name) 
    : ObjectType(name, false, false, false, false, false) {
    LOG_DEBUG("创建接口类型: " + name);
}

void InterfaceType::addExtendedInterface(const string& interfaceName) {
    extendedInterfaces.push_back(interfaceName);
    LOG_DEBUG("接口 '" + typeName + "' 继承接口: " + interfaceName);
}

const vector<string>& InterfaceType::getExtendedInterfaces() const {
    return extendedInterfaces;
}

void InterfaceType::addMethodSignature(const string& methodName, FunctionPrototype* methodSignature) {
    if (!methodSignature) {
        LOG_ERROR("尝试添加空的方法签名到接口: " + typeName);
        return;
    }
    
    methodSignatures[methodName] = methodSignature;
    LOG_DEBUG("接口 '" + typeName + "' 添加方法签名: " + methodName);
}

const map<string, FunctionPrototype*>& InterfaceType::getMethodSignatures() const {
    return methodSignatures;
}

bool InterfaceType::hasMethodSignature(const string& methodName) const {
    return methodSignatures.find(methodName) != methodSignatures.end();
}

FunctionPrototype* InterfaceType::getMethodSignature(const string& methodName) const {
    auto it = methodSignatures.find(methodName);
    return (it != methodSignatures.end()) ? it->second : nullptr;
}

bool InterfaceType::isImplementedBy(ClassType* classType) const {
    if (!classType) {
        return false;
    }
    
    LOG_DEBUG("检查类 '" + classType->getTypeName() + "' 是否实现了接口 '" + typeName + "'");
    
    // 获取接口要求的所有方法签名（包括继承的接口）
    vector<FunctionPrototype*> requiredMethods = getAllRequiredMethodSignatures();
    
    // 检查类是否实现了所有必需的方法
    for (FunctionPrototype* requiredMethod : requiredMethods) {
        if (!requiredMethod) continue;
        
        string methodName = requiredMethod->name;
        
        // 检查类是否有这个方法
        if (!classType->hasUserMethod(methodName)) {
            LOG_DEBUG("类 '" + classType->getTypeName() + "' 缺少接口 '" + typeName + "' 要求的方法: " + methodName);
            return false;
        }
        
        // 这里可以进一步检查方法签名是否匹配
        // 暂时只检查方法名是否存在
    }
    
    LOG_DEBUG("类 '" + classType->getTypeName() + "' 成功实现了接口 '" + typeName + "'");
    return true;
}

vector<FunctionPrototype*> InterfaceType::getAllRequiredMethodSignatures() const {
    vector<FunctionPrototype*> allMethods;
    
    // 添加当前接口的方法
    for (const auto& methodPair : methodSignatures) {
        allMethods.push_back(methodPair.second);
    }
    
    // 添加继承接口的方法（递归）
    // 注意：这里需要访问TypeRegistry来查找继承的接口
    // 暂时简化实现，只返回当前接口的方法
    // TODO: 实现递归查找继承接口的方法
    
    return allMethods;
}

Value* InterfaceType::createDefaultValue() {
    // 接口类型不能创建实例（Java风格）
    LOG_ERROR("接口类型 '" + typeName + "' 不能创建实例");
    return nullptr;
}

bool InterfaceType::isCompatibleWith(ObjectType* other) const {
    if (!other) return false;
    
    // 接口类型只能与实现了该接口的类类型兼容
    if (ClassType* classType = dynamic_cast<ClassType*>(other)) {
        return isImplementedBy(classType);
    }
    
    // 接口类型与自身兼容
    if (InterfaceType* interfaceType = dynamic_cast<InterfaceType*>(other)) {
        return typeName == interfaceType->getTypeName();
    }
    
    return false;
}
