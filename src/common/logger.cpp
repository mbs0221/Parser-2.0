#include "common/logger.h"
#include <iostream>

// 静态成员初始化
log4cpp::Category* Logger::category = nullptr;
bool Logger::initialized = false;

void Logger::initialize(const std::string& logFile) {
    if (initialized) return;
    
    try {
        // 创建文件输出器
        log4cpp::FileAppender* fileAppender = new log4cpp::FileAppender("fileAppender", logFile);
        
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
        
        info("Logger initialized successfully");
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

void Logger::debug(const std::string& message) {
    if (category) {
        category->debug(message);
    }
}

void Logger::info(const std::string& message) {
    if (category) {
        category->info(message);
    }
}

void Logger::warn(const std::string& message) {
    if (category) {
        category->warn(message);
    }
}

void Logger::error(const std::string& message) {
    if (category) {
        category->error(message);
    }
}

void Logger::fatal(const std::string& message) {
    if (category) {
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
