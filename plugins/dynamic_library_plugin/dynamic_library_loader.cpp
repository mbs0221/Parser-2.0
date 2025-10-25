#include "dynamic_library_loader.h"
#include "interpreter/plugins/builtin_plugin.h"
#include "common/logger.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

namespace DynamicLoader {

DynamicLibraryLoader::DynamicLibraryLoader() {
}

DynamicLibraryLoader::~DynamicLibraryLoader() {
    unloadLibrary();
}

bool DynamicLibraryLoader::loadLibrary(const std::string& library_path) {
    if (isLoaded()) {
        LOG_WARN("库已加载，先卸载当前库");
        unloadLibrary();
    }
    
    // 打开动态库
    library_handle_ = dlopen(library_path.c_str(), RTLD_LAZY);
    if (!library_handle_) {
        LOG_ERROR("无法加载库 " + library_path + ": " + std::string(dlerror()));
        return false;
    }
    
    config_.library_path = library_path;
    config_.library_name = std::filesystem::path(library_path).stem().string();
    
    LOG_INFO("成功加载库: " + library_path);
    return true;
}

bool DynamicLibraryLoader::loadFromConfig(const std::string& config_file) {
    if (!parseConfigFile(config_file)) {
        return false;
    }
    
    return loadLibrary(config_.library_path);
}

bool DynamicLibraryLoader::parseConfigFile(const std::string& config_file) {
    std::ifstream file(config_file);
    if (!file.is_open()) {
        LOG_ERROR("无法打开配置文件: " + config_file);
        return false;
    }
    
    std::ostringstream oss;
    oss << file.rdbuf();
    std::string content = oss.str();
    
    // 简单的JSON解析（实际项目中应使用专业JSON库）
    std::istringstream iss(content);
    std::string line;
    
    while (std::getline(iss, line)) {
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);
        
        // 解析基本字段
        if (line.find("\"library_name\"") != std::string::npos) {
            size_t start = line.find(": \"") + 3;
            size_t end = line.find("\"", start);
            if (start != std::string::npos && end != std::string::npos) {
                config_.library_name = line.substr(start, end - start);
            }
        } else if (line.find("\"library_path\"") != std::string::npos) {
            size_t start = line.find(": \"") + 3;
            size_t end = line.find("\"", start);
            if (start != std::string::npos && end != std::string::npos) {
                config_.library_path = line.substr(start, end - start);
            }
        } else if (line.find("\"description\"") != std::string::npos) {
            size_t start = line.find(": \"") + 3;
            size_t end = line.find("\"", start);
            if (start != std::string::npos && end != std::string::npos) {
                config_.description = line.substr(start, end - start);
            }
        }
    }
    
    // 设置默认函数列表（这里应该从JSON中解析）
    if (config_.library_name == "math" || config_.library_name.find("math") != std::string::npos) {
        config_.functions = {
            {"sin", "double sin(double x)", "计算正弦值", {{"x", FunctionType::DOUBLE, false, false, "角度值"}}, FunctionType::DOUBLE, nullptr, false},
            {"cos", "double cos(double x)", "计算余弦值", {{"x", FunctionType::DOUBLE, false, false, "角度值"}}, FunctionType::DOUBLE, nullptr, false},
            {"tan", "double tan(double x)", "计算正切值", {{"x", FunctionType::DOUBLE, false, false, "角度值"}}, FunctionType::DOUBLE, nullptr, false},
            {"sqrt", "double sqrt(double x)", "计算平方根", {{"x", FunctionType::DOUBLE, false, false, "非负数"}}, FunctionType::DOUBLE, nullptr, false},
            {"log", "double log(double x)", "计算自然对数", {{"x", FunctionType::DOUBLE, false, false, "正数"}}, FunctionType::DOUBLE, nullptr, false},
            {"floor", "double floor(double x)", "向下取整", {{"x", FunctionType::DOUBLE, false, false, "浮点数"}}, FunctionType::DOUBLE, nullptr, false},
            {"ceil", "double ceil(double x)", "向上取整", {{"x", FunctionType::DOUBLE, false, false, "浮点数"}}, FunctionType::DOUBLE, nullptr, false},
            {"abs", "int abs(int x)", "计算绝对值", {{"x", FunctionType::INT, false, false, "整数"}}, FunctionType::INT, nullptr, false}
        };
    } else if (config_.library_name == "string" || config_.library_name.find("string") != std::string::npos) {
        config_.functions = {
            {"strlen", "size_t strlen(const char* str)", "计算字符串长度", {{"str", FunctionType::STRING, true, true, "字符串"}}, FunctionType::INT, nullptr, false},
            {"strcmp", "int strcmp(const char* str1, const char* str2)", "比较字符串", 
             {{"str1", FunctionType::STRING, true, true, "字符串1"}, {"str2", FunctionType::STRING, true, true, "字符串2"}}, FunctionType::INT, nullptr, false}
        };
    }
    
    return true;
}

bool DynamicLibraryLoader::loadFunctionSymbols() {
    if (!isLoaded()) {
        LOG_ERROR("库未加载，无法加载函数符号");
        return false;
    }
    
    loaded_functions_.clear();
    
    for (auto& func_info : config_.functions) {
        void* symbol = getFunctionSymbol(func_info.name);
        if (symbol) {
            func_info.function_ptr = symbol;
            loaded_functions_.push_back(func_info);
            LOG_DEBUG("成功加载函数符号: " + func_info.name);
        } else {
            LOG_WARN("无法加载函数符号: " + func_info.name);
        }
    }
    
    LOG_INFO("成功加载 " + std::to_string(loaded_functions_.size()) + " 个函数符号");
    return !loaded_functions_.empty();
}

void* DynamicLibraryLoader::getFunctionSymbol(const std::string& function_name) {
    if (!library_handle_) {
        return nullptr;
    }
    
    void* symbol = dlsym(library_handle_, function_name.c_str());
    if (!symbol) {
        LOG_DEBUG("无法找到符号 " + function_name + ": " + std::string(dlerror()));
    }
    
    return symbol;
}

void DynamicLibraryLoader::registerFunctions(ScopeManager& scopeManager) {
    if (!loadFunctionSymbols()) {
        LOG_ERROR("无法加载函数符号");
        return;
    }
    
    for (const auto& func_info : loaded_functions_) {
        // 创建函数包装器
        auto wrapper = createFunctionWrapper(func_info);
        
        // 创建BuiltinFunction对象
        BuiltinFunction* builtin_func = new BuiltinFunction(
            func_info.name, 
            wrapper, 
            {func_info.name}
        );
        
        // 注册到作用域管理器
        scopeManager.defineFunction(func_info.name, builtin_func);
        
        LOG_DEBUG("注册函数: " + func_info.name);
    }
    
    LOG_INFO("成功注册 " + std::to_string(loaded_functions_.size()) + " 个函数");
}

std::function<Value*(Scope*)> DynamicLibraryLoader::createFunctionWrapper(const DynamicFunctionInfo& func_info) {
    return [this, func_info](Scope* scope) -> Value* {
        try {
            // 根据函数类型创建不同的包装器
            if (func_info.return_type == FunctionType::DOUBLE) {
                if (func_info.parameters.size() == 1 && func_info.parameters[0].type == FunctionType::DOUBLE) {
                    // 单参数double函数
                    auto func_ptr = reinterpret_cast<double(*)(double)>(func_info.function_ptr);
                    double arg = scope->getArgument<Double>(func_info.parameters[0].name)->getValue();
                    double result = func_ptr(arg);
                    return new Double(result);
                }
            } else if (func_info.return_type == FunctionType::INT) {
                if (func_info.parameters.size() == 1 && func_info.parameters[0].type == FunctionType::INT) {
                    // 单参数int函数
                    auto func_ptr = reinterpret_cast<int(*)(int)>(func_info.function_ptr);
                    int arg = scope->getArgument<Integer>(func_info.parameters[0].name)->getValue();
                    int result = func_ptr(arg);
                    return new Integer(result);
                } else if (func_info.parameters.size() == 2 && 
                          func_info.parameters[0].type == FunctionType::STRING && 
                          func_info.parameters[1].type == FunctionType::STRING) {
                    // 双参数字符串比较函数
                    auto func_ptr = reinterpret_cast<int(*)(const char*, const char*)>(func_info.function_ptr);
                    const char* arg1 = scope->getArgument<String>(func_info.parameters[0].name)->getValue().c_str();
                    const char* arg2 = scope->getArgument<String>(func_info.parameters[1].name)->getValue().c_str();
                    int result = func_ptr(arg1, arg2);
                    return new Integer(result);
                }
            } else if (func_info.return_type == FunctionType::STRING) {
                if (func_info.parameters.size() == 1 && func_info.parameters[0].type == FunctionType::STRING) {
                    // 单参数字符串函数
                    auto func_ptr = reinterpret_cast<size_t(*)(const char*)>(func_info.function_ptr);
                    const char* arg = scope->getArgument<String>(func_info.parameters[0].name)->getValue().c_str();
                    size_t result = func_ptr(arg);
                    return new Integer(static_cast<int>(result));
                }
            }
            
            LOG_ERROR("不支持的函数签名: " + func_info.c_signature);
            return nullptr;
            
        } catch (const std::exception& e) {
            LOG_ERROR("调用函数 " + func_info.name + " 时出错: " + std::string(e.what()));
            return nullptr;
        }
    };
}

std::vector<DynamicFunctionInfo> DynamicLibraryLoader::getFunctions() const {
    return loaded_functions_;
}

std::string DynamicLibraryLoader::getLibraryName() const {
    return config_.library_name;
}

void DynamicLibraryLoader::unloadLibrary() {
    if (library_handle_) {
        dlclose(library_handle_);
        library_handle_ = nullptr;
        loaded_functions_.clear();
        LOG_INFO("卸载库: " + config_.library_name);
    }
}

bool DynamicLibraryLoader::isLoaded() const {
    return library_handle_ != nullptr;
}

FunctionType DynamicLibraryLoader::parseType(const std::string& type_str) {
    if (type_str == "double") return FunctionType::DOUBLE;
    if (type_str == "int") return FunctionType::INT;
    if (type_str == "size_t") return FunctionType::INT;
    if (type_str == "char*" || type_str == "const char*") return FunctionType::STRING;
    if (type_str == "void") return FunctionType::VOID;
    return FunctionType::UNKNOWN;
}

std::string DynamicLibraryLoader::mapCTypeToInterpreterType(FunctionType type) {
    switch (type) {
        case FunctionType::DOUBLE: return "Double";
        case FunctionType::INT: return "Integer";
        case FunctionType::STRING: return "String";
        case FunctionType::VOID: return "null";
        default: return "Unknown";
    }
}

} // namespace DynamicLoader
