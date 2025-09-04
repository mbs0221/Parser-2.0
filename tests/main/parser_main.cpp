#include "parser/parser.h"
#include "parser/statement.h"
#include "parser/expression.h"
#include "common/logger.h"
#include "interpreter/types/types.h"
#include <iostream>
#include <string>
#include <getopt.h>
#include <unistd.h>
#include <cstdlib>

using namespace std;

// 显示使用帮助
void showUsage(const char* programName) {
    printf("Usage: %s [OPTIONS] <file>\n", programName);
    printf("Options:\n");
    printf("  -l, --log-level LEVEL    Set log level (DEBUG, INFO, WARN, ERROR, FATAL)\n");
    printf("  -v, --verbose            Enable verbose output (same as --log-level DEBUG)\n");
    printf("  -q, --quiet              Enable quiet output (same as --log-level ERROR)\n");
    printf("  -a, --ast                Show AST structure\n");
    printf("  -h, --help               Show this help message\n");
    printf("\nExamples:\n");
    printf("  %s input.txt                    # Use default log level (INFO)\n", programName);
    printf("  %s -l DEBUG input.txt           # Enable debug logging\n", programName);
    printf("  %s --log-level WARN input.txt   # Only show warnings and errors\n", programName);
    printf("  %s -v input.txt                 # Verbose mode\n", programName);
    printf("  %s -q input.txt                 # Quiet mode\n", programName);
    printf("  %s -a input.txt                 # Show AST structure\n", programName);
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

// 递归打印AST节点
void printAST(Statement* stmt, int depth = 0) {
    if (!stmt) return;
    
    // 打印缩进
    for (int i = 0; i < depth; ++i) {
        printf("  ");
    }
    
    // 打印节点类型
    printf("- %s", typeid(*stmt).name());
    
    // 打印更多信息
    if (FunctionDefinition* func = dynamic_cast<FunctionDefinition*>(stmt)) {
        printf(" (name: %s)", func->name.c_str());
    } else if (ClassDefinition* cls = dynamic_cast<ClassDefinition*>(stmt)) {
        printf(" (name: %s)", cls->name.c_str());
    } else if (VariableDefinition* var = dynamic_cast<VariableDefinition*>(stmt)) {
        printf(" (variables: %zu)", var->getVariableCount());
    }
    
    printf("\n");
}

// 递归打印表达式节点
void printExpression(Expression* expr, int depth = 0) {
    if (!expr) return;
    
    // 打印缩进
    for (int i = 0; i < depth; ++i) {
        printf("  ");
    }
    
    // 打印节点类型
    printf("- %s", typeid(*expr).name());
    
    // 打印更多信息
    if (VariableExpression* var = dynamic_cast<VariableExpression*>(expr)) {
        printf(" (name: %s)", var->name.c_str());
    } else if (ConstantExpression<string>* strLit = dynamic_cast<ConstantExpression<string>*>(expr)) {
        printf(" (value: %s)", strLit->value.c_str());
    } else if (ConstantExpression<int>* intLit = dynamic_cast<ConstantExpression<int>*>(expr)) {
        printf(" (value: %d)", intLit->value);
    } else if (BinaryExpression* bin = dynamic_cast<BinaryExpression*>(expr)) {
        printf(" (binary op)");
    } else if (UnaryExpression* un = dynamic_cast<UnaryExpression*>(expr)) {
        printf(" (unary op)");
    }
    
    printf("\n");
}

int main(int argc, char *argv[])
{
    string logLevel = "INFO";  // 默认日志级别
    string fileName;
    bool showAST = false;
    
    // 定义长选项
    static struct option long_options[] = {
        {"log-level", required_argument, 0, 'l'},
        {"verbose",   no_argument,       0, 'v'},
        {"quiet",     no_argument,       0, 'q'},
        {"ast",       no_argument,       0, 'a'},
        {"help",      no_argument,       0, 'h'},
        {0, 0, 0, 0}
    };
    
    int option_index = 0;
    int c;
    
    // 解析命令行参数
    while ((c = getopt_long(argc, argv, "l:vqah", long_options, &option_index)) != -1) {
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
            case 'a':
                showAST = true;
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
    Logger::initialize("parser.log", 5 * 1024 * 1024, 3);
    setLogLevel(logLevel);
    
    LOG_INFO("Parser application started with log level: " + logLevel);
    LOG_INFO("Parsing file: " + fileName);
    
    // 初始化内置类型（解析器需要类型信息）
    LOG_INFO("Initializing built-in types...");
    TypeRegistry::initializeBuiltinTypes();
    
    try {
        // 创建解析器
        Parser parser;
        
        // 进行词法分析和语法分析
        Program* program = parser.parse(fileName);
        
        if (program != nullptr) {
            LOG_INFO("Parsing completed successfully");
            printf("语法分析完成，成功生成AST\n");
            
            // 统计AST信息
            vector<Statement*> statements = program->statements;
            printf("\n解析统计:\n");
            printf("- 语句总数: %zu\n", statements.size());
            
            // 统计各种类型的语句
            map<string, int> stmtTypeCount;
            for (const auto& stmt : statements) {
                stmtTypeCount[typeid(*stmt).name()]++;
            }
            
            printf("- 语句类型分布:\n");
            for (const auto& pair : stmtTypeCount) {
                printf("  %s: %d\n", pair.first.c_str(), pair.second);
            }
            
            // 如果需要，显示AST结构
            if (showAST) {
                printf("\nAST结构:\n");
                for (const auto& stmt : statements) {
                    printAST(stmt);
                }
            }
            
            printf("\n语法分析成功完成！\n");
            
        } else {
            LOG_ERROR("Parsing failed");
            printf("语法分析失败\n");
            return -1;
        }
        
    } catch (const std::exception& e) {
        LOG_ERROR("Parsing failed: " + string(e.what()));
        printf("解析失败: %s\n", e.what());
        return -1;
    }
    
    Logger::shutdown();
    return 0;
}
