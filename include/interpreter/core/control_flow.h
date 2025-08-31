#ifndef CONTROL_FLOW_H
#define CONTROL_FLOW_H

#include <stdexcept>
#include <string>

// 控制流异常基类
class ControlFlowException : public std::exception {
protected:
    std::string message;
    
public:
    ControlFlowException(const std::string& msg) : message(msg) {}
    
    virtual const char* what() const noexcept override {
        return message.c_str();
    }
    
    virtual ~ControlFlowException() = default;
};

// Return异常
class ReturnException : public ControlFlowException {
private:
    void* returnValue;  // 返回值指针
    
public:
    ReturnException() : ControlFlowException("Return"), returnValue(nullptr) {}
    ReturnException(void* value) : ControlFlowException("Return"), returnValue(value) {}
    
    // 获取返回值
    void* getValue() const { return returnValue; }
    
    // 设置返回值
    void setValue(void* value) { returnValue = value; }
};

// Break异常
class BreakException : public ControlFlowException {
public:
    BreakException() : ControlFlowException("Break") {}
};

// Continue异常
class ContinueException : public ControlFlowException {
public:
    ContinueException() : ControlFlowException("Continue") {}
};

// 运行时异常
class RuntimeException : public ControlFlowException {
public:
    RuntimeException(const std::string& msg) : ControlFlowException("Runtime Error: " + msg) {}
};

// 类型错误异常
class TypeErrorException : public ControlFlowException {
public:
    TypeErrorException(const std::string& expected, const std::string& actual) 
        : ControlFlowException("Type Error: expected " + expected + ", got " + actual) {}
};

// 未定义标识符异常
class UndefinedIdentifierException : public ControlFlowException {
public:
    UndefinedIdentifierException(const std::string& name) 
        : ControlFlowException("Undefined identifier: " + name) {}
};

// 函数调用异常
class FunctionCallException : public ControlFlowException {
public:
    FunctionCallException(const std::string& funcName, const std::string& reason) 
        : ControlFlowException("Function call error in '" + funcName + "': " + reason) {}
};

#endif // CONTROL_FLOW_H
