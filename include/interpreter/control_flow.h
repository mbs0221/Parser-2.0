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
public:
    ReturnException() : ControlFlowException("Return") {}
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

#endif // CONTROL_FLOW_H
