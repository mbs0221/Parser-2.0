#include "interpreter/core/function_call.h"
#include "interpreter/scope/scope.h"
#include "interpreter/core/interpreter.h"
#include "interpreter/types/types.h"
#include "interpreter/utils/logger.h"

using namespace std;


// ==================== FunctionCall 基类实现 ====================

FunctionCall::FunctionCall(Scope* scope, std::vector<Value*>& arguments) 
    : currentScope(scope), args(arguments) {}

void FunctionCall::setCurrentScope(Scope* scope) {
    currentScope = scope;
}

Scope* FunctionCall::getCurrentScope() const {
    return currentScope;
}

// ==================== BasicFunctionCall 实现 ====================

BasicFunctionCall::BasicFunctionCall(Scope* scope, Function* func, vector<Value*>& arguments)
    : FunctionCall(scope, arguments), function(func) {}

Value* BasicFunctionCall::execute() {
    if (!function) {
        LOG_ERROR("BasicFunctionCall: Function is null");
        return nullptr;
    }
    
    if (!currentScope) {
        LOG_ERROR("BasicFunctionCall: Current scope not available");
        return nullptr;
    }
    
    // 检查函数是否可调用
    if (!function->isCallable()) {
        LOG_ERROR("BasicFunctionCall: Function is not callable");
        return nullptr;
    }
    
    // 绑定参数到作用域
    bindParameters(args);
    
    // 调用函数
    LOG_DEBUG("BasicFunctionCall: Executing function");
    return function->call(args);
}

void BasicFunctionCall::bindParameters(const vector<Value*>& args) {
    // 对于用户函数，需要绑定参数到作用域
    if (UserFunction* userFunc = dynamic_cast<UserFunction*>(function)) {
        const vector<string>& paramList = userFunc->getParameterList();
        if (!paramList.empty()) {
            LOG_DEBUG("BasicFunctionCall: Binding parameters for user function");
            
            // 绑定参数到作用域
            for (size_t i = 0; i < paramList.size() && i < args.size(); ++i) {
                const string& paramName = paramList[i];
                Value* argValue = args[i];
                currentScope->getObjectRegistry()->defineVariable(paramName, argValue);
                LOG_DEBUG("BasicFunctionCall: Bound parameter '" + paramName + "' to argument");
            }
        }
    }
    // 对于内置函数，不需要参数绑定，因为内置函数自己处理参数
}

// ==================== StaticMethodCall 实现 ====================

StaticMethodCall::StaticMethodCall(Scope* scope, ClassType* cls, Function* method, vector<Value*>& arguments)
    : BasicFunctionCall(scope, method, arguments), classType(cls) {}

Value* StaticMethodCall::execute() {
    if (!classType) {
        LOG_ERROR("StaticMethodCall: Class type is null");
        return nullptr;
    }
    
    if (!currentScope) {
        LOG_ERROR("StaticMethodCall: Current scope not available");
        return nullptr;
    }
    
    // 先注入静态成员到作用域
    injectStaticMembers();
    
    // 然后调用基类方法执行函数
    LOG_DEBUG("StaticMethodCall: Executing static method with class context");
    return BasicFunctionCall::execute();
}

void StaticMethodCall::injectStaticMembers() {
    LOG_DEBUG("StaticMethodCall: Injecting static members into scope");
    
    // 将类的静态成员注入到当前作用域
    for (const auto& staticMember : classType->getStaticMemberTypes()) {
        const string& memberName = staticMember.first;
        Value* memberValue = classType->getStaticMemberValue(memberName);
        
        if (memberValue) {
            currentScope->getObjectRegistry()->defineVariable(memberName, memberValue);
            LOG_DEBUG("StaticMethodCall: Injected static member '" + memberName + "' into scope");
        }
    }
}

// ==================== InstanceMethodCall 实现 ====================

InstanceMethodCall::InstanceMethodCall(Scope* scope, Value* inst, const string& method, vector<Value*>& arguments)
    : BasicFunctionCall(scope, nullptr, arguments), instance(inst), methodName(method) {}

Value* InstanceMethodCall::execute() {
    if (!instance) {
        LOG_ERROR("InstanceMethodCall: Instance is null");
        return nullptr;
    }
    
    if (!currentScope) {
        LOG_ERROR("InstanceMethodCall: Current scope not available");
        return nullptr;
    }
    
    // 获取对象的运行时类型
    ObjectType* objectType = instance->getValueType();
    if (!objectType) {
        LOG_ERROR("InstanceMethodCall: Object has no runtime type");
        return nullptr;
    }
    
    // 检查方法是否存在
    if (!objectType->hasMethod(methodName)) {
        LOG_ERROR("InstanceMethodCall: Method '" + methodName + "' not found in type '" + objectType->getTypeName() + "'");
        return nullptr;
    }
    
    // 先设置this指针到作用域
    setupThisPointer();
    
    try {
        // 通过类型系统调用方法
        LOG_DEBUG("InstanceMethodCall: Executing instance method with this pointer");
        Value* result = objectType->callMethod(instance, methodName, args);
        
        // 清理this指针
        cleanupThisPointer();
        
        return result;
    } catch (...) {
        // 确保异常情况下也清理this指针
        cleanupThisPointer();
        throw;
    }
}

void InstanceMethodCall::setupThisPointer() {
    LOG_DEBUG("InstanceMethodCall: Setting up this pointer in scope");
    currentScope->thisPointer = instance;
}

void InstanceMethodCall::cleanupThisPointer() {
    LOG_DEBUG("InstanceMethodCall: Cleaning up this pointer from scope");
    currentScope->thisPointer = nullptr;
}

// ==================== InterpreterFunctionExecutor 实现 ====================

InterpreterFunctionExecutor::InterpreterFunctionExecutor(Interpreter* interp) 
    : interpreterPtr(interp) {}

Value* InterpreterFunctionExecutor::executeFunction(const vector<Value*>& args, 
                                                  class Statement* functionBody) {
    if (!interpreterPtr) {
        LOG_ERROR("InterpreterFunctionExecutor: No interpreter available");
        return nullptr;
    }
    
    if (!functionBody) {
        LOG_ERROR("InterpreterFunctionExecutor: Function body is null");
        return nullptr;
    }
    
    LOG_DEBUG("InterpreterFunctionExecutor: Executing function body");
    
    try {
        // 不创建新作用域，直接在当前作用域执行函数体
        // 这样可以避免不必要的性能开销，同时保持变量可见性
        LOG_DEBUG("InterpreterFunctionExecutor: Executing function body in current scope");
        
        // 执行函数体（参数绑定由 BasicFunctionCall 处理）
        interpreterPtr->visit(functionBody);
        
        LOG_DEBUG("InterpreterFunctionExecutor: Function execution completed successfully");
        return new String("Function execution completed");
        
    } catch (const exception& e) {
        LOG_ERROR("InterpreterFunctionExecutor: Error executing function: " + string(e.what()));
        return new String("Function execution error: " + string(e.what()));
    }
}

string InterpreterFunctionExecutor::getExecutorName() const {
    return "InterpreterFunctionExecutor";
}

void InterpreterFunctionExecutor::setInterpreter(Interpreter* interp) {
    interpreterPtr = interp;
}

Interpreter* InterpreterFunctionExecutor::getInterpreter() const {
    return interpreterPtr;
}
