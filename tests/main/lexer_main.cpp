#include "lexer/lexer.h"
#include "common/logger.h"
#include <iostream>
#include <string>
#include <getopt.h>
#include <unistd.h>
#include <cstdlib>

using namespace std;
using namespace lexer;

// 显示使用帮助
void showUsage(const char* programName) {
    printf("Usage: %s [OPTIONS] <file>\n", programName);
    printf("Options:\n");
    printf("  -l, --log-level LEVEL    Set log level (DEBUG, INFO, WARN, ERROR, FATAL)\n");
    printf("  -v, --verbose            Enable verbose output (same as --log-level DEBUG)\n");
    printf("  -q, --quiet              Enable quiet output (same as --log-level ERROR)\n");
    printf("  -h, --help               Show this help message\n");
    printf("\nExamples:\n");
    printf("  %s input.txt                    # Use default log level (INFO)\n", programName);
    printf("  %s -l DEBUG input.txt           # Enable debug logging\n", programName);
    printf("  %s --log-level WARN input.txt   # Only show warnings and errors\n", programName);
    printf("  %s -v input.txt                 # Verbose mode\n", programName);
    printf("  %s -q input.txt                 # Quiet mode\n", programName);
}

// 设置日志级别
void setLogLevel(const std::string& level) {
    if (level == "DEBUG") {
        Logger::setLevel(log4cpp::Priority::DEBUG);
    } else if (level == "INFO") {
        Logger::setLevel(log4cpp::Priority::INFO);
    } else if (level == "WARN") {
        Logger::setLevel(log4cpp::Priority::WARN);
    } else if (level == "ERROR") {
        Logger::setLevel(log4cpp::Priority::ERROR);
    } else if (level == "FATAL") {
        Logger::setLevel(log4cpp::Priority::FATAL);
    } else {
        printf("Warning: Unknown log level '%s', using INFO\n", level.c_str());
        Logger::setLevel(log4cpp::Priority::INFO);
    }
}

int main(int argc, char *argv[])
{
    string logLevel = "INFO";  // 默认日志级别
    string fileName;
    
    // 定义长选项
    static struct option long_options[] = {
        {"log-level", required_argument, 0, 'l'},
        {"verbose",   no_argument,       0, 'v'},
        {"quiet",     no_argument,       0, 'q'},
        {"help",      no_argument,       0, 'h'},
        {0, 0, 0, 0}
    };
    
    int option_index = 0;
    int c;
    
    // 解析命令行参数
    while ((c = getopt_long(argc, argv, "l:vqh", long_options, &option_index)) != -1) {
        switch (c) {
            case 'l':
                logLevel = optarg;
                break;
            case 'v':
                logLevel = "DEBUG";
                break;
            case 'q':
                logLevel = "ERROR";
                break;
            case 'h':
                showUsage(argv[0]);
                return 0;
            case '?':
                showUsage(argv[0]);
                return 1;
            default:
                showUsage(argv[0]);
                return 1;
        }
    }
    
    // 获取文件名（剩余的非选项参数）
    if (optind < argc) {
        fileName = argv[optind];
    } else {
        printf("Error: No input file specified\n");
        showUsage(argv[0]);
        return 1;
    }
    
    // 初始化日志系统
    Logger::initialize("logs/lexer.log", 5 * 1024 * 1024, 3);
    setLogLevel(logLevel);
    
    LOG_INFO("Lexer application started with log level: " + logLevel);
    LOG_INFO("Lexical analysis of file: " + fileName);
    
    try {
        // 创建词法分析器
        Lexer lexer;
        
        // 从文件加载
        if (!lexer.from_file(fileName)) {
            printf("无法打开文件: %s\n", fileName.c_str());
            return -1;
        }
        
        LOG_INFO("Lexical analysis started");
        printf("开始词法分析: %s\n", fileName.c_str());
        
        // 收集所有词法单元
        vector<Token*> tokens;
        Token* token;
        
        while ((token = lexer.scan()) != nullptr && token->Tag != END_OF_FILE) {
            tokens.push_back(token);
        }
        
        LOG_INFO("Lexical analysis completed successfully");
        printf("词法分析完成，共生成 %zu 个词法单元\n", tokens.size());
        
        // 输出词法单元信息
        printf("\n词法单元列表:\n");
        printf("%-4s %-15s %-20s %-10s\n", "行号", "类型", "值", "位置");
        printf("%-4s %-15s %-20s %-10s\n", "----", "----", "----", "----");
        
        for (size_t i = 0; i < tokens.size(); ++i) {
            Token* token = tokens[i];
            printf("%-4d %-15d %-20s %-10d\n", 
                   lexer.line, 
                   token->Tag,
                   token->str().c_str(),
                   lexer.column);
        }
        
        printf("\n词法分析统计:\n");
        printf("- 总词法单元数: %zu\n", tokens.size());
        
        // 统计各种类型的词法单元
        map<int, int> typeCount;
        for (const auto& token : tokens) {
            typeCount[token->Tag]++;
        }
        
        printf("- 词法单元类型分布:\n");
        for (const auto& pair : typeCount) {
            printf("  Tag %d: %d\n", pair.first, pair.second);
        }
        
    } catch (const std::exception& e) {
        LOG_ERROR("Lexical analysis failed: " + string(e.what()));
        printf("词法分析失败: %s\n", e.what());
        return -1;
    }
    
    Logger::shutdown();
    return 0;
}
