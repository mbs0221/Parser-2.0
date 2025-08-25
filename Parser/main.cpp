#include "parser.h"
#include "interpreter.h"
#include "logger.h"
#include <iostream>
#include <string>
#include <getopt.h>
#include <unistd.h>

using namespace std;

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
void setLogLevel(const string& level) {
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

// 测试
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
				// getopt已经打印了错误信息
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
	Logger::initialize("parser.log");
	setLogLevel(logLevel);
	
	LOG_INFO("Parser application started with log level: " + logLevel);
	
	LOG_INFO("Parsing file: " + fileName);
	Parser p;
	Program *program = p.parse(fileName);
	if (program != nullptr){
		LOG_INFO("Parsing successful, executing program");
		Interpreter interpreter;
		interpreter.execute(program);
		LOG_INFO("Program execution completed");
	} else {
		LOG_ERROR("Parsing failed");
		printf("解析失败\n");
	}
	
	Logger::shutdown();
	return 0;
}