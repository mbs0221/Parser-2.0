#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <fstream>

namespace AutoWrapper {

// 参数信息结构
struct ParameterInfo {
    std::string name;
    std::string c_type;
    std::string interpreter_type;
    std::string description;
    bool is_pointer = false;
    bool is_array = false;
};

// 函数信息结构
struct FunctionInfo {
    std::string name;
    std::string c_signature;
    std::string description;
    std::vector<ParameterInfo> parameters;
    std::string return_c_type;
    std::string return_interpreter_type;
    bool is_void_return = false;
};

// 库配置信息
struct LibraryConfig {
    std::string library_name;
    std::string plugin_name;
    std::string description;
    std::vector<std::string> header_files;
    std::vector<FunctionInfo> functions;
    std::map<std::string, std::string> type_mappings;
    std::string output_directory;
};

// 代码生成器类
class CodeGenerator {
public:
    CodeGenerator(const LibraryConfig& config);
    
    // 生成插件头文件
    std::string generateHeader();
    
    // 生成插件实现文件
    std::string generateImplementation();
    
    // 生成CMakeLists.txt
    std::string generateCMakeLists();
    
    // 生成README.md
    std::string generateReadme();
    
    // 生成所有文件
    bool generateAll();

private:
    LibraryConfig config_;
    
    // 辅助方法
    std::string mapCTypeToInterpreterType(const std::string& c_type);
    std::string generateFunctionWrapper(const FunctionInfo& func);
    std::string generateParameterConversion(const ParameterInfo& param, const std::string& var_name);
    std::string generateReturnConversion(const FunctionInfo& func, const std::string& result_var);
    std::string escapeString(const std::string& str);
};

// 配置解析器类
class ConfigParser {
public:
    static LibraryConfig parseFromFile(const std::string& config_file);
    static LibraryConfig parseFromJson(const std::string& json_content);
    
private:
    static std::string readFile(const std::string& filename);
};

// 主工具类
class AutoWrapper {
public:
    AutoWrapper();
    
    // 从配置文件生成插件
    bool generateFromConfig(const std::string& config_file, const std::string& output_dir = "");
    
    // 从命令行参数生成插件
    bool generateFromArgs(int argc, char* argv[]);
    
    // 显示帮助信息
    void showHelp();

private:
    std::string output_directory_;
    bool verbose_ = false;
};

} // namespace AutoWrapper
