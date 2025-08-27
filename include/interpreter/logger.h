#ifndef LOGGER_H
#define LOGGER_H

#include <log4cpp/Category.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/PatternLayout.hh>
#include <log4cpp/Priority.hh>
#include <log4cpp/PropertyConfigurator.hh>
#include <string>

class Logger {
private:
    static log4cpp::Category* category;
    static bool initialized;

public:
    static void initialize(const std::string& logFile = "parser.log");
    static void shutdown();
    
    // 日志级别方法
    static void debug(const std::string& message);
    static void info(const std::string& message);
    static void warn(const std::string& message);
    static void error(const std::string& message);
    static void fatal(const std::string& message);
    
    // 设置日志级别
    static void setLevel(log4cpp::Priority::Value level);
    
    // 获取category实例
    static log4cpp::Category* getCategory();
};

// 便捷宏定义
#define LOG_DEBUG(msg) Logger::debug(msg)
#define LOG_INFO(msg) Logger::info(msg)
#define LOG_WARN(msg) Logger::warn(msg)
#define LOG_ERROR(msg) Logger::error(msg)
#define LOG_FATAL(msg) Logger::fatal(msg)

#endif // LOGGER_H
