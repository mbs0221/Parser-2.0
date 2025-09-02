#include "interpreter/types/types.h"
#include "interpreter/values/value.h"
#include "interpreter/values/value.h"

using namespace std;

// ==================== PrimitiveType 方法实现 ====================

Function* PrimitiveType::findUserMethod(const FunctionSignature& signature) const {
    auto it = userFunctionMethods.find(signature);
    return it != userFunctionMethods.end() ? it->second : nullptr;
}

Function* PrimitiveType::findStaticMethod(const FunctionSignature& signature) const {
    auto it = staticMethods.find(signature);
    return it != staticMethods.end() ? it->second : nullptr;
}

Function* PrimitiveType::findUserMethod(const string& name, const vector<Value*>& args) const {
    auto it = methodNameToSignatures.find(name);
    if (it == methodNameToSignatures.end()) {
        return nullptr;
    }
    
    // 尝试匹配参数
    for (const auto& signature : it->second) {
        if (signature.matches(name, args)) {
            auto funcIt = userFunctionMethods.find(signature);
            if (funcIt != userFunctionMethods.end()) {
                return funcIt->second;
            }
        }
    }
    return nullptr;
}

Function* PrimitiveType::findStaticMethod(const string& name, const vector<Value*>& args) const {
    auto it = staticMethodNameToSignatures.find(name);
    if (it == staticMethodNameToSignatures.end()) {
        return nullptr;
    }
    
    // 尝试匹配参数
    for (const auto& signature : it->second) {
        if (signature.matches(name, args)) {
            auto funcIt = staticMethods.find(signature);
            if (funcIt != staticMethods.end()) {
                return funcIt->second;
            }
        }
    }
    return nullptr;
}

vector<FunctionSignature> PrimitiveType::getUserMethodOverloads(const string& name) const {
    auto it = methodNameToSignatures.find(name);
    return it != methodNameToSignatures.end() ? it->second : vector<FunctionSignature>();
}

vector<FunctionSignature> PrimitiveType::getStaticMethodOverloads(const string& name) const {
    auto it = staticMethodNameToSignatures.find(name);
    return it != staticMethodNameToSignatures.end() ? it->second : vector<FunctionSignature>();
}

bool PrimitiveType::hasUserMethod(const string& name) const {
    return methodNameToSignatures.find(name) != methodNameToSignatures.end();
}

bool PrimitiveType::hasStaticMethod(const string& name) const {
    return staticMethodNameToSignatures.find(name) != staticMethodNameToSignatures.end();
}

bool PrimitiveType::hasStaticMethodName(const string& name) const {
    return hasStaticMethod(name);
}

const map<FunctionSignature, Function*>& PrimitiveType::getUserFunctionMethods() const {
    return userFunctionMethods;
}

const map<FunctionSignature, Function*>& PrimitiveType::getStaticMethods() const {
    return staticMethods;
}

map<string, Function*> PrimitiveType::getUserFunctionMethodsByName() const {
    map<string, Function*> result;
    for (const auto& pair : userFunctionMethods) {
        result[pair.first.getName()] = pair.second;
    }
    return result;
}

map<string, Function*> PrimitiveType::getStaticMethodsByName() const {
    map<string, Function*> result;
    for (const auto& pair : staticMethods) {
        result[pair.first.getName()] = pair.second;
    }
    return result;
}
