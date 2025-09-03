#include "interpreter/values/value.h"
#include "interpreter/types/types.h"
#include "interpreter/scope/scope.h"
#include <algorithm>
#include <iostream> // Added for debugging
#include <sstream>

using namespace std;

// ==================== MethodReference 方法实现 ====================

MethodReference::MethodReference(ObjectType* type, const string& name)
    : Function(name, {}), targetType(type), methodName(name), 
      cachedFunction(nullptr) {
}

ObjectType* MethodReference::getTargetType() const {
    return targetType;
}

string MethodReference::getMethodName() const {
    return methodName;
}

string MethodReference::getFunctionType() const {
    return "method_reference"; // 基类无法确定是否为静态方法
}

string MethodReference::getTypeName() const {
    return "MethodReference";
}

Value* MethodReference::clone() const {
    // 基类无法克隆，需要派生类实现
    return nullptr;
}

string MethodReference::toString() const {
    stringstream ss;
    ss << "method<" << (targetType ? targetType->getTypeName() : "unknown") << "::" << methodName << ">";
    return ss.str();
}

// 父类实现统一的调用逻辑，子类负责查找匹配方法
Value* MethodReference::call(Scope* scope) {
    if (cachedFunction) {
        return cachedFunction->call(scope);
    }
    
    // 计算函数签名 - 使用FunctionSignature的构造函数
    FunctionSignature callSignature(methodName, scope);
    
    // 调用子类的findBestMatch方法查找匹配函数
    Function* foundFunction = findBestMatch(callSignature);
    
    // 如果找到了函数，直接设置缓存并调用
    if (foundFunction) {
        LOG_DEBUG("MethodReference::call: Found function");
        cachedFunction = foundFunction;
        return foundFunction->call(scope);
    }
    
    LOG_DEBUG("MethodReference::call: No function found");
    return nullptr;
}

// 实现Function基类的纯虚函数
FunctionSignature MethodReference::getSignature() const {
    // 创建一个通用的方法引用签名
    // 由于方法引用的具体签名取决于调用时的参数，这里返回一个基础签名
    vector<Parameter> params;
    if (targetType) {
        // 尝试获取方法的默认签名信息
        if (targetType->supportsMethods()) {
            IMethodSupport* methodSupport = dynamic_cast<IMethodSupport*>(targetType);
            if (methodSupport) {
                // 获取方法的默认签名（如果有的话）
                vector<FunctionSignature> overloads = methodSupport->getUserMethodOverloads(methodName);
                if (!overloads.empty()) {
                    return overloads[0];  // 返回第一个重载的签名
                }
            }
        }
    }
    
    // 如果没有找到具体签名，返回一个通用的签名
    return FunctionSignature(methodName, params);
}

// ==================== InstanceMethodReference 实现 ====================

InstanceMethodReference::InstanceMethodReference(ObjectType* type, Value* instance, const string& name)
    : MethodReference(type, name), targetInstance(instance) {
}

Value* InstanceMethodReference::getTargetInstance() const {
    return targetInstance;
}

bool InstanceMethodReference::isStaticMethod() const {
    return false;
}

Value* InstanceMethodReference::clone() const {
    return new InstanceMethodReference(targetType, targetInstance, methodName);
}

string InstanceMethodReference::toString() const {
    stringstream ss;
    ss << "instance_method<" << (targetType ? targetType->getTypeName() : "unknown") << "::" << methodName << ">";
    return ss.str();
}

// 子类实现：根据函数签名查找最佳匹配的实例方法
Function* InstanceMethodReference::findBestMatch(const FunctionSignature& callSignature) const {
    if (!targetType || !targetType->supportsMethods()) {
        LOG_ERROR("InstanceMethodReference::findBestMatch: Target type or method support is null");
        return nullptr;
    }
    
    // 直接根据调用签名查找匹配的实例方法
    IMethodSupport* methodSupport = dynamic_cast<IMethodSupport*>(targetType);
    if (methodSupport) {
        LOG_DEBUG("InstanceMethodReference::findBestMatch: Method support is not null");
        return methodSupport->findUserMethod(callSignature);
    }
    
    LOG_ERROR("InstanceMethodReference::findBestMatch: Method support is null");
    return nullptr;
}

// ==================== StaticMethodReference 实现 ====================

StaticMethodReference::StaticMethodReference(ObjectType* type, const string& name)
    : MethodReference(type, name) {
}

bool StaticMethodReference::isStaticMethod() const {
    return true;
}

Value* StaticMethodReference::clone() const {
    return new StaticMethodReference(targetType, methodName);
}

string StaticMethodReference::toString() const {
    stringstream ss;
    ss << "static_method<" << (targetType ? targetType->getTypeName() : "unknown") << "::" << methodName << ">";
    return ss.str();
}

// 子类实现：根据函数签名查找最佳匹配的静态方法
Function* StaticMethodReference::findBestMatch(const FunctionSignature& callSignature) const {
    if (!targetType || !targetType->supportsMethods()) {
        LOG_ERROR("StaticMethodReference::findBestMatch: Target type or method support is null");
        return nullptr;
    }
    
    // 直接根据调用签名查找匹配的静态方法
    IMethodSupport* methodSupport = dynamic_cast<IMethodSupport*>(targetType);
    if (methodSupport) {
        LOG_DEBUG("StaticMethodReference::findBestMatch: Method support is not null");
        LOG_DEBUG(callSignature.toString());
        return methodSupport->findStaticMethod(callSignature);
    }
    
    LOG_ERROR("StaticMethodReference::findBestMatch: Method support is null");
    return nullptr;
}
