// #include "interpreter/types/function_signature.h"
#include "interpreter/values/value.h"
#include "interpreter/types/types.h"
#include <sstream>

using namespace std;

// ==================== FunctionSignature构造函数实现 ====================
FunctionSignature::FunctionSignature(const string& n, const vector<string>& types) 
    : name(n), parameterTypes(types) {
}

FunctionSignature::FunctionSignature(const FunctionPrototype* prototype) {
    if (prototype) {
        name = prototype->name;
        // 从原型中提取参数类型名称
        parameterTypes = prototype->getParameterTypeNames();
    }
}

FunctionSignature::FunctionSignature(const FunctionDefinition* funcDef) {
    if (funcDef) {
        name = funcDef->prototype ? funcDef->prototype->name : "";
        // 从原型中提取参数类型名称
        if (funcDef->prototype) {
            parameterTypes = funcDef->prototype->getParameterTypeNames();
        }
    }
}

// 比较函数签名是否匹配
bool FunctionSignature::matches(const string& methodName, const vector<Value*>& args) const {
    if (name != methodName) return false;
    if (parameterTypes.size() != args.size()) return false;
    
    // 检查参数类型匹配
    for (size_t i = 0; i < parameterTypes.size(); ++i) {
        if (i >= args.size()) return false;
        if (!args[i]) return false;
        
        ObjectType* argType = args[i]->getValueType();
        if (!argType) return false;
        
        // 检查类型是否匹配（支持类型兼容性）
        if (parameterTypes[i] != "any" && 
            parameterTypes[i] != argType->getTypeName() &&
            !argType->isCompatibleWith(getTypeByName(parameterTypes[i]))) {
            return false;
        }
    }
    return true;
}

// 辅助函数：根据类型名称获取类型对象
ObjectType* FunctionSignature::getTypeByName(const string& typeName) const {
    // 通过TypeRegistry获取类型
    TypeRegistry* registry = TypeRegistry::getGlobalInstance();
    if (registry) {
        return registry->getType(typeName);
    }
    return nullptr;
}

// 拷贝构造函数
FunctionSignature::FunctionSignature(const FunctionSignature& other) 
    : name(other.name), parameterTypes(other.parameterTypes) {}

// 赋值运算符
FunctionSignature& FunctionSignature::operator=(const FunctionSignature& other) {
    if (this != &other) {
        name = other.name;
        parameterTypes = other.parameterTypes;
    }
    return *this;
}

// 获取函数名
string FunctionSignature::getName() const { 
    return name; 
}

// 获取参数类型列表
const vector<string>& FunctionSignature::getParameterTypes() const { 
    return parameterTypes; 
}

// 获取参数数量
size_t FunctionSignature::getParameterCount() const { 
    return parameterTypes.size(); 
}



// 检查是否与另一个签名匹配（用于重载解析）
bool FunctionSignature::matches(const FunctionSignature& other) const {
    if (name != other.name) return false;
    if (parameterTypes.size() != other.parameterTypes.size()) return false;
    
    for (size_t i = 0; i < parameterTypes.size(); ++i) {
        if (parameterTypes[i] != other.parameterTypes[i]) return false;
    }
    return true;
}

// 获取签名字符串表示
string FunctionSignature::toString() const {
    stringstream ss;
    ss << name << "(";
    for (size_t i = 0; i < parameterTypes.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << parameterTypes[i];
    }
    ss << ")";
    return ss.str();
}

// 比较两个签名是否相等
bool FunctionSignature::operator==(const FunctionSignature& other) const {
    if (name != other.name) return false;
    if (parameterTypes.size() != other.parameterTypes.size()) return false;
    for (size_t i = 0; i < parameterTypes.size(); ++i) {
        if (parameterTypes[i] != other.parameterTypes[i]) return false;
    }
    return true;
}

// 比较两个签名的大小（用于排序）
bool FunctionSignature::operator<(const FunctionSignature& other) const {
    if (name != other.name) return name < other.name;
    if (parameterTypes.size() != other.parameterTypes.size()) {
        return parameterTypes.size() < other.parameterTypes.size();
    }
    for (size_t i = 0; i < parameterTypes.size(); ++i) {
        if (parameterTypes[i] != other.parameterTypes[i]) {
            return parameterTypes[i] < other.parameterTypes[i];
        }
    }
    return false;
}

// 检查是否为构造函数（函数名与类型名相同）
bool FunctionSignature::isConstructor() const {
    // 构造函数通常与类型名相同
    // 这里可以根据命名约定来判断
    return !name.empty() && name[0] >= 'A' && name[0] <= 'Z';
}

// 检查是否为默认构造函数（无参数）
bool FunctionSignature::isDefaultConstructor() const {
    return isConstructor() && parameterTypes.empty();
}

// 检查是否为拷贝构造函数
bool FunctionSignature::isCopyConstructor() const {
    return isConstructor() && parameterTypes.size() == 1 && 
           parameterTypes[0] == "const " + name + "&";
}

// 检查是否为移动构造函数
bool FunctionSignature::isMoveConstructor() const {
    return isConstructor() && parameterTypes.size() == 1 && 
           parameterTypes[0] == name + "&&";
}

// 检查是否为析构函数
bool FunctionSignature::isDestructor() const {
    return name == "~" + name.substr(1); // 假设析构函数名为 ~TypeName
}

// 获取函数类型（普通函数、构造函数、析构函数等）
string FunctionSignature::getFunctionType() const {
    if (isDestructor()) return "destructor";
    if (isConstructor()) return "constructor";
    return "function";
}

// 打印函数签名信息
void FunctionSignature::print() const {
    cout << "Function Signature: " << toString() << endl;
    cout << "  Name: " << name << endl;
    cout << "  Parameters: " << parameterTypes.size() << endl;
    for (size_t i = 0; i < parameterTypes.size(); ++i) {
        cout << "    " << i << ": " << parameterTypes[i] << endl;
    }
    cout << "  Type: " << getFunctionType() << endl;
}

// 从函数原型创建函数签名
FunctionSignature FunctionSignature::fromPrototype(const FunctionPrototype* prototype) {
    return FunctionSignature(prototype);
}

// 从函数定义创建函数签名
FunctionSignature FunctionSignature::fromDefinition(const FunctionDefinition* funcDef) {
    return FunctionSignature(funcDef);
}

// 从函数名和参数类型列表创建函数签名
FunctionSignature FunctionSignature::fromTypes(const string& funcName, const vector<string>& paramTypes) {
    return FunctionSignature(funcName, paramTypes);
}

// 从函数名和参数类型字符串创建函数签名（用于解析）
FunctionSignature FunctionSignature::fromString(const string& signature) {
    // 解析签名字符串，格式：functionName(type1, type2, ...)
    size_t openParen = signature.find('(');
    if (openParen == string::npos) {
        return FunctionSignature(signature);
    }
    
    string funcName = signature.substr(0, openParen);
    string paramsStr = signature.substr(openParen + 1);
    
    // 移除结尾的 ')'
    if (!paramsStr.empty() && paramsStr.back() == ')') {
        paramsStr.pop_back();
    }
    
    vector<string> paramTypes;
    if (!paramsStr.empty()) {
        // 简单的参数类型解析（按逗号分割）
        size_t start = 0;
        size_t pos = 0;
        while ((pos = paramsStr.find(',', start)) != string::npos) {
            string type = paramsStr.substr(start, pos - start);
            // 去除前后空格
            type.erase(0, type.find_first_not_of(" \t"));
            type.erase(type.find_last_not_of(" \t") + 1);
            paramTypes.push_back(type);
            start = pos + 1;
        }
        
        // 添加最后一个参数类型
        if (start < paramsStr.length()) {
            string type = paramsStr.substr(start);
            type.erase(0, type.find_first_not_of(" \t"));
            type.erase(type.find_last_not_of(" \t") + 1);
            paramTypes.push_back(type);
        }
    }
    
    return FunctionSignature(funcName, paramTypes);
}

