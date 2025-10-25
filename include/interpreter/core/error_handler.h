#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <chrono>
#include <iomanip>

#include "interpreter/values/value.h"

namespace InterpreterCore {

// 错误严重级别
enum class ErrorSeverity {
    INFO,       // 信息
    WARNING,    // 警告
    ERROR,      // 错误
    FATAL       // 致命错误
};

// 错误位置信息
struct ErrorLocation {
    std::string filename;
    int line;
    int column;
    std::string context;  // 错误上下文代码
    
    ErrorLocation() : line(-1), column(-1) {}
    ErrorLocation(const std::string& file, int l, int c, const std::string& ctx = "")
        : filename(file), line(l), column(c), context(ctx) {}
    
    std::string toString() const {
        std::ostringstream oss;
        if (!filename.empty()) {
            oss << filename;
            if (line > 0) {
                oss << ":" << line;
                if (column > 0) {
                    oss << ":" << column;
                }
            }
        }
        return oss.str();
    }
};

// 错误信息结构
struct ErrorInfo {
    ErrorSeverity severity;
    std::string message;
    std::string errorCode;
    ErrorLocation location;
    std::vector<std::string> suggestions;  // 修复建议
    std::chrono::system_clock::time_point timestamp;
    
    ErrorInfo(ErrorSeverity sev, const std::string& msg, const std::string& code = "")
        : severity(sev), message(msg), errorCode(code), 
          timestamp(std::chrono::system_clock::now()) {}
    
    std::string getFormattedMessage() const {
        std::ostringstream oss;
        
        // 时间戳
        auto time_t = std::chrono::system_clock::to_time_t(timestamp);
        oss << "[" << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S") << "] ";
        
        // 严重级别
        switch (severity) {
            case ErrorSeverity::INFO:    oss << "[INFO] "; break;
            case ErrorSeverity::WARNING: oss << "[WARN] "; break;
            case ErrorSeverity::ERROR:   oss << "[ERROR] "; break;
            case ErrorSeverity::FATAL:   oss << "[FATAL] "; break;
        }
        
        // 错误代码
        if (!errorCode.empty()) {
            oss << "[" << errorCode << "] ";
        }
        
        // 位置信息
        if (!location.filename.empty() || location.line > 0) {
            oss << location.toString() << ": ";
        }
        
        // 错误消息
        oss << message;
        
        // 修复建议
        if (!suggestions.empty()) {
            oss << "\n建议:";
            for (const auto& suggestion : suggestions) {
                oss << "\n  - " << suggestion;
            }
        }
        
        return oss.str();
    }
};

// 错误处理器类
class ErrorHandler {
private:
    std::vector<ErrorInfo> errors;
    std::vector<ErrorInfo> warnings;
    bool stopOnFirstError;
    std::string currentFile;
    
    // 错误统计
    int errorCount;
    int warningCount;
    
public:
    ErrorHandler(bool stopOnError = false) 
        : stopOnFirstError(stopOnError), errorCount(0), warningCount(0) {}
    
    // 设置当前文件
    void setCurrentFile(const std::string& filename) {
        currentFile = filename;
    }
    
    // 报告错误
    void reportError(const std::string& message, 
                    const std::string& errorCode = "",
                    const ErrorLocation& location = ErrorLocation(),
                    const std::vector<std::string>& suggestions = {}) {
        
        ErrorLocation loc = location;
        if (loc.filename.empty() && !currentFile.empty()) {
            loc.filename = currentFile;
        }
        
        ErrorInfo error(ErrorSeverity::ERROR, message, errorCode);
        error.location = loc;
        error.suggestions = suggestions;
        
        errors.push_back(error);
        errorCount++;
        
        std::cerr << error.getFormattedMessage() << std::endl;
        
        if (stopOnFirstError) {
            throw std::runtime_error(message);
        }
    }
    
    // 报告警告
    void reportWarning(const std::string& message,
                      const std::string& errorCode = "",
                      const ErrorLocation& location = ErrorLocation(),
                      const std::vector<std::string>& suggestions = {}) {
        
        ErrorLocation loc = location;
        if (loc.filename.empty() && !currentFile.empty()) {
            loc.filename = currentFile;
        }
        
        ErrorInfo warning(ErrorSeverity::WARNING, message, errorCode);
        warning.location = loc;
        warning.suggestions = suggestions;
        
        warnings.push_back(warning);
        warningCount++;
        
        std::cerr << warning.getFormattedMessage() << std::endl;
    }
    
    // 报告信息
    void reportInfo(const std::string& message,
                   const ErrorLocation& location = ErrorLocation()) {
        
        ErrorLocation loc = location;
        if (loc.filename.empty() && !currentFile.empty()) {
            loc.filename = currentFile;
        }
        
        ErrorInfo info(ErrorSeverity::INFO, message);
        info.location = loc;
        
        std::cout << info.getFormattedMessage() << std::endl;
    }
    
    // 报告致命错误
    void reportFatal(const std::string& message,
                    const std::string& errorCode = "",
                    const ErrorLocation& location = ErrorLocation()) {
        
        ErrorLocation loc = location;
        if (loc.filename.empty() && !currentFile.empty()) {
            loc.filename = currentFile;
        }
        
        ErrorInfo fatal(ErrorSeverity::FATAL, message, errorCode);
        fatal.location = loc;
        
        std::cerr << fatal.getFormattedMessage() << std::endl;
        throw std::runtime_error(message);
    }
    
    // 类型错误
    void reportTypeError(const std::string& expected, 
                        const std::string& actual,
                        const ErrorLocation& location = ErrorLocation()) {
        
        std::vector<std::string> suggestions = {
            "检查变量类型声明",
            "使用类型转换函数",
            "确认函数参数类型"
        };
        
        reportError("类型错误: 期望 " + expected + "，但得到 " + actual,
                   "TYPE_ERROR", location, suggestions);
    }
    
    // 未定义标识符错误
    void reportUndefinedIdentifier(const std::string& identifier,
                                  const ErrorLocation& location = ErrorLocation()) {
        
        std::vector<std::string> suggestions = {
            "检查变量名拼写",
            "确认变量已声明",
            "检查作用域范围"
        };
        
        reportError("未定义的标识符: " + identifier,
                   "UNDEFINED_IDENTIFIER", location, suggestions);
    }
    
    // 函数调用错误
    void reportFunctionCallError(const std::string& functionName,
                                const std::string& reason,
                                const ErrorLocation& location = ErrorLocation()) {
        
        std::vector<std::string> suggestions = {
            "检查函数名拼写",
            "确认函数已定义",
            "检查参数数量和类型"
        };
        
        reportError("函数调用错误 '" + functionName + "': " + reason,
                   "FUNCTION_CALL_ERROR", location, suggestions);
    }
    
    // 除零错误
    void reportDivisionByZero(const ErrorLocation& location = ErrorLocation()) {
        std::vector<std::string> suggestions = {
            "在除法前检查除数是否为零",
            "使用条件语句避免除零"
        };
        
        reportError("除零错误",
                   "DIVISION_BY_ZERO", location, suggestions);
    }
    
    // 数组越界错误
    void reportArrayIndexOutOfBounds(int index, int size,
                                    const ErrorLocation& location = ErrorLocation()) {
        std::vector<std::string> suggestions = {
            "检查数组索引范围",
            "使用条件语句验证索引",
            "确认数组大小"
        };
        
        reportError("数组索引越界: 索引 " + std::to_string(index) + 
                   "，数组大小 " + std::to_string(size),
                   "ARRAY_INDEX_OUT_OF_BOUNDS", location, suggestions);
    }
    
    // 获取错误统计
    int getErrorCount() const { return errorCount; }
    int getWarningCount() const { return warningCount; }
    bool hasErrors() const { return errorCount > 0; }
    bool hasWarnings() const { return warningCount > 0; }
    
    // 获取所有错误
    const std::vector<ErrorInfo>& getErrors() const { return errors; }
    const std::vector<ErrorInfo>& getWarnings() const { return warnings; }
    
    // 清除所有错误和警告
    void clear() {
        errors.clear();
        warnings.clear();
        errorCount = 0;
        warningCount = 0;
    }
    
    // 生成错误报告
    std::string generateReport() const {
        std::ostringstream oss;
        
        oss << "=== 错误报告 ===" << std::endl;
        oss << "错误数量: " << errorCount << std::endl;
        oss << "警告数量: " << warningCount << std::endl;
        
        if (!errors.empty()) {
            oss << "\n错误列表:" << std::endl;
            for (const auto& error : errors) {
                oss << error.getFormattedMessage() << std::endl << std::endl;
            }
        }
        
        if (!warnings.empty()) {
            oss << "\n警告列表:" << std::endl;
            for (const auto& warning : warnings) {
                oss << warning.getFormattedMessage() << std::endl << std::endl;
            }
        }
        
        return oss.str();
    }
};

// 全局错误处理器实例
extern std::unique_ptr<ErrorHandler> g_errorHandler;

// 便捷函数
void reportError(const std::string& message, 
                const std::string& errorCode = "",
                const ErrorLocation& location = ErrorLocation());

void reportWarning(const std::string& message,
                  const std::string& errorCode = "",
                  const ErrorLocation& location = ErrorLocation());

void reportTypeError(const std::string& expected, 
                    const std::string& actual,
                    const ErrorLocation& location = ErrorLocation());

void reportUndefinedIdentifier(const std::string& identifier,
                              const ErrorLocation& location = ErrorLocation());

void reportFunctionCallError(const std::string& functionName,
                            const std::string& reason,
                            const ErrorLocation& location = ErrorLocation());

void reportDivisionByZero(const ErrorLocation& location = ErrorLocation());

void reportArrayIndexOutOfBounds(int index, int size,
                                const ErrorLocation& location = ErrorLocation());

} // namespace InterpreterCore

#endif // ERROR_HANDLER_H
