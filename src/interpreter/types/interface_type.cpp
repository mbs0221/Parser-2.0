#include "interpreter/types/types.h"

using namespace std;

// ==================== Interface实现 ====================
Interface::Interface(const string& name) 
    : ObjectType(name, false, false, false, false, false) {
}

void Interface::addMethodPrototype(const string& name, function<Value*(Value*, vector<Value*>&)> prototype) {
    // 创建函数签名
    FunctionSignature signature(name, vector<Parameter>());
    addMethodPrototype(signature, prototype);
}

function<Value*(Value*, vector<Value*>&)> Interface::getMethodPrototype(const string& name) const {
    auto it = methodNameToSignatures.find(name);
    if (it == methodNameToSignatures.end() || it->second.empty()) {
        return nullptr;
    }
    
    // 返回第一个匹配的方法原型
    return getMethodPrototype(it->second[0]);
}

bool Interface::hasMethodPrototype(const string& name) const {
    return methodNameToSignatures.find(name) != methodNameToSignatures.end();
}

vector<string> Interface::getMethodPrototypeNames() const {
    vector<string> names;
    for (const auto& pair : methodNameToSignatures) {
        names.push_back(pair.first);
    }
    return names;
}

bool Interface::isCompatibleWith(ObjectType* other) const {
    return typeName == other->getTypeName();
}

Value* Interface::createDefaultValue() {
    // 接口类型不能创建实例
    return nullptr;
}

// ==================== 函数签名支持的新方法实现 ====================

void Interface::addUserMethod(Function* func, VisibilityType visibility) {
    if (!func) return;
    
    // 接口只存储方法原型，不存储实际实现
    FunctionSignature signature = func->getSignature();
    
    // 创建默认实现（接口方法没有实际实现）
    auto defaultImpl = [](Value* instance, vector<Value*>& args) -> Value* {
        // 接口方法没有实现，返回nullptr
        return nullptr;
    };
    
    // 存储到方法原型映射
    methodPrototypes[signature] = defaultImpl;
    
    // 更新名称到签名的映射
    string methodName = signature.getName();
    methodNameToSignatures[methodName].push_back(signature);
}

void Interface::addStaticMethod(Function* func, VisibilityType visibility) {
    if (!func) return;
    
    // 接口只存储方法原型，不存储实际实现
    FunctionSignature signature = func->getSignature();
    
    // 创建默认实现（接口方法没有实际实现）
    auto defaultImpl = [](Value* instance, vector<Value*>& args) -> Value* {
        // 接口方法没有实现，返回nullptr
        return nullptr;
    };
    
    // 存储到方法原型映射
    methodPrototypes[signature] = defaultImpl;
    
    // 更新名称到签名的映射（接口中静态方法和实例方法都存储在同一个映射中）
    string methodName = signature.getName();
    methodNameToSignatures[methodName].push_back(signature);
}

void Interface::addMethodPrototype(const FunctionSignature& signature, function<Value*(Value*, vector<Value*>&)> prototype) {
    // 存储到函数签名映射
    methodPrototypes[signature] = prototype;
    
    // 更新名称到签名的映射
    string methodName = signature.getName();
    methodNameToSignatures[methodName].push_back(signature);
    
    // 注释掉 registerMethod 调用，因为该方法不存在
    // registerMethod(methodName, prototype);
}

void Interface::addMethodPrototype(const FunctionPrototype* prototype) {
    if (prototype) {
        // 从FunctionPrototype创建FunctionSignature
        FunctionSignature signature(prototype);
        
        // 创建默认实现（接口方法没有实际实现）
        auto defaultImpl = [](Value* instance, vector<Value*>& args) -> Value* {
            // 接口方法没有实现，返回nullptr
            return nullptr;
        };
        
        addMethodPrototype(signature, defaultImpl);
    }
}

function<Value*(Value*, vector<Value*>&)> Interface::getMethodPrototype(const FunctionSignature& signature) const {
    auto it = methodPrototypes.find(signature);
    return (it != methodPrototypes.end()) ? it->second : nullptr;
}

bool Interface::hasMethodPrototype(const FunctionSignature& signature) const {
    return methodPrototypes.find(signature) != methodPrototypes.end();
}

function<Value*(Value*, vector<Value*>&)> Interface::findMethodPrototype(const FunctionSignature& signature) const {
    return getMethodPrototype(signature);
}

function<Value*(Value*, vector<Value*>&)> Interface::findMethodPrototype(const string& name, const vector<Value*>& args) const {
    auto it = methodNameToSignatures.find(name);
    if (it == methodNameToSignatures.end()) {
        return nullptr;
    }
    
    // 尝试匹配参数
    for (const FunctionSignature& signature : it->second) {
        if (signature.matches(name, args)) {
            return getMethodPrototype(signature);
        }
    }
    return nullptr;
}

vector<FunctionSignature> Interface::getMethodPrototypeOverloads(const string& name) const {
    auto it = methodNameToSignatures.find(name);
    return (it != methodNameToSignatures.end()) ? it->second : vector<FunctionSignature>();
}

// ==================== IMethodSupport 接口的其他方法实现 ====================

Function* Interface::findUserMethod(const FunctionSignature& signature) const {
    // 接口没有实际的用户方法实现
    return nullptr;
}

Function* Interface::findStaticMethod(const FunctionSignature& signature) const {
    // 接口没有实际的静态方法实现
    return nullptr;
}

Function* Interface::findUserMethod(const string& name, const vector<Value*>& args) const {
    // 接口没有实际的用户方法实现
    return nullptr;
}

Function* Interface::findStaticMethod(const string& name, const vector<Value*>& args) const {
    // 接口没有实际的静态方法实现
    return nullptr;
}

vector<FunctionSignature> Interface::getUserMethodOverloads(const string& name) const {
    // 接口没有实际的用户方法实现
    return vector<FunctionSignature>();
}

vector<FunctionSignature> Interface::getStaticMethodOverloads(const string& name) const {
    // 接口中静态方法和实例方法都存储在同一个映射中
    auto it = methodNameToSignatures.find(name);
    return (it != methodNameToSignatures.end()) ? it->second : vector<FunctionSignature>();
}

bool Interface::hasUserMethod(const string& name) const {
    // 接口没有实际的用户方法实现
    return false;
}

bool Interface::hasStaticMethod(const string& name) const {
    // 接口中静态方法和实例方法都存储在同一个映射中
    auto it = methodNameToSignatures.find(name);
    return it != methodNameToSignatures.end() && !it->second.empty();
}

bool Interface::hasStaticMethodName(const string& name) const {
    return hasStaticMethod(name);
}

const map<FunctionSignature, Function*>& Interface::getUserFunctionMethods() const {
    // 接口没有实际的用户方法实现，返回空映射
    static const map<FunctionSignature, Function*> emptyMap;
    return emptyMap;
}

const map<FunctionSignature, Function*>& Interface::getStaticMethods() const {
    // 接口没有实际的静态方法实现，返回空映射
    static const map<FunctionSignature, Function*> emptyMap;
    return emptyMap;
}

map<string, Function*> Interface::getUserFunctionMethodsByName() const {
    // 接口没有实际的用户方法实现，返回空映射
    return map<string, Function*>();
}

map<string, Function*> Interface::getStaticMethodsByName() const {
    // 接口没有实际的静态方法实现，返回空映射
    return map<string, Function*>();
}
