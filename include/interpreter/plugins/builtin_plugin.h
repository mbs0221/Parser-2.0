#ifndef BUILTIN_PLUGIN_H
#define BUILTIN_PLUGIN_H

#include "interpreter/values/value.h"
#include "parser/definition.h"
#include "interpreter/scope/scope.h"
#include "interpreter/values/value.h"
#include <vector>
#include <string>
#include <functional>
#include <map>

// using namespace std; // 已移除，使用显式std前缀

// ==================== 内置函数类型定义 ====================
// 内置函数指针类型定义
typedef Value* (*BuiltinFunctionPtr)(std::vector<Value*>&);

// 函数信息三元组：<函数指针, 参数列表, 描述>
typedef std::tuple<BuiltinFunctionPtr, std::vector<std::string>, std::string> FunctionInfo;

// 插件信息结构
struct PluginInfo {
    std::string name;
    std::string version;
    std::string description;
    std::vector<std::string> functions;
};

// 插件接口类
class BuiltinPlugin {
public:
    virtual ~BuiltinPlugin() = default;
    
    // 获取已创建的函数对象映射
    virtual std::map<std::string, BuiltinFunction*> getFunctionObjectMap() const;
    
    // 获取插件信息
    virtual PluginInfo getPluginInfo() const = 0;
    
    // 注册插件函数到作用域管理器
    virtual void registerFunctions(ScopeManager& scopeManager);
    
    // 辅助方法：注册单个内置函数
    void defineBuiltinFunction(ScopeManager& scopeManager, const std::string& name, BuiltinFunctionPtr func);
    
    // 辅助方法：批量注册内置函数
    void defineBuiltinFunctions(ScopeManager& scopeManager, const std::map<std::string, BuiltinFunctionPtr>& functions);

protected:
    // 子类需要实现的函数映射方法 - 使用三元组简化注册
    virtual std::map<std::string, FunctionInfo> getFunctionInfoMap() const = 0;
    
    // 为了向后兼容，保留原有方法（可选实现）
    virtual std::map<std::string, BuiltinFunctionPtr> getFunctionMap() const;
    virtual std::map<std::string, std::vector<std::string>> getFunctionParameters() const;
};

// ==================== 自动插件函数生成宏 ====================

// 自动生成无参数的插件函数（通过类型系统调用方法）
#define AUTO_PLUGIN_FUNCTION_0(funcName, methodName) \
    Value* funcName(vector<Value*>& args) { \
        if (args.size() != 1 || !args[0]) return nullptr; \
        Value* val = args[0]; \
        ObjectType* type = val->getValueType(); \
        if (!type) return nullptr; \
        vector<Value*> methodArgs; \
        return type->callMethod(val, methodName, methodArgs); \
    }

// 自动生成单参数的插件函数（通过类型系统调用方法）
#define AUTO_PLUGIN_FUNCTION_1(funcName, methodName) \
    Value* funcName(vector<Value*>& args) { \
        if (args.size() != 2 || !args[0] || !args[1]) return nullptr; \
        Value* val = args[0]; \
        ObjectType* type = val->getValueType(); \
        if (!type) return nullptr; \
        vector<Value*> methodArgs = {args[1]}; \
        return type->callMethod(val, methodName, methodArgs); \
    }

// 自动生成双参数的插件函数（通过类型系统调用方法）
#define AUTO_PLUGIN_FUNCTION_2(funcName, methodName) \
    Value* funcName(vector<Value*>& args) { \
        if (args.size() != 3 || !args[0] || !args[1] || !args[2]) return nullptr; \
        Value* val = args[0]; \
        ObjectType* type = val->getValueType(); \
        if (!type) return nullptr; \
        vector<Value*> methodArgs = {args[1], args[2]}; \
        return type->callMethod(val, methodName, methodArgs); \
    }

// 自动生成可变参数的插件函数（通过类型系统调用方法）
#define AUTO_PLUGIN_FUNCTION_VAR(funcName, methodName) \
    Value* funcName(vector<Value*>& args) { \
        if (args.size() < 1 || !args[0]) return nullptr; \
        Value* val = args[0]; \
        ObjectType* type = val->getValueType(); \
        if (!type) return nullptr; \
        vector<Value*> methodArgs(args.begin() + 1, args.end()); \
        return type->callMethod(val, methodName, methodArgs); \
    }

// 自动生成类型转换插件函数（通过类型系统调用convertTo方法）
#define AUTO_PLUGIN_CONVERT_FUNCTION(funcName) \
    Value* funcName(vector<Value*>& args) { \
        if (args.size() != 1 || !args[0]) return nullptr; \
        Value* val = args[0]; \
        ObjectType* type = val->getValueType(); \
        if (!type) return nullptr; \
        vector<Value*> methodArgs; \
        return type->callMethod(val, "convertTo", methodArgs); \
    }

// 自动生成数学函数插件函数（通过类型系统调用数学方法）
#define AUTO_PLUGIN_MATH_FUNCTION(funcName, methodName) \
    AUTO_PLUGIN_FUNCTION_0(funcName, methodName)

// 自动生成字符串函数插件函数（通过类型系统调用字符串方法）
#define AUTO_PLUGIN_STRING_FUNCTION(funcName, methodName) \
    AUTO_PLUGIN_FUNCTION_VAR(funcName, methodName)

// 自动生成数组函数插件函数（通过类型系统调用数组方法）
#define AUTO_PLUGIN_ARRAY_FUNCTION(funcName, methodName) \
    AUTO_PLUGIN_FUNCTION_VAR(funcName, methodName)

// 自动生成类型函数插件函数（通过类型系统调用类型方法）
#define AUTO_PLUGIN_TYPE_FUNCTION(funcName, methodName) \
    AUTO_PLUGIN_FUNCTION_0(funcName, methodName)

// 插件管理器类
class PluginManager {
private:
    map<string, BuiltinPlugin*> plugins;
    map<string, void*> pluginHandles;
    
public:
    PluginManager() = default;
    ~PluginManager();
    
    bool loadPlugin(const string& pluginPath);
    bool unloadPlugin(const string& pluginName);
    void registerAllPlugins(ScopeManager& scopeManager);
    vector<string> getLoadedPlugins() const;
    bool isPluginLoaded(const string& pluginName) const;
    PluginInfo getPluginInfo(const string& pluginName) const;
};

// 插件导出宏
#define EXPORT_PLUGIN(PluginClass) \
    extern "C" { \
        BuiltinPlugin* createPlugin() { return new PluginClass(); } \
        void destroyPlugin(BuiltinPlugin* plugin) { delete plugin; } \
    }

// 简化插件开发的宏
#define REGISTER_BUILTIN_FUNCTION(scopeManager, funcName, funcPtr) \
    scopeManager.defineFunction(funcName, new BuiltinFunction(funcName, funcPtr))

#define REGISTER_BUILTIN_FUNCTIONS(scopeManager, ...) \
    do { \
        std::map<std::string, BuiltinFunctionPtr> funcMap = {__VA_ARGS__}; \
        for (const auto& pair : funcMap) { \
            scopeManager.defineFunction(pair.first, new BuiltinFunction(pair.first, pair.second)); \
        } \
    } while(0)

#endif // BUILTIN_PLUGIN_H
