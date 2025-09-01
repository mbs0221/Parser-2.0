#include "interpreter/values/value.h"
#include "interpreter/types/types.h"
#include "interpreter/core/function_call.h"
#include "interpreter/utils/logger.h"
#include <sstream>
#include <iostream> // Added for debugging

using namespace std;

// ==================== Function基类实现 ====================
Function::Function(const string& funcName, const vector<string>& params) 
    : Value(nullptr), name(funcName), parameters(params) {
}

string Function::getName() const {
    return name;
}

const vector<string>& Function::getParameters() const {
    return parameters;
}

const vector<string>& Function::getParameterList() const {
    return parameters;
}

bool Function::validateArguments(const vector<Value*>& args) const {
    // 检查是否有可变参数标记 "..."
    bool hasVariadic = false;
    size_t minParams = 0;
    
    LOG_DEBUG("validateArguments for '" + name + "' with " + to_string(args.size()) + " args, " + to_string(parameters.size()) + " params");
    
    for (const string& param : parameters) {
        LOG_DEBUG("param: '" + param + "'");
        if (param == "...") {
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

string Function::getSignature() const {
    stringstream ss;
    ss << name << "(";
    for (size_t i = 0; i < parameters.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << parameters[i];
    }
    ss << ")";
    return ss.str();
}

bool Function::isCallable() const {
    return true;
}

string Function::toString() const {
    return getSignature();
}

bool Function::toBool() const {
    return true; // 函数对象总是为true
}

string Function::getBuiltinTypeName() const {
    return "function";
}

// ==================== BuiltinFunction实现 ====================
BuiltinFunction::BuiltinFunction(const string& funcName, 
                                 function<Value*(vector<Value*>)> f,
                                 const vector<string>& params)
    : Function(funcName, params), func(f) {
}

// 新增：接受函数指针的构造函数
BuiltinFunction::BuiltinFunction(const string& funcName, 
                                 Value* (*f)(vector<Value*>&),
                                 const vector<string>& params)
    : Function(funcName, params) {
    // 将函数指针包装为std::function
    func = [f](vector<Value*> args) -> Value* {
        return f(args);
    };
    
    // 调试信息
    LOG_DEBUG("BuiltinFunction '" + funcName + "' created with " + to_string(params.size()) + " parameters");
    for (size_t i = 0; i < params.size(); ++i) {
        LOG_DEBUG("  param[" + to_string(i) + "]: " + params[i]);
    }
}

Value* BuiltinFunction::call(vector<Value*> args) {
    if (validateArguments(args)) {
        return func(args);
    }
    return nullptr;
}

string BuiltinFunction::getFunctionType() const {
    return "builtin";
}

Value* BuiltinFunction::clone() const {
    return new BuiltinFunction(name, func, parameters);
}

bool BuiltinFunction::isBuiltinFunction() const {
    return true;
}

// ==================== UserFunction实现 ====================
UserFunction::UserFunction(const string& funcName, 
                           const vector<string>& params,
                           Statement* body)
    : Function(funcName, params), functionBody(body), executor(nullptr) {
}

Value* UserFunction::call(vector<Value*> args) {
    if (validateArguments(args)) {
        // 使用执行器来执行函数体
        if (executor) {
            return executor->executeFunction(args, functionBody);
        } else if (functionBody) {
            // 如果没有执行器但有函数体，可以尝试直接执行
            // 这里需要更复杂的AST执行逻辑
            // 暂时返回nullptr
            return nullptr;
        }
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
