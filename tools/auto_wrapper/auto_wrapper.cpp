#include "auto_wrapper.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

namespace AutoWrapper {

AutoWrapper::AutoWrapper() {
}

bool AutoWrapper::generateFromConfig(const std::string& config_file, const std::string& output_dir) {
    try {
        // 解析配置文件
        LibraryConfig config = ConfigParser::parseFromFile(config_file);
        
        // 如果指定了输出目录，覆盖配置中的目录
        if (!output_dir.empty()) {
            config.output_directory = output_dir;
        }
        
        if (verbose_) {
            std::cout << "解析配置文件: " << config_file << std::endl;
            std::cout << "库名称: " << config.library_name << std::endl;
            std::cout << "插件名称: " << config.plugin_name << std::endl;
            std::cout << "函数数量: " << config.functions.size() << std::endl;
            std::cout << "输出目录: " << config.output_directory << std::endl;
        }
        
        // 生成代码
        CodeGenerator generator(config);
        return generator.generateAll();
        
    } catch (const std::exception& e) {
        std::cerr << "生成插件时出错: " << e.what() << std::endl;
        return false;
    }
}

bool AutoWrapper::generateFromArgs(int argc, char* argv[]) {
    // 这个方法已经在main.cpp中实现了
    return true;
}

void AutoWrapper::showHelp() {
    std::cout << "C库自动化封装工具\n\n";
    std::cout << "用法:\n";
    std::cout << "  auto_wrapper [选项] <配置文件>\n\n";
    std::cout << "选项:\n";
    std::cout << "  -c, --config <文件>    指定配置文件路径\n";
    std::cout << "  -o, --output <目录>    指定输出目录\n";
    std::cout << "  -v, --verbose          显示详细信息\n";
    std::cout << "  -h, --help             显示帮助信息\n\n";
    std::cout << "示例:\n";
    std::cout << "  auto_wrapper math_config.json\n";
    std::cout << "  auto_wrapper --config string_config.json --output plugins/string_auto\n";
    std::cout << "  auto_wrapper -c math_config.json -o plugins/math_auto -v\n\n";
    std::cout << "配置文件格式:\n";
    std::cout << "  请参考 examples/ 目录下的示例配置文件\n";
}

LibraryConfig ConfigParser::parseFromFile(const std::string& config_file) {
    std::string content = readFile(config_file);
    return parseFromJson(content);
}

LibraryConfig ConfigParser::parseFromJson(const std::string& json_content) {
    // 这里应该使用JSON解析库，为了简化，我们使用简单的字符串解析
    // 在实际项目中，建议使用 nlohmann/json 或其他JSON库
    
    LibraryConfig config;
    
    // 简单的JSON解析（仅用于演示，实际应该使用专业的JSON库）
    std::istringstream iss(json_content);
    std::string line;
    
    while (std::getline(iss, line)) {
        // 移除空白字符
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);
        
        // 解析基本字段
        if (line.find("\"library_name\"") != std::string::npos) {
            size_t start = line.find(": \"") + 3;
            size_t end = line.find("\"", start);
            if (start != std::string::npos && end != std::string::npos) {
                config.library_name = line.substr(start, end - start);
            }
        } else if (line.find("\"plugin_name\"") != std::string::npos) {
            size_t start = line.find(": \"") + 3;
            size_t end = line.find("\"", start);
            if (start != std::string::npos && end != std::string::npos) {
                config.plugin_name = line.substr(start, end - start);
            }
        } else if (line.find("\"description\"") != std::string::npos) {
            size_t start = line.find(": \"") + 3;
            size_t end = line.find("\"", start);
            if (start != std::string::npos && end != std::string::npos) {
                config.description = line.substr(start, end - start);
            }
        } else if (line.find("\"output_directory\"") != std::string::npos) {
            size_t start = line.find(": \"") + 3;
            size_t end = line.find("\"", start);
            if (start != std::string::npos && end != std::string::npos) {
                config.output_directory = line.substr(start, end - start);
            }
        }
    }
    
    // 设置默认值
    if (config.plugin_name.empty()) {
        config.plugin_name = config.library_name + "AutoPlugin";
    }
    if (config.description.empty()) {
        config.description = "自动生成的" + config.library_name + "库插件";
    }
    if (config.output_directory.empty()) {
        config.output_directory = "plugins/" + config.library_name + "_auto_plugin";
    }
    
    // 添加默认头文件
    config.header_files = {"math.h", "stdlib.h", "string.h"};
    
    // 添加默认函数（这里应该从JSON中解析，为了简化使用默认值）
    if (config.library_name == "math") {
        config.functions = {
            {
                "sin", "double sin(double x)", "计算正弦值",
                {{"x", "double", "Double", "角度值（弧度）"}},
                "double", "Double", false
            },
            {
                "cos", "double cos(double x)", "计算余弦值", 
                {{"x", "double", "Double", "角度值（弧度）"}},
                "double", "Double", false
            },
            {
                "sqrt", "double sqrt(double x)", "计算平方根",
                {{"x", "double", "Double", "非负数"}},
                "double", "Double", false
            }
        };
    }
    
    // 设置默认类型映射
    config.type_mappings = {
        {"double", "Double"},
        {"int", "Integer"},
        {"size_t", "Integer"},
        {"char*", "String"},
        {"const char*", "String"},
        {"void", "null"}
    };
    
    return config;
}

std::string ConfigParser::readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("无法打开配置文件: " + filename);
    }
    
    std::ostringstream oss;
    oss << file.rdbuf();
    return oss.str();
}

} // namespace AutoWrapper
