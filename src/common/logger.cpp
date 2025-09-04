#include "common/logger.h"
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <unistd.h>
#include <cstdio>
#include <sstream>
#include <iomanip>
#include <ctime>

// 静态成员初始化
log4cpp::Category* Logger::category = nullptr;
bool Logger::initialized = false;
std::string Logger::currentLogFile = "";
size_t Logger::maxLogSize = 10 * 1024 * 1024;  // 默认10MB
int Logger::maxLogFiles = 5;  // 默认保留5个文件

void Logger::initialize(const std::string& logFile, size_t maxSize, int maxFiles) {
    if (initialized) return;
    
    try {
        currentLogFile = logFile;
        maxLogSize = maxSize;
        maxLogFiles = maxFiles;
        
        // 检查是否需要轮转日志
        if (shouldRotateLog()) {
            rotateLogFile();
        }
        
        // 创建文件输出器
        log4cpp::FileAppender* fileAppender = new log4cpp::FileAppender("fileAppender", currentLogFile);
        
        // 创建布局
        log4cpp::PatternLayout* fileLayout = new log4cpp::PatternLayout();
        fileLayout->setConversionPattern("%d [%p] %c: %m%n");
        
        // 设置布局
        fileAppender->setLayout(fileLayout);
        
        // 获取根分类
        category = &log4cpp::Category::getRoot();
        
        // 只添加文件输出器
        category->addAppender(fileAppender);
        
        // 设置默认级别
        category->setPriority(log4cpp::Priority::DEBUG);
        
        initialized = true;
        
        info("Logger initialized successfully with log rotation (max size: " + 
             std::to_string(maxSize / 1024 / 1024) + "MB, max files: " + std::to_string(maxFiles) + ")");
    } catch (const std::exception& e) {
        std::cerr << "Failed to initialize logger: " << e.what() << std::endl;
    }
}

void Logger::shutdown() {
    if (initialized && category) {
        info("Logger shutting down");
        log4cpp::Category::shutdown();
        initialized = false;
        category = nullptr;
    }
}

void Logger::trace(const std::string& message) {
    if (category) {
        // 检查是否需要轮转日志
        if (shouldRotateLog()) {
            rotateLogFile();
        }
        category->debug(message);  // log4cpp没有trace级别，使用debug级别
    }
}

void Logger::debug(const std::string& message) {
    if (category) {
        // 检查是否需要轮转日志
        if (shouldRotateLog()) {
            rotateLogFile();
        }
        category->debug(message);
    }
}

void Logger::info(const std::string& message) {
    if (category) {
        // 检查是否需要轮转日志
        if (shouldRotateLog()) {
            rotateLogFile();
        }
        category->info(message);
    }
}

void Logger::warn(const std::string& message) {
    if (category) {
        // 检查是否需要轮转日志
        if (shouldRotateLog()) {
            rotateLogFile();
        }
        category->warn(message);
    }
}

void Logger::error(const std::string& message) {
    if (category) {
        // 检查是否需要轮转日志
        if (shouldRotateLog()) {
            rotateLogFile();
        }
        category->error(message);
    }
}

void Logger::fatal(const std::string& message) {
    if (category) {
        // 检查是否需要轮转日志
        if (shouldRotateLog()) {
            rotateLogFile();
        }
        category->fatal(message);
    }
}

void Logger::setLevel(log4cpp::Priority::Value level) {
    if (category) {
        category->setPriority(level);
    }
}

log4cpp::Category* Logger::getCategory() {
    return category;
}

// 检查是否需要轮转日志
bool Logger::shouldRotateLog() {
    if (currentLogFile.empty()) return false;
    
    size_t fileSize = getFileSize(currentLogFile);
    return fileSize >= maxLogSize;
}

// 获取文件大小
size_t Logger::getFileSize(const std::string& filename) {
    struct stat st;
    if (stat(filename.c_str(), &st) == 0) {
        return st.st_size;
    }
    return 0;
}

// 轮转日志文件
void Logger::rotateLogFile() {
    if (currentLogFile.empty()) return;
    
    try {
        // 关闭当前的日志系统
        if (category) {
            log4cpp::Category::shutdown();
            category = nullptr;
        }
        
        // 清理旧日志文件
        cleanupOldLogs();
        
        // 重命名当前日志文件
        std::string timestamp = Logger::getCurrentTimestamp();
        std::string rotatedFile = currentLogFile + "." + timestamp;
        
        if (rename(currentLogFile.c_str(), rotatedFile.c_str()) != 0) {
            std::cerr << "Failed to rotate log file: " << currentLogFile << std::endl;
        }
        
        // 重新初始化日志系统
        log4cpp::FileAppender* fileAppender = new log4cpp::FileAppender("fileAppender", currentLogFile);
        log4cpp::PatternLayout* fileLayout = new log4cpp::PatternLayout();
        fileLayout->setConversionPattern("%d [%p] %c: %m%n");
        fileAppender->setLayout(fileLayout);
        
        category = &log4cpp::Category::getRoot();
        category->addAppender(fileAppender);
        category->setPriority(log4cpp::Priority::DEBUG);
        
        info("Log file rotated to: " + rotatedFile);
        
    } catch (const std::exception& e) {
        std::cerr << "Failed to rotate log file: " << e.what() << std::endl;
    }
}

// 清理旧日志文件
void Logger::cleanupOldLogs() {
    if (currentLogFile.empty()) return;
    
    try {
        // 查找所有相关的日志文件
        std::string baseName = currentLogFile;
        std::string dir = baseName.substr(0, baseName.find_last_of("/\\"));
        if (dir.empty()) dir = ".";
        
        std::string fileName = baseName.substr(baseName.find_last_of("/\\") + 1);
        
        // 这里简化实现，实际项目中可以使用更复杂的文件查找逻辑
        // 删除超过 maxLogFiles 数量的旧文件
        for (int i = maxLogFiles; i < maxLogFiles + 10; ++i) {
            std::string oldFile = baseName + "." + std::to_string(i);
            if (access(oldFile.c_str(), F_OK) == 0) {
                if (remove(oldFile.c_str()) == 0) {
                    std::cout << "Removed old log file: " << oldFile << std::endl;
                }
            }
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Failed to cleanup old logs: " << e.what() << std::endl;
    }
}

// 获取当前时间戳
std::string Logger::getCurrentTimestamp() {
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);
    
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y%m%d_%H%M%S");
    return oss.str();
}

// 手动触发日志轮转
void Logger::rotateLog() {
    if (initialized) {
        rotateLogFile();
    }
}
