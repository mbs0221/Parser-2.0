#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <dlfcn.h>
#include "interpreter/values/value.h"
#include "interpreter/scope/scope.h"

namespace DynamicLoader {

// 函数类型枚举
enum class FunctionType {
    VOID,
    INT,
    DOUBLE,
    STRING,
    POINTER,
    UNKNOWN
};

// 参数信息
struct ParameterInfo {
    std::string name;
    FunctionType type;
    bool is_pointer = false;
    bool is_const = false;
    std::string description;
};

// 函数信息
struct DynamicFunctionInfo {
    std::string name;
    std::string c_signature;
    std::string description;
    std::vector<ParameterInfo> parameters;
    FunctionType return_type;
    void* function_ptr = nullptr;
    bool is_void_return = false;
};

// 库配置信息
struct LibraryConfig {
    std::string library_name;
    std::string library_path;
    std::string description;
    std::vector<std::string> header_files;
    std::vector<DynamicFunctionInfo> functions;
    std::map<std::string, std::string> type_mappings;
};

// 动态库加载器类
class DynamicLibraryLoader {
public:
    DynamicLibraryLoader();
    ~DynamicLibraryLoader();
    
    // 加载库
    bool loadLibrary(const std::string& library_path);
    
    // 从配置文件加载
    bool loadFromConfig(const std::string& config_file);
    
    // 注册函数到作用域管理器
    void registerFunctions(class ScopeManager& scopeManager);
    
    // 获取库信息
    std::vector<DynamicFunctionInfo> getFunctions() const;
    std::string getLibraryName() const;
    
    // 卸载库
    void unloadLibrary();
    
    // 检查库是否已加载
    bool isLoaded() const;
    
    // 函数包装器生成（公有方法，供插件使用）
    std::function<Value*(Scope*)> createFunctionWrapper(const DynamicFunctionInfo& func_info);

private:
    void* library_handle_ = nullptr;
    LibraryConfig config_;
    std::vector<DynamicFunctionInfo> loaded_functions_;
    
    // 辅助方法
    bool parseConfigFile(const std::string& config_file);
    bool loadFunctionSymbols();
    void* getFunctionSymbol(const std::string& function_name);
    FunctionType parseType(const std::string& type_str);
    std::string mapCTypeToInterpreterType(FunctionType type);
    
    
    // 类型转换函数
    template<typename T>
    T extractArgument(Scope* scope, const std::string& param_name, FunctionType type);
    
    template<typename T>
    Value* convertReturnValue(T value, FunctionType return_type);
};

// 全局函数包装器模板
template<typename RetType, typename... Args>
class FunctionWrapper {
public:
    using FunctionPtr = RetType(*)(Args...);
    
    FunctionWrapper(FunctionPtr func_ptr, const DynamicFunctionInfo& func_info)
        : func_ptr_(func_ptr), func_info_(func_info) {}
    
    Value* call(Scope* scope) {
        try {
            // 提取参数
            auto args = extractArguments<Args...>(scope);
            
            // 调用C函数
            if constexpr (std::is_void_v<RetType>) {
                std::apply(func_ptr_, args);
                return nullptr;
            } else {
                auto result = std::apply(func_ptr_, args);
                return convertReturnValue(result, func_info_.return_type);
            }
        } catch (const std::exception& e) {
            // 错误处理
            return nullptr;
        }
    }

private:
    FunctionPtr func_ptr_;
    DynamicFunctionInfo func_info_;
    
    template<typename... T>
    std::tuple<T...> extractArguments(Scope* scope) {
        // 参数提取逻辑
        return std::make_tuple();
    }
};

// 类型转换器
class TypeConverter {
public:
    // 从解释器类型转换为C类型
    template<typename T>
    static T fromInterpreterValue(Value* value, FunctionType target_type);
    
    // 从C类型转换为解释器类型
    template<typename T>
    static Value* toInterpreterValue(T value, FunctionType source_type);
    
    // 字符串到类型的转换
    static FunctionType stringToType(const std::string& type_str);
    static std::string typeToString(FunctionType type);
};

} // namespace DynamicLoader
