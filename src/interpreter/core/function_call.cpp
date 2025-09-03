#include "interpreter/core/function_call.h"
#include "interpreter/scope/scope.h"
#include "interpreter/core/interpreter.h"
#include "interpreter/types/types.h"
#include "interpreter/utils/logger.h"

using namespace std;


// ==================== FunctionCall 基类实现 ====================

FunctionCall::FunctionCall(Scope* scope, const std::vector<Value*>& arguments) 
    : currentScope(scope), args(arguments) {}

void FunctionCall::setCurrentScope(Scope* scope) {
    currentScope = scope;
}

Scope* FunctionCall::getCurrentScope() const {
    return currentScope;
}

// ==================== BasicFunctionCall 实现 ====================

BasicFunctionCall::BasicFunctionCall(Scope* scope, Function* func, const vector<Value*>& arguments)
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
    
    // 调用函数 - 使用scope机制
    LOG_DEBUG("BasicFunctionCall: Executing function with scope");
    return function->call(currentScope);
}

void BasicFunctionCall::bindParameters(const vector<Value*>& args) {
    // 统一采用scope方式执行，不区分函数类型
    LOG_DEBUG("BasicFunctionCall: Binding parameters using unified scope approach");
    
    // 将函数名注册到作用域中，这样函数内部就能知道自己的函数名
    string functionName = function->getName();
    currentScope->setArgument("__func__", new String(functionName));
    LOG_DEBUG("BasicFunctionCall: Registered function name '" + functionName + "' to scope as __func__");
    
    // 使用虚函数获取参数名称，避免dynamic_cast
    vector<string> paramNames = function->getParameterNames();
    
    // 过滤掉可变参数标记 "..."，只保留实际的参数名
    vector<string> actualParamNames;
    bool hasVariadic = false;
    for (const string& paramName : paramNames) {
        if (paramName == "...") {
            hasVariadic = true;
        } else {
            actualParamNames.push_back(paramName);
        }
    }
    
    LOG_DEBUG("BasicFunctionCall: Original param names: " + to_string(paramNames.size()) + 
              ", Actual param names: " + to_string(actualParamNames.size()) + 
              ", Has variadic: " + string(hasVariadic ? "true" : "false"));
    
    // 按照实际参数名称列表绑定参数到作用域
    if (!actualParamNames.empty()) {
        for (size_t i = 0; i < actualParamNames.size() && i < args.size(); ++i) {
            const string& paramName = actualParamNames[i];
            Value* argValue = args[i];
            currentScope->setArgument(paramName, argValue);
            LOG_DEBUG("BasicFunctionCall: Bound parameter '" + paramName + "' to argument at index " + to_string(i));
        }
    }
    
    // 处理剩余参数：支持*args和**kwargs两种方式
    if (args.size() > actualParamNames.size()) {
        size_t remainingCount = args.size() - actualParamNames.size();
        
        // 检查剩余参数的模式
        bool isKeyValuePairs = true;
        for (size_t i = actualParamNames.size(); i < args.size(); i += 2) {
            if (i + 1 >= args.size() || !args[i] || !args[i + 1]) {
                isKeyValuePairs = false;
                break;
            }
        }
        
        if (isKeyValuePairs && remainingCount % 2 == 0 && remainingCount > 0) {
            // **kwargs模式：键值对形式
            Dict* kwargs = new Dict();
            for (size_t i = actualParamNames.size(); i < args.size(); i += 2) {
                Value* key = args[i];
                Value* value = args[i + 1];
                if (key && value) {
                    kwargs->setEntry(key->toString(), value);
                    LOG_DEBUG("BasicFunctionCall: Added to kwargs: " + key->toString() + " = " + value->toString());
                }
            }
            currentScope->setArgument("kwargs", kwargs);
            LOG_DEBUG("BasicFunctionCall: Created kwargs with " + to_string(kwargs->getSize()) + " key-value pairs");
        } else if (remainingCount > 0) {
            // *args模式：位置参数形式
            Array* argsArray = new Array();
            for (size_t i = actualParamNames.size(); i < args.size(); ++i) {
                if (args[i]) {
                    argsArray->addElement(args[i]->clone());
                    LOG_DEBUG("BasicFunctionCall: Added to args: " + args[i]->toString());
                }
            }
            currentScope->setArgument("args", argsArray);
            LOG_DEBUG("BasicFunctionCall: Created args with " + to_string(argsArray->getSize()) + " positional arguments");
        }
    }
    
    // 保持向后兼容：设置参数总数和参数数组
    currentScope->setArgument("argc", new Integer(args.size()));
    
    // 简单的参数数量日志
    LOG_DEBUG("BasicFunctionCall: About to create Array with " + to_string(args.size()) + " arguments");
    
    // 修复：Array构造函数需要三个参数，第三个参数是元素类型
    currentScope->setArgument("args", new Array(args, nullptr, nullptr));
    LOG_DEBUG("BasicFunctionCall: Bound " + to_string(args.size()) + " arguments using kwargs approach");
}

// ==================== StaticMethodCall 实现 ====================

StaticMethodCall::StaticMethodCall(Scope* scope, ClassType* cls, Function* method, const vector<Value*>& arguments)
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
    
    // 将类名注册到作用域中，这样静态方法内部就能知道当前类的名称
    string className = classType->getTypeName();
    currentScope->setArgument("__class_name__", new String(className));
    LOG_DEBUG("StaticMethodCall: Registered class name '" + className + "' to scope as __class_name__");
    
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

// 构造函数：接受MethodReference对象
InstanceMethodCall::InstanceMethodCall(Scope* scope, Value* inst, MethodReference* methodRef, const vector<Value*>& arguments)
    : BasicFunctionCall(scope, nullptr, arguments), instance(inst), methodRef(methodRef) {}

Value* InstanceMethodCall::execute() {
    if (!instance) {
        LOG_ERROR("InstanceMethodCall: Instance is null");
        return nullptr;
    }
    
    if (!currentScope) {
        LOG_ERROR("InstanceMethodCall: Current scope not available");
        return nullptr;
    }
    
    if (!methodRef) {
        LOG_ERROR("InstanceMethodCall: MethodReference is null");
        return nullptr;
    }
    
    // 绑定实例到作用域
    bindInstanceToScope();
    
    try {
        // 委托给MethodReference执行，它会自动处理：
        // 1. 方法查找（如果还没找到）
        // 2. 参数绑定
        // 3. 方法执行
        LOG_DEBUG("InstanceMethodCall: Delegating execution to MethodReference");
        Value* result = methodRef->call(currentScope);
        return result;
    } catch (...) {
        // 异常情况下不需要特殊处理，作用域会自动清理
        throw;
    }
}

void InstanceMethodCall::bindInstanceToScope() {
    LOG_DEBUG("InstanceMethodCall: Binding instance to scope variables");
    
    // 绑定实例到作用域变量
    currentScope->setVariable("this", instance);
    currentScope->setVariable("instance", instance);
    // 增加self变量，符合Python等语言的惯例
    currentScope->setVariable("self", instance);
    
    // 将类名注册到作用域中，这样实例方法内部就能知道当前类的名称
    ObjectType* instanceType = instance->getValueType();
    if (instanceType) {
        string className = instanceType->getTypeName();
        currentScope->setArgument("__class_name__", new String(className));
        LOG_DEBUG("InstanceMethodCall: Registered class name '" + className + "' to scope as __class_name__");
    }
    
    LOG_DEBUG("InstanceMethodCall: Bound instance: " + instance->toString());
}

// ==================== InterpreterFunctionExecutor 实现 ====================

InterpreterFunctionExecutor::InterpreterFunctionExecutor(Interpreter* interp) 
    : interpreterPtr(interp) {}

Value* InterpreterFunctionExecutor::executeFunction(class Statement* functionBody) {
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
    } catch (ReturnException& e) {
        return e.getValue<Value*>();
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
