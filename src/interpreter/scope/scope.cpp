#include "interpreter/scope/scope.h"
#include "interpreter/values/value.h"
#include "interpreter/types/types.h"
#include <iostream>
#include <memory>

using namespace std;

// ==================== Scope实现 ====================
// Scope的实现现在在头文件中，因为使用了unique_ptr

// 类型管理方法已移除 - 类型管理由TypeRegistry统一负责
// createTypeFromString函数已移动到TypeRegistry中

// Scope构造函数
Scope::Scope() {
    // 创建类型管理器和对象管理器
    typeRegistry = std::make_unique<TypeRegistry>();
    objectRegistry = std::make_unique<ObjectRegistry>();
}

// Scope析构函数
Scope::~Scope() {

}



// 打印作用域内容（用于调试）
void Scope::print() const {
    cout << "=== Scope Content ===" << endl;
    
    if (typeRegistry) {
        cout << "Type Registry: " << typeRegistry->getTypeCount() << " types" << endl;
    }
    
    if (objectRegistry) {
        cout << "Object Registry: " << endl;
        // 这里可以添加更多对象信息的打印
    }
    

}

// 获取类型管理器
TypeRegistry* Scope::getTypeRegistry() const {
    return typeRegistry.get();
}

// 获取对象管理器
ObjectRegistry* Scope::getObjectRegistry() const {
    return objectRegistry.get();
}

// 获取变量值（基础版本）
Value* Scope::getVariable(const std::string& name) const {
    if (objectRegistry) {
        return objectRegistry->lookupVariable(name);
    }
    return nullptr;
}

// 设置变量值到当前作用域
void Scope::setVariable(const std::string& name, Value* value) {
    if (objectRegistry) {
        objectRegistry->defineVariable(name, value);
    }
}

// 设置函数参数到当前作用域（统一绑定方式）
void Scope::setArgument(const std::string& name, Value* value) {
    // 直接使用ObjectRegistry存储参数
    setVariable(name, value);
}

// ==================== 位置参数访问实现 ====================

// 按索引获取参数（基于ObjectRegistry的顺序）
Value* Scope::getArgumentByIndex(size_t index) const {
    if (!objectRegistry) return nullptr;
    
    // 从ObjectRegistry获取所有变量，按定义顺序
    std::vector<Value*> allVars;
    for (const auto& name : objectRegistry->getVariableNames()) {
        if (Value* var = objectRegistry->lookupVariable(name)) {
            allVars.push_back(var);
        }
    }
    if (index < allVars.size()) {
        return allVars[index];
    }
    return nullptr;
}

// 获取所有参数（按定义顺序）
std::vector<Value*> Scope::getAllArguments() const {
    if (!objectRegistry) return std::vector<Value*>();
    std::vector<Value*> allVars;
    for (const auto& name : objectRegistry->getVariableNames()) {
        if (Value* var = objectRegistry->lookupVariable(name)) {
            allVars.push_back(var);
        }
    }
    return allVars;
}

// 获取参数数量
size_t Scope::getArgumentCount() const {
    if (!objectRegistry) return 0;
    
    // 只统计真正的函数参数，过滤掉系统变量
    size_t count = 0;
    std::vector<std::string> allNames = objectRegistry->getVariableNames();
    
    for (const std::string& name : allNames) {
        // 过滤掉系统参数和特殊变量
        if (name != "argc" && name != "args" && name != "this" && 
            name != "instance" && name != "kwargs" && name != "__class_name__" && 
            name != "self" && name != "__func__") {
            count++;
        }
    }
    
    return count;
}

// 获取参数名称列表（用于函数原型匹配）
std::vector<std::string> Scope::getParameterNames() const {
    if (!objectRegistry) return std::vector<std::string>();
    
    // 从ObjectRegistry获取所有变量名称，按定义顺序
    std::vector<std::string> allNames = objectRegistry->getVariableNames();
    
    // 过滤掉特殊的系统参数（如argc, args, this, instance, kwargs）
    // 注意：args现在用于*args位置参数，kwargs用于**kwargs关键字参数
    std::vector<std::string> paramNames;
    for (const std::string& name : allNames) {
        if (name != "argc" && name != "args" && name != "this" && name != "instance" && name != "kwargs" && name != "__class_name__" && name != "self" && name != "__func__") {
            paramNames.push_back(name);
        }
    }
    
    return paramNames;
}

// 获取有名字的函数实际参数（过滤系统变量）
std::map<std::string, Value*> Scope::getNamedArgs() const {
    std::map<std::string, Value*> namedArgs;
    if (!objectRegistry) return namedArgs;
    
    // 从ObjectRegistry获取所有变量名称
    std::vector<std::string> allNames = objectRegistry->getVariableNames();
    
    // 过滤掉系统参数，只保留真正的函数参数
    for (const std::string& name : allNames) {
        if (name != "argc" && name != "args" && name != "this" && name != "instance" && name != "kwargs" && name != "__class_name__" && name != "self" && name != "__func__") {
            Value* value = objectRegistry->lookupVariable(name);
            if (value) {
                namedArgs[name] = value;
            }
        }
    }
    
    return namedArgs;
}

// 获取kwargs字典（可变参数支持）
Dict* Scope::getKwargs() const {
    return getVariable<Dict>("kwargs");
}

// 检查是否有kwargs
bool Scope::hasKwargs() const {
    return getVariable("kwargs") != nullptr;
}

// 获取args数组（位置可变参数支持）
Array* Scope::getArgs() const {
    return getVariable<Array>("args");
}

// 检查是否有args
bool Scope::hasArgs() const {
    return getVariable("args") != nullptr;
}

// ==================== 可变参数支持实现 ====================

// 注意：旧的setVarArgs机制已被新的*args和**kwargs机制替代
// 保留hasVarArgs作为兼容性方法，检查是否有任何形式的可变参数
bool Scope::hasVarArgs() const {
    return hasArgs() || hasKwargs();
}


