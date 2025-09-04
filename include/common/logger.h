#ifndef LOGGER_H
#define LOGGER_H

#include <log4cpp/Category.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/PatternLayout.hh>
#include <log4cpp/Priority.hh>
#include <log4cpp/PropertyConfigurator.hh>
#include <string>
#include <fstream>
#include <sys/stat.h>

class Logger {
private:
    static log4cpp::Category* category;
    static bool initialized;
    static std::string currentLogFile;
    static size_t maxLogSize;  // 最大日志文件大小（字节）
    static int maxLogFiles;    // 最大保留的日志文件数量

    // 私有方法
    static bool shouldRotateLog();
    static void rotateLogFile();
    static void cleanupOldLogs();
    static size_t getFileSize(const std::string& filename);
    static std::string getCurrentTimestamp();

public:
    static void initialize(const std::string& logFile = "parser.log", 
                          size_t maxSize = 10 * 1024 * 1024,  // 默认10MB
                          int maxFiles = 5);  // 默认保留5个文件
    static void shutdown();
    
    // 日志级别方法
    static void trace(const std::string& message);
    static void debug(const std::string& message);
    static void info(const std::string& message);
    static void warn(const std::string& message);
    static void error(const std::string& message);
    static void fatal(const std::string& message);
    
    // 设置日志级别
    static void setLevel(log4cpp::Priority::Value level);
    
    // 获取category实例
    static log4cpp::Category* getCategory();
    
    // 手动触发日志轮转
    static void rotateLog();
};

// 便捷宏定义
#define LOG_TRACE(msg) Logger::trace("[" + std::string(__func__) + "] " + msg)
#define LOG_DEBUG(msg) Logger::debug("[" + std::string(__func__) + "] " + msg)
#define LOG_INFO(msg) Logger::info("[" + std::string(__func__) + "] " + msg)
#define LOG_WARN(msg) Logger::warn("[" + std::string(__func__) + "] " + msg)
#define LOG_ERROR(msg) Logger::error("[" + std::string(__func__) + "] " + msg)
#define LOG_FATAL(msg) Logger::fatal("[" + std::string(__func__) + "] " + msg)

#endif // LOGGER_H
