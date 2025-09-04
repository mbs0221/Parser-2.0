
#include "interpreter/scope/scope.h"
#include "interpreter/values/value.h"
#include "common/logger.h"
#include <sstream>

using namespace std;

// 辅助函数：判断是否为系统注入的变量
static bool isSystemVariable(const string& key) {
    return (key == "__func__" || key == "this" || key == "instance" || 
            key == "self" || key == "__class_name__" || key == "argc" || 
            key == "args" || key == "kwargs");
}

// 基本构造函数
FunctionSignature::FunctionSignature(const string& n, const vector<Parameter>& params) 
    : name(n), parameters(params) {
}

// 兼容性构造函数
FunctionSignature::FunctionSignature(const string& n, const vector<string>& types, 
                                   const vector<string>& defaults, bool varArgs, const string& varType) 
    : name(n) {
    for (size_t i = 0; i < types.size(); ++i) {
        string defaultValue = (i < defaults.size()) ? defaults[i] : "";
        parameters.push_back(Parameter("arg" + to_string(i), types[i], defaultValue, false));
    }
    if (varArgs) {
        parameters.push_back(Parameter("...", varType, "", true));
    }
}

// 从FunctionPrototype构造
FunctionSignature::FunctionSignature(const FunctionPrototype* prototype) {
    if (prototype) {
        name = prototype->name;
        const vector<pair<string, Type*>>& protoParams = prototype->parameters;
        for (const auto& param : protoParams) {
            string paramName = param.first;
            string paramType = param.second ? param.second->str() : "any";
            parameters.push_back(Parameter(paramName, paramType, "", false));
        }
    }
}

// 从FunctionDefinition构造
FunctionSignature::FunctionSignature(const FunctionDefinition* funcDef) {
    FunctionSignature(funcDef->prototype);
}

// 从Scope构造
FunctionSignature::FunctionSignature(const string& funcName, class Scope* scope) 
    : name(funcName) {
    if (!scope) {
        LOG_DEBUG("FunctionSignature: Scope is null, creating empty signature");
        return;
    }
    
    // 检查是否有可变参数标志
    bool hasVarArgs = scope->hasArgs();
    bool hasVarKwargs = scope->hasKwargs();
    
    LOG_DEBUG("FunctionSignature: Checking for variadic arguments - hasArgs: " + 
              string(hasVarArgs ? "true" : "false") + ", hasKwargs: " + string(hasVarKwargs ? "true" : "false"));
    
    // 对于方法调用，我们不应该从scope中提取参数，因为scope包含了脚本执行过程中的所有变量
    // 方法调用的参数应该通过实际传递的arguments来确定
    // 这里我们创建一个空的签名，让方法查找基于名称进行
    
    // 调试：打印新作用域中的所有变量
    LOG_DEBUG("FunctionSignature: Scope contents for method call '" + funcName + "':");
    if (scope && scope->getObjectRegistry()) {
        vector<string> allNames = scope->getObjectRegistry()->getVariableNames();
        LOG_DEBUG("FunctionSignature: Found " + to_string(allNames.size()) + " variables in scope");
        for (const string& name : allNames) {
            Value* value = scope->getVariable(name);
            if (value) {
                LOG_DEBUG("FunctionSignature: Scope variable: " + name + " = " + value->toString() + " (" + value->getBuiltinTypeName() + ")");
            }
        }
    }
    
    LOG_DEBUG("FunctionSignature: Creating empty signature for method call '" + funcName + "'");
    LOG_DEBUG("FunctionSignature: Final signature: " + toString());
}


// 拷贝构造函数
FunctionSignature::FunctionSignature(const FunctionSignature& other)
    : name(other.name), parameters(other.parameters) {
}

// 赋值运算符
FunctionSignature& FunctionSignature::operator=(const FunctionSignature& other) {
    if (this != &other) {
        name = other.name;
        parameters = other.parameters;
    }
    return *this;
}

// 基本信息方法
string FunctionSignature::getName() const { return name; }
const vector<Parameter>& FunctionSignature::getParameters() const { return parameters; }
size_t FunctionSignature::getParameterCount() const { return parameters.size(); }

size_t FunctionSignature::getRequiredParameterCount() const {
    size_t count = 0;
    for (const auto& param : parameters) {
        if (!param.hasDefaultValue()) count++;
    }
    return count;
}

bool FunctionSignature::supportsVarArgs() const {
    for (const auto& param : parameters) {
        if (param.isVariadic()) return true;
    }
    return false;
}

string FunctionSignature::getVarArgsType() const {
    for (const auto& param : parameters) {
        if (param.isVariadic()) return param.getTypeName();
    }
    return "any";
}

// 兼容性方法
vector<string> FunctionSignature::getParameterTypes() const {
    vector<string> types;
    for (const auto& param : parameters) {
        types.push_back(param.getTypeName());
    }
    return types;
}

// 检查与另一个函数签名的兼容性
bool FunctionSignature::isCompatibleWith(const FunctionSignature& other) const {
    if (name != other.name) return false;
    if (parameters.size() != other.parameters.size()) return false;
    
    for (size_t i = 0; i < parameters.size(); ++i) {
        if (parameters[i].getTypeName() != other.parameters[i].getTypeName()) return false;
    }
    
    return true;
}

vector<string> FunctionSignature::getDefaultValues() const {
    vector<string> defaults;
    for (const auto& param : parameters) {
        defaults.push_back(param.hasDefaultValue() ? param.getDefaultValue() : "");
    }
    return defaults;
}

vector<string> FunctionSignature::getParameterNames() const {
    vector<string> names;
    for (const auto& param : parameters) {
        names.push_back(param.getName());
    }
    return names;
}

// 可变参数相关方法
size_t FunctionSignature::getFixedParameterCount() const {
    size_t count = 0;
    for (const auto& param : parameters) {
        if (!param.isVariadic()) count++;
    }
    return count;
}

size_t FunctionSignature::getVarArgsStartIndex() const {
    for (size_t i = 0; i < parameters.size(); ++i) {
        if (parameters[i].isVariadic()) return i;
    }
    return SIZE_MAX;
}

bool FunctionSignature::isVarArgsParameter(size_t index) const {
    return index < parameters.size() && parameters[index].isVariadic();
}

// 参数匹配和重载解析
bool FunctionSignature::matches(const string& methodName, const vector<Value*>& args) const {
    return methodName == name && canAcceptArguments(args);
}

bool FunctionSignature::matches(const FunctionSignature& other) const {
    if (name != other.name || parameters.size() != other.parameters.size()) return false;
    for (size_t i = 0; i < parameters.size(); ++i) {
        if (parameters[i].getTypeName() != other.parameters[i].getTypeName()) return false;
    }
    return true;
}

int FunctionSignature::calculateMatchScore(const vector<Value*>& args) const {
    if (!canAcceptArguments(args)) return -1;
    
    int score = 0;
    size_t checkCount = min(args.size(), parameters.size());
    
    for (size_t i = 0; i < checkCount; ++i) {
        if (args[i] && !parameters[i].isVariadic()) {
            score += getTypeCompatibilityScore(parameters[i].getTypeName(), args[i]->getValueType());
        }
    }
    
    if (supportsVarArgs() && args.size() > parameters.size()) {
        string varArgsType = getVarArgsType();
        for (size_t i = parameters.size(); i < args.size(); ++i) {
            if (args[i]) {
                score += getTypeCompatibilityScore(varArgsType, args[i]->getValueType());
            }
        }
    }
    
    return score;
}

// 参数处理
vector<Value*> FunctionSignature::fillDefaultArguments(const vector<Value*>& args) const {
    vector<Value*> filledArgs = args;
    while (filledArgs.size() < parameters.size()) {
        size_t index = filledArgs.size();
        if (index < parameters.size() && parameters[index].hasDefaultValue()) {
            filledArgs.push_back(nullptr);
        } else break;
    }
    return filledArgs;
}

bool FunctionSignature::canAcceptArguments(const vector<Value*>& args) const {
    if (args.size() < getRequiredParameterCount()) return false;
    if (!supportsVarArgs() && args.size() > parameters.size()) return false;
    return true;
}

bool FunctionSignature::canAcceptVarArgs(const vector<Value*>& args) const {
    if (!supportsVarArgs()) return false;
    
    for (size_t i = 0; i < min(args.size(), parameters.size()); ++i) {
        if (args[i] && !parameters[i].isVariadic()) {
            if (!isTypeCompatible(parameters[i].getTypeName(), args[i]->getValueType())) return false;
        }
    }
    
    for (size_t i = parameters.size(); i < args.size(); ++i) {
        if (args[i]) {
            if (!isTypeCompatible(getVarArgsType(), args[i]->getValueType())) return false;
        }
    }
    
    return true;
}

// 字符串表示和比较
string FunctionSignature::toString() const {
    stringstream ss;
    ss << name << "(";
    for (size_t i = 0; i < parameters.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << parameters[i].toString();
    }
    ss << ")";
    return ss.str();
}

bool FunctionSignature::operator==(const FunctionSignature& other) const {
    if (name != other.name || parameters.size() != other.parameters.size()) return false;
    for (size_t i = 0; i < parameters.size(); ++i) {
        if (parameters[i] != other.parameters[i]) return false;
    }
    return true;
}

bool FunctionSignature::operator<(const FunctionSignature& other) const {
    if (name != other.name) return name < other.name;
    if (parameters.size() != other.parameters.size()) return parameters.size() < other.parameters.size();
    for (size_t i = 0; i < parameters.size(); ++i) {
        if (parameters[i] != other.parameters[i]) return parameters[i].getName() < other.parameters[i].getName();
    }
    return false;
}

// 构造函数检查
bool FunctionSignature::isConstructor() const {
    return name.find("__init__") != string::npos || name.find("constructor") != string::npos;
}

bool FunctionSignature::isDefaultConstructor() const {
    return isConstructor() && parameters.empty();
}

bool FunctionSignature::isCopyConstructor() const {
    return isConstructor() && parameters.size() == 1 && parameters[0].getTypeName() == "self";
}

bool FunctionSignature::isMoveConstructor() const {
    return isConstructor() && parameters.size() == 1 && parameters[0].getTypeName() == "self";
}

bool FunctionSignature::isDestructor() const {
    return name.find("__del__") != string::npos || name.find("destructor") != string::npos;
}

string FunctionSignature::getFunctionType() const {
    if (isConstructor()) return "constructor";
    if (isDestructor()) return "destructor";
    return "function";
}

// 调试和打印
void FunctionSignature::print() const {
    cout << "FunctionSignature: " << name << endl;
    cout << "  Parameters: " << parameters.size() << endl;
    for (size_t i = 0; i < parameters.size(); ++i) {
        cout << "    " << i << ": " << parameters[i].toString() << endl;
    }
}

// 静态工厂方法
FunctionSignature FunctionSignature::fromPrototype(const FunctionPrototype* prototype) {
    return FunctionSignature(prototype);
}

FunctionSignature FunctionSignature::fromDefinition(const FunctionDefinition* funcDef) {
    return FunctionSignature(funcDef);
}

FunctionSignature FunctionSignature::fromTypes(const string& funcName, const vector<string>& paramTypes) {
    vector<Parameter> params;
    for (const auto& type : paramTypes) {
        params.push_back(Parameter("arg", type, "", false));
    }
    return FunctionSignature(funcName, params);
}

FunctionSignature FunctionSignature::fromString(const string& signature) {
    // 使用单参数的构造函数，传入空参数列表
    return FunctionSignature(signature, vector<Parameter>());
}

// 私有辅助方法
ObjectType* FunctionSignature::getTypeByName(const string& typeName) const {
    return nullptr;
}

bool FunctionSignature::isTypeCompatible(const string& expectedType, ObjectType* actualType) const {
    if (expectedType == "any" || expectedType.empty()) return true;
    if (!actualType) return false;
    return actualType->getTypeName() == expectedType;
}

int FunctionSignature::getTypeCompatibilityScore(const string& expectedType, ObjectType* actualType) const {
    if (!isTypeCompatible(expectedType, actualType)) return -1;
    if (expectedType == "any" || expectedType.empty()) return 0;
    if (actualType->getTypeName() == expectedType) return 100;
    return 50;
}
