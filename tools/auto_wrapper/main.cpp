#include "auto_wrapper.h"
#include <iostream>
#include <string>

using namespace AutoWrapper;

int main(int argc, char* argv[]) {
    AutoWrapper wrapper;
    
    if (argc < 2) {
        wrapper.showHelp();
        return 1;
    }
    
    // 解析命令行参数
    std::string config_file;
    std::string output_dir;
    bool verbose = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            wrapper.showHelp();
            return 0;
        } else if (arg == "--config" || arg == "-c") {
            if (i + 1 < argc) {
                config_file = argv[++i];
            } else {
                std::cerr << "错误: --config 需要指定配置文件路径" << std::endl;
                return 1;
            }
        } else if (arg == "--output" || arg == "-o") {
            if (i + 1 < argc) {
                output_dir = argv[++i];
            } else {
                std::cerr << "错误: --output 需要指定输出目录" << std::endl;
                return 1;
            }
        } else if (arg == "--verbose" || arg == "-v") {
            verbose = true;
        } else if (config_file.empty()) {
            config_file = arg;
        }
    }
    
    if (config_file.empty()) {
        std::cerr << "错误: 请指定配置文件" << std::endl;
        wrapper.showHelp();
        return 1;
    }
    
    // 生成插件
    bool success = wrapper.generateFromConfig(config_file, output_dir);
    
    if (success) {
        std::cout << "插件生成成功！" << std::endl;
        return 0;
    } else {
        std::cerr << "插件生成失败！" << std::endl;
        return 1;
    }
}
