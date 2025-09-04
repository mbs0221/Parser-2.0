#include "interpreter/values/value.h"
#include "interpreter/types/types.h"
#include "interpreter/core/function_call.h"
#include "interpreter/scope/scope.h"
#include "common/logger.h"
#include "common/function_signature_parser.h"
#include <algorithm>
#include <sstream>
#include <iostream> // Added for debugging

using namespace std;

// ==================== Parameter类实现 ====================

Parameter::Parameter(const string& paramName, const string& paramType, const string& defaultVal, bool varArgs)
    : name(paramName), typeName(paramType), defaultValue(defaultVal), hasDefault(!defaultVal.empty()), isVarArgs(varArgs) {
}

Parameter::Parameter(const Parameter& other)
    : name(other.name), typeName(other.typeName), defaultValue(other.defaultValue), 
      hasDefault(other.hasDefault), isVarArgs(other.isVarArgs) {
}

Parameter& Parameter::operator=(const Parameter& other) {
    if (this != &other) {
        name = other.name;
        typeName = other.typeName;
        defaultValue = other.defaultValue;
        hasDefault = other.hasDefault;
        isVarArgs = other.isVarArgs;
    }
    return *this;
}

string Parameter::toString() const {
    stringstream ss;
    ss << typeName << " " << name;
    if (hasDefault) {
        ss << " = " << defaultValue;
    }
    if (isVarArgs) {
        ss << "...";
    }
    return ss.str();
}

bool Parameter::operator==(const Parameter& other) const {
    return name == other.name && 
           typeName == other.typeName && 
           defaultValue == other.defaultValue && 
           hasDefault == other.hasDefault && 
           isVarArgs == other.isVarArgs;
}

bool Parameter::operator!=(const Parameter& other) const {
    return !(*this == other);
}

bool Parameter::isTypeCompatible(const string& actualType) const {
    if (typeName == "any") return true;
    if (typeName == actualType) return true;
    
    // 通过TypeRegistry检查类型兼容性
    TypeRegistry* registry = TypeRegistry::getGlobalInstance();
    if (registry) {
        ObjectType* expectedType = registry->getType(typeName);
        ObjectType* actualTypeObj = registry->getType(actualType);
        if (expectedType && actualTypeObj) {
            return actualTypeObj->isCompatibleWith(expectedType);
        }
    }
    return false;
}

bool Parameter::isTypeCompatible(ObjectType* actualType) const {
    if (!actualType) return false;
    if (typeName == "any") return true;
    
    // 通过TypeRegistry检查类型兼容性
    TypeRegistry* registry = TypeRegistry::getGlobalInstance();
    if (registry) {
        ObjectType* expectedType = registry->getType(typeName);
        if (expectedType) {
            return actualType->isCompatibleWith(expectedType);
        }
    }
    return false;
}

// ==================== Function基类实现 ====================
Function::Function(const string& funcName, const vector<Parameter>& params) 
    : Value(nullptr), name(funcName), parameters(params) {
}

string Function::getName() const {
    return name;
}

const vector<Parameter>& Function::getParameters() const {
    return parameters;
}

// 兼容性方法：获取参数名称列表
vector<string> Function::getParameterNames() const {
    vector<string> names;
    for (const Parameter& param : parameters) {
        names.push_back(param.getName());
    }
    return names;
}

// 兼容性方法：获取参数类型列表
vector<string> Function::getParameterTypes() const {
    vector<string> types;
    for (const Parameter& param : parameters) {
        types.push_back(param.getTypeName());
    }
    return types;
}

// 兼容性方法：获取带默认值的参数
vector<string> Function::getParametersWithDefaults() const {
    vector<string> names;
    for (const Parameter& param : parameters) {
        if (param.hasDefaultValue()) {
            names.push_back(param.getName());
        }
    }
    return names;
}

bool Function::validateArguments(const vector<Value*>& args) const {
    // 检查是否有可变参数标记 "..."
    bool hasVariadic = false;
    size_t minParams = 0;
    
    LOG_DEBUG("validateArguments for '" + name + "' with " + to_string(args.size()) + " args, " + to_string(parameters.size()) + " params");
    
    for (const Parameter& param : parameters) {
        LOG_DEBUG("param: '" + param.toString() + "'");
        if (param.isVariadic()) {
            hasVariadic = true;
            break;
        }
        minParams++;
    }
    
    LOG_DEBUG("hasVariadic=" + string(hasVariadic ? "true" : "false") + ", minParams=" + to_string(minParams));
    
    if (hasVariadic) {
        // 可变参数函数：参数数量必须至少等于最小参数数量
        bool result = args.size() >= minParams;
        LOG_DEBUG("variadic validation result=" + string(result ? "true" : "false"));
        return result;
    } else {
        // 固定参数函数：参数数量必须完全匹配
        bool result = args.size() == parameters.size();
        LOG_DEBUG("fixed validation result=" + string(result ? "true" : "false"));
        return result;
    }
}

bool Function::isCallable() const {
    return true;
}

string Function::toString() const {
    stringstream ss;
    ss << name << "(";
    for (size_t i = 0; i < parameters.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << parameters[i].toString();
    }
    ss << ")";
    return ss.str();
}

bool Function::toBool() const {
    return true; // 函数对象总是为true
}

string Function::getBuiltinTypeName() const {
    return "function";
}

// ==================== BuiltinFunction实现 ====================

// 新式构造函数：完全通过作用域获取this和参数（使用Parameter对象）
BuiltinFunction::BuiltinFunction(const string& funcName, 
                                 function<Value*(class Scope*)> f,
                                 const vector<Parameter>& params)
    : Function(funcName, params), scopeFunc(f) {
    // 从Parameter对象中提取参数名称
    for (const Parameter& param : params) {
        parameterNames.push_back(param.getName());
    }
    LOG_DEBUG("BuiltinFunction '" + funcName + "' created with new-style scope interface and Parameter objects");
}

// 兼容性构造函数：接受字符串参数列表
BuiltinFunction::BuiltinFunction(const string& funcName, 
                                 function<Value*(class Scope*)> f,
                                 const vector<string>& paramNames)
    : Function(funcName, {}), scopeFunc(f), parameterNames(paramNames) {
    // 将字符串参数名称转换为Parameter对象
    for (const string& paramName : paramNames) {
        parameters.push_back(Parameter(paramName, "any", "", false));
    }
    LOG_DEBUG("BuiltinFunction '" + funcName + "' created with string parameter names for compatibility");
}

// C风格函数原型构造函数：直接解析完整函数原型
BuiltinFunction::BuiltinFunction(function<Value*(class Scope*)> f,
                                 const char* functionPrototype)
    : Function("", {}), scopeFunc(f), parameterNames({}) {
    
    // 解析C风格函数原型（包括函数名和参数）
    parseCFormatParams(functionPrototype);
    
    LOG_DEBUG("BuiltinFunction '" + name + "' created with C-style prototype: " + functionPrototype);
}

// C风格可变参数构造函数：支持混合固定参数和可变参数（使用Parameter对象）
BuiltinFunction::BuiltinFunction(const string& funcName, 
                                 function<Value*(class Scope*)> f,
                                 const vector<Parameter>& fixedParams,
                                 bool supportsVarArgs)
    : Function(funcName, fixedParams), scopeFunc(f) {
    
    // 从Parameter对象中提取参数名称
    for (const Parameter& param : fixedParams) {
        parameterNames.push_back(param.getName());
    }
    
    if (supportsVarArgs) {
        // 添加可变参数标记
        parameters.push_back(Parameter("...", "any", "", true));
        parameterNames.push_back("...");
        varArgsSupport = true;
    }
    
    LOG_DEBUG("BuiltinFunction '" + funcName + "' created with " + 
              to_string(fixedParams.size()) + " fixed params, varargs=" + 
              string(supportsVarArgs ? "true" : "false"));
}

// 使用统一的函数签名解析器解析C风格函数原型
void BuiltinFunction::parseCFormatParams(const char* functionPrototype) {
    if (!functionPrototype) return;
    
    // 使用统一的函数签名解析器
    FunctionSignatureParseResult result = FunctionSignatureParser::parseCFormatParams(functionPrototype);
    
    // 设置函数名
    name = result.functionName;
    
    // 设置参数列表
    parameters = result.parameters;
    parameterNames = result.parameterNames;
    paramDefaults = result.paramDefaults;
    varArgsSupport = result.varArgsSupport;
    varArgsName = result.varArgsName;
    
    LOG_DEBUG("BuiltinFunction: C-style prototype parsing completed using unified parser. Function: " + name + 
              ", Parameters: " + to_string(parameters.size()) + 
              ", VarArgs: " + string(varArgsSupport ? "true" : "false"));
    
    // 调试：打印所有解析的参数
    for (size_t i = 0; i < parameters.size(); ++i) {
        LOG_DEBUG("BuiltinFunction: Parameter " + to_string(i) + ": " + parameters[i].toString());
    }
}

// 获取参数的默认值
std::string BuiltinFunction::getParamDefaultValue(const std::string& paramName) const {
    auto it = paramDefaults.find(paramName);
    if (it != paramDefaults.end()) {
        return it->second;
    }
    return ""; // 返回空字符串表示没有默认值
}

// 检查参数是否有默认值
bool BuiltinFunction::hasParamDefaultValue(const std::string& paramName) const {
    return paramDefaults.find(paramName) != paramDefaults.end();
}

// 获取所有带默认值的参数
std::vector<std::string> BuiltinFunction::getParamsWithDefaults() const {
    std::vector<std::string> result;
    for (const auto& pair : paramDefaults) {
        result.push_back(pair.first);
    }
    return result;
}



// 新增：支持scope的函数调用
Value* BuiltinFunction::call(Scope* scope) {
    if (scopeFunc) {
        // 直接调用scope函数
        LOG_DEBUG("BuiltinFunction '" + name + "' calling scope function directly");
        return scopeFunc(scope);
    } else {
        LOG_DEBUG("BuiltinFunction '" + name + "' does not support scope interface");
        return nullptr;
    }
}

string BuiltinFunction::getFunctionType() const {
    return "builtin";
}

Value* BuiltinFunction::clone() const {
    // 根据函数类型选择合适的构造函数
    if (scopeFunc) {
        // 明确指定使用3参数构造函数，避免歧义
        // 使用 false 作为第4个参数来明确调用3参数构造函数
        // 将字符串参数转换为Parameter对象
        vector<Parameter> paramObjects;
        for (const auto& param : parameterNames) {
            paramObjects.push_back(Parameter("arg", param, "", false));
        }
        return new BuiltinFunction(name, scopeFunc, paramObjects, false);
    } else {
        // 将字符串参数转换为Parameter对象
        vector<Parameter> paramObjects;
        for (const auto& param : parameterNames) {
            paramObjects.push_back(Parameter("arg", param, "", false));
        }
        return new BuiltinFunction(name, scopeFunc, paramObjects, false);
    }
}

bool BuiltinFunction::isBuiltinFunction() const {
    return true;
}

FunctionSignature BuiltinFunction::getSignature() const {
    // 从Parameter对象中提取类型信息和默认值
    vector<string> paramTypes;
    vector<string> defaults;
    bool hasVarArgs = false;
    string varArgsType = "any";
    
    LOG_DEBUG("BuiltinFunction::getSignature: Function '" + name + "' has " + to_string(parameters.size()) + " parameters");
    
    for (const Parameter& param : parameters) {
        LOG_DEBUG("BuiltinFunction::getSignature: Parameter: " + param.toString());
        if (param.isVariadic()) {
            hasVarArgs = true;
            varArgsType = param.getTypeName();
        } else {
            paramTypes.push_back(param.getTypeName());
            if (param.hasDefaultValue()) {
                defaults.push_back(param.getDefaultValue());
            }
        }
    }
    
    FunctionSignature result = FunctionSignature(name, paramTypes, defaults, hasVarArgs, varArgsType);
    LOG_DEBUG("BuiltinFunction::getSignature: Generated signature: " + result.toString());
    return result;
}

// ==================== BuiltinFunction 可变参数支持方法 ====================

bool BuiltinFunction::supportsVarArgs() const {
    return varArgsSupport;
}

void BuiltinFunction::setVarArgsSupport(bool support) {
    varArgsSupport = support;
}

std::string BuiltinFunction::getVarArgsName() const {
    return varArgsName;
}

void BuiltinFunction::setVarArgsName(const std::string& name) {
    varArgsName = name;
}

// ==================== UserFunction实现 ====================
// 构造函数：只接受Parameter数组
UserFunction::UserFunction(const string& funcName, 
                           const vector<Parameter>& params,
                           Statement* body)
    : Function(funcName, params), functionBody(body), executor(nullptr) {
}



// 新增：支持scope的函数调用
Value* UserFunction::call(Scope* scope) {
    if (executor) {
        // 从scope获取参数
        vector<Value*> args;
        int argc = scope->getArgument<Integer>("argc")->getValue();
        
        for (int i = 0; i < argc; ++i) {
            string paramName = "arg" + to_string(i);
            Value* arg = scope->getArgument<Value>(paramName);
            if (arg) {
                args.push_back(arg);
            }
        }
        
        // 使用执行器执行函数体
        LOG_DEBUG("UserFunction '" + name + "' calling executor with scope, extracted " + to_string(args.size()) + " arguments");
        return executor->executeFunction(functionBody);
    } else if (functionBody) {
        // 如果没有执行器但有函数体，可以尝试直接执行
        // 这里需要更复杂的AST执行逻辑
        // 暂时返回nullptr
        return nullptr;
    }
    return nullptr;
}

string UserFunction::getFunctionType() const {
    return "user";
}

Value* UserFunction::clone() const {
    return new UserFunction(name, parameters, functionBody);
}

Statement* UserFunction::getFunctionBody() const {
    return functionBody;
}

void UserFunction::setFunctionBody(Statement* body) {
    functionBody = body;
}

FunctionExecutor* UserFunction::getExecutor() const {
    return executor;
}

void UserFunction::setExecutor(FunctionExecutor* exec) {
    executor = exec;
}

bool UserFunction::isBuiltinFunction() const {
    return false;
}

FunctionSignature UserFunction::getSignature() const {
    return FunctionSignature(name, parameters);
}





// 实现setParameters方法
void Function::setParameters(const vector<Value*>& args) {
    parameters.clear();
    for (Value* arg : args) {
        if (arg) {
            // 获取参数的类型名称
            string typeName = arg->getBuiltinTypeName();
            parameters.push_back(Parameter("arg" + to_string(parameters.size()), typeName, "", false));
        } else {
            // 空参数用"null"表示
            parameters.push_back(Parameter("arg" + to_string(parameters.size()), "null", "", false));
        }
    }
}

// 实现setParameters方法（使用Parameter对象）



