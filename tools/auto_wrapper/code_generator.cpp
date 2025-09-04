#include "auto_wrapper.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace AutoWrapper {

CodeGenerator::CodeGenerator(const LibraryConfig& config) : config_(config) {
}

std::string CodeGenerator::generateHeader() {
    std::ostringstream oss;
    
    oss << "#pragma once\n\n";
    oss << "#include \"interpreter/plugins/builtin_plugin.h\"\n";
    oss << "#include \"interpreter/values/value.h\"\n";
    oss << "#include \"interpreter/scope/scope.h\"\n";
    oss << "#include \"common/logger.h\"\n\n";
    
    // 添加必要的头文件
    for (const auto& header : config_.header_files) {
        oss << "#include <" << header << ">\n";
    }
    oss << "\n";
    
    oss << "using namespace std;\n\n";
    
    // 前向声明
    oss << "// 前向声明\n";
    for (const auto& func : config_.functions) {
        oss << "Value* " << func.name << "_wrapper(Scope* scope);\n";
    }
    oss << "\n";
    
    // 插件类定义
    oss << "class " << config_.plugin_name << " : public BuiltinPlugin {\n";
    oss << "public:\n";
    oss << "    " << config_.plugin_name << "() = default;\n";
    oss << "    ~" << config_.plugin_name << "() override = default;\n\n";
    
    oss << "    PluginInfo getPluginInfo() const override {\n";
    oss << "        return {\n";
    oss << "            \"" << config_.library_name << "\",\n";
    oss << "            \"1.0.0\",\n";
    oss << "            \"" << config_.description << "\",\n";
    oss << "            {";
    
    for (size_t i = 0; i < config_.functions.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << "\"" << config_.functions[i].name << "\"";
    }
    oss << "}\n";
    oss << "        };\n";
    oss << "    }\n\n";
    
    oss << "    std::vector<FunctionInfo> getFunctions() override;\n";
    oss << "};\n\n";
    
    oss << "// 导出插件\n";
    oss << "EXPORT_PLUGIN(" << config_.plugin_name << ")\n";
    
    return oss.str();
}

std::string CodeGenerator::generateImplementation() {
    std::ostringstream oss;
    
    oss << "#include \"" << config_.plugin_name << ".h\"\n\n";
    
    // 生成包装函数
    for (const auto& func : config_.functions) {
        oss << generateFunctionWrapper(func) << "\n";
    }
    
    // 生成getFunctions方法
    oss << "std::vector<FunctionInfo> " << config_.plugin_name << "::getFunctions() {\n";
    oss << "    std::vector<FunctionInfo> functions;\n\n";
    
    for (const auto& func : config_.functions) {
        oss << "    functions.emplace_back(\"" << func.name << "\", \"" << func.c_signature << "\",\n";
        oss << "        std::vector<std::string>{";
        
        for (size_t i = 0; i < func.parameters.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << "\"" << func.parameters[i].name << "\"";
        }
        oss << "},\n";
        oss << "        [](Scope* scope) -> Value* {\n";
        oss << "            return " << func.name << "_wrapper(scope);\n";
        oss << "        });\n\n";
    }
    
    oss << "    return functions;\n";
    oss << "}\n";
    
    return oss.str();
}

std::string CodeGenerator::generateFunctionWrapper(const FunctionInfo& func) {
    std::ostringstream oss;
    
    oss << "// " << func.description << "\n";
    oss << "Value* " << func.name << "_wrapper(Scope* scope) {\n";
    oss << "    try {\n";
    
    // 参数提取和转换
    for (const auto& param : func.parameters) {
        oss << "        " << param.c_type << " " << param.name << " = ";
        
        if (param.c_type == "double") {
            oss << "scope->getArgument<Double>(\"" << param.name << "\")->getValue();\n";
        } else if (param.c_type == "int") {
            oss << "scope->getArgument<Integer>(\"" << param.name << "\")->getValue();\n";
        } else if (param.c_type == "char*") {
            oss << "const_cast<char*>(scope->getArgument<String>(\"" << param.name << "\")->getValue().c_str());\n";
        } else if (param.c_type == "const char*") {
            oss << "scope->getArgument<String>(\"" << param.name << "\")->getValue().c_str();\n";
        } else {
            oss << "/* TODO: 添加类型转换逻辑 */ 0;\n";
        }
    }
    
    oss << "\n";
    
    // 函数调用
    if (func.is_void_return) {
        oss << "        " << func.name << "(";
    } else {
        oss << "        " << func.return_c_type << " result = " << func.name << "(";
    }
    
    for (size_t i = 0; i < func.parameters.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << func.parameters[i].name;
    }
    oss << ");\n\n";
    
    // 返回值转换
    if (!func.is_void_return) {
        oss << generateReturnConversion(func, "result");
    } else {
        oss << "        return nullptr;\n";
    }
    
    oss << "    } catch (const std::exception& e) {\n";
    oss << "        LOG_ERROR(\"Error in " << func.name << ": \" + std::string(e.what()));\n";
    oss << "        return nullptr;\n";
    oss << "    }\n";
    oss << "}\n";
    
    return oss.str();
}

std::string CodeGenerator::generateReturnConversion(const FunctionInfo& func, const std::string& result_var) {
    std::ostringstream oss;
    
    if (func.return_c_type == "double") {
        oss << "        return new Double(" << result_var << ");\n";
    } else if (func.return_c_type == "int") {
        oss << "        return new Integer(" << result_var << ");\n";
    } else if (func.return_c_type == "char*" || func.return_c_type == "const char*") {
        oss << "        return new String(std::string(" << result_var << "));\n";
    } else if (func.return_c_type == "void") {
        oss << "        return nullptr;\n";
    } else {
        oss << "        // TODO: 添加返回类型转换逻辑\n";
        oss << "        return nullptr;\n";
    }
    
    return oss.str();
}

std::string CodeGenerator::generateCMakeLists() {
    std::ostringstream oss;
    
    oss << "# " << config_.plugin_name << " 插件的CMakeLists.txt\n";
    oss << "add_library(" << config_.plugin_name << " SHARED " << config_.plugin_name << ".cpp)\n";
    oss << "target_include_directories(" << config_.plugin_name << " PRIVATE ${CMAKE_SOURCE_DIR}/include)\n";
    oss << "target_link_libraries(" << config_.plugin_name << " parser interpreter)\n";
    
    return oss.str();
}

std::string CodeGenerator::generateReadme() {
    std::ostringstream oss;
    
    oss << "# " << config_.plugin_name << " 插件\n\n";
    oss << config_.description << "\n\n";
    oss << "## 功能\n\n";
    
    for (const auto& func : config_.functions) {
        oss << "- **" << func.name << "**: " << func.description << "\n";
        oss << "  - 签名: `" << func.c_signature << "`\n";
        if (!func.parameters.empty()) {
            oss << "  - 参数:\n";
            for (const auto& param : func.parameters) {
                oss << "    - `" << param.name << "` (" << param.c_type << "): " << param.description << "\n";
            }
        }
        oss << "  - 返回: `" << func.return_c_type << "`\n\n";
    }
    
    oss << "## 使用方法\n\n";
    oss << "```cpp\n";
    oss << "// 加载插件\n";
    oss << "PluginManager manager;\n";
    oss << "manager.loadPlugin(\"lib" << config_.plugin_name << ".so\");\n";
    oss << "manager.registerAllPlugins(scopeManager);\n";
    oss << "```\n\n";
    
    return oss.str();
}

bool CodeGenerator::generateAll() {
    try {
        // 创建输出目录
        std::filesystem::create_directories(config_.output_directory);
        
        // 生成头文件
        std::ofstream header_file(config_.output_directory + "/" + config_.plugin_name + ".h");
        header_file << generateHeader();
        header_file.close();
        
        // 生成实现文件
        std::ofstream impl_file(config_.output_directory + "/" + config_.plugin_name + ".cpp");
        impl_file << generateImplementation();
        impl_file.close();
        
        // 生成CMakeLists.txt
        std::ofstream cmake_file(config_.output_directory + "/CMakeLists.txt");
        cmake_file << generateCMakeLists();
        cmake_file.close();
        
        // 生成README.md
        std::ofstream readme_file(config_.output_directory + "/README.md");
        readme_file << generateReadme();
        readme_file.close();
        
        std::cout << "成功生成插件: " << config_.plugin_name << std::endl;
        std::cout << "输出目录: " << config_.output_directory << std::endl;
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "生成插件时出错: " << e.what() << std::endl;
        return false;
    }
}

} // namespace AutoWrapper
