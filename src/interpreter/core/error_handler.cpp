#include "interpreter/core/error_handler.h"
#include <iostream>
#include <memory>

namespace InterpreterCore {

// 全局错误处理器实例
std::unique_ptr<ErrorHandler> g_errorHandler = std::make_unique<ErrorHandler>();

// 便捷函数实现
void reportError(const std::string& message, 
                const std::string& errorCode,
                const ErrorLocation& location) {
    if (g_errorHandler) {
        g_errorHandler->reportError(message, errorCode, location);
    } else {
        std::cerr << "[ERROR] " << message << std::endl;
    }
}

void reportWarning(const std::string& message,
                  const std::string& errorCode,
                  const ErrorLocation& location) {
    if (g_errorHandler) {
        g_errorHandler->reportWarning(message, errorCode, location);
    } else {
        std::cerr << "[WARNING] " << message << std::endl;
    }
}

void reportTypeError(const std::string& expected, 
                    const std::string& actual,
                    const ErrorLocation& location) {
    if (g_errorHandler) {
        g_errorHandler->reportTypeError(expected, actual, location);
    } else {
        std::cerr << "[TYPE_ERROR] 期望 " << expected << "，但得到 " << actual << std::endl;
    }
}

void reportUndefinedIdentifier(const std::string& identifier,
                              const ErrorLocation& location) {
    if (g_errorHandler) {
        g_errorHandler->reportUndefinedIdentifier(identifier, location);
    } else {
        std::cerr << "[UNDEFINED_IDENTIFIER] 未定义的标识符: " << identifier << std::endl;
    }
}

void reportFunctionCallError(const std::string& functionName,
                            const std::string& reason,
                            const ErrorLocation& location) {
    if (g_errorHandler) {
        g_errorHandler->reportFunctionCallError(functionName, reason, location);
    } else {
        std::cerr << "[FUNCTION_CALL_ERROR] 函数调用错误 '" << functionName << "': " << reason << std::endl;
    }
}

void reportDivisionByZero(const ErrorLocation& location) {
    if (g_errorHandler) {
        g_errorHandler->reportDivisionByZero(location);
    } else {
        std::cerr << "[DIVISION_BY_ZERO] 除零错误" << std::endl;
    }
}

void reportArrayIndexOutOfBounds(int index, int size,
                                const ErrorLocation& location) {
    if (g_errorHandler) {
        g_errorHandler->reportArrayIndexOutOfBounds(index, size, location);
    } else {
        std::cerr << "[ARRAY_INDEX_OUT_OF_BOUNDS] 数组索引越界: 索引 " << index 
                  << "，数组大小 " << size << std::endl;
    }
}

} // namespace InterpreterCore
