#include "interpreter/core/interpreter.h"
#include "interpreter/utils/logger.h"
#include "interpreter/types/types.h"
#include "interpreter/values/value.h"
#include "interpreter/scope/scope.h"
#include "parser/parser.h"
#include <iostream>
#include <string>
#include <sstream>
#include <cstdio>
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
    printf("  -p, --plugins PLUGINS    Load specific plugins (comma-separated, default: basic)\n");
    printf("  -n, --no-plugins         Disable plugin loading\n");
    printf("  -h, --help               Show this help message\n");
    printf("\nExamples:\n");
    printf("  %s input.txt                    # Use default log level (INFO) and basic plugins\n", programName);
    printf("  %s -l DEBUG input.txt           # Enable debug logging\n", programName);
    printf("  %s --log-level WARN input.txt   # Only show warnings and errors\n", programName);
    printf("  %s -v input.txt                 # Verbose mode\n", programName);
    printf("  %s -q input.txt                 # Quiet mode\n", programName);
    printf("  %s -p basic,math input.txt      # Load basic and math plugins\n", programName);
    printf("  %s -n input.txt                 # Run without plugins\n", programName);
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

// 测试
int main(int argc, char *argv[])
{
	string logLevel = "INFO";  // 默认日志级别
	string fileName;
	bool loadPlugins = true;   // 默认加载插件
	string pluginList = ""; // 默认不加载插件
	
	// 定义长选项
	static struct option long_options[] = {
		{"log-level", required_argument, 0, 'l'},
		{"verbose",   no_argument,       0, 'v'},
		{"quiet",     no_argument,       0, 'q'},
		{"plugins",   required_argument, 0, 'p'},
		{"no-plugins", no_argument,      0, 'n'},
		{"help",      no_argument,       0, 'h'},
		{0, 0, 0, 0}
	};
	
	int option_index = 0;
	int c;
	
	// 解析命令行参数
	while ((c = getopt_long(argc, argv, "l:vqnp:h", long_options, &option_index)) != -1) {
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
			case 'p':
				pluginList = optarg;
				loadPlugins = true;
				break;
			case 'n':
				loadPlugins = false;
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
	LOG_INFO("Plugin loading: " + string(loadPlugins ? "enabled" : "disabled"));
	if (loadPlugins) {
		LOG_INFO("Plugins to load: " + pluginList);
	}
	
	// 初始化内置类型
	LOG_INFO("Initializing built-in types...");
	TypeRegistry::initializeBuiltinTypes();
	
	// 验证内置类型注册
	LOG_INFO("Verifying built-in type registration...");
	TypeRegistry* globalRegistry = TypeRegistry::getGlobalInstance();
	if (globalRegistry) {
		LOG_INFO("Global TypeRegistry found with " + to_string(globalRegistry->getTypeCount()) + " types");
		
		// 检查关键类型
		ObjectType* stringType = globalRegistry->getType("string");
		ObjectType* intType = globalRegistry->getType("int");
		ObjectType* boolType = globalRegistry->getType("bool");
		
		if (stringType) {
			LOG_INFO("✓ String type registered: " + stringType->getTypeName());
		} else {
			LOG_ERROR("✗ String type NOT found in registry!");
		}
		
		if (intType) {
			LOG_INFO("✓ Int type registered: " + intType->getTypeName());
		} else {
			LOG_ERROR("✗ Int type NOT found in registry!");
		}
		
		if (boolType) {
			LOG_INFO("✓ Bool type registered: " + boolType->getTypeName());
		} else {
			LOG_ERROR("✗ Bool type NOT found in registry!");
		}
	} else {
		LOG_ERROR("✗ Global TypeRegistry is null!");
	}
	
	LOG_INFO("Parsing file: " + fileName);
	Parser p;
	Program *program = p.parse(fileName);
	if (program != nullptr){
		LOG_INFO("Parsing successful, executing program");
		
		// 暂时不使用优化器
		// TypeCastOptimizer optimizer;
		// Program* optimizedProgram = optimizer.optimize(program);
		
		LOG_INFO("Executing program");
		// 解释器在构造函数中已经自动加载了插件，这里不需要重复加载
		Interpreter interpreter(loadPlugins);
		
		interpreter.visit(program);  // 直接调用visit方法，消除execute函数依赖
		LOG_INFO("Program execution completed");
	} else {
		LOG_ERROR("Parsing failed");
		printf("解析失败\n");
	}
	
	Logger::shutdown();
	return 0;
}