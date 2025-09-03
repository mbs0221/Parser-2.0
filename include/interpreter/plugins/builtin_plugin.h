#ifndef BUILTIN_PLUGIN_H
#define BUILTIN_PLUGIN_H

#include "interpreter/values/value.h"
#include "interpreter/types/types.h"
#include "parser/definition.h"
#include "interpreter/scope/scope.h"
#include <vector>
#include <string>
#include <functional>
#include <map>

// using namespace std; // 已移除，使用显式std前缀

// ==================== 内置函数类型定义 ====================
// 内置函数指针类型定义
typedef Value* (*BuiltinFunctionPtr)(class Scope*);

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
    
    // 获取插件信息
    virtual PluginInfo getPluginInfo() const = 0;
    
    // 注册插件函数到作用域管理器（虚函数，子类必须实现）
    virtual void registerFunctions(ScopeManager& scopeManager) = 0;
};

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
    
    // 通过类型系统调用方法
    static Value* callMethodOnValue(Value* value, const string& methodName, vector<Value*>& args);
};

// 插件导出宏
#define EXPORT_PLUGIN(PluginClass) \
    extern "C" { \
        BuiltinPlugin* createPlugin() { return new PluginClass(); } \
        void destroyPlugin(BuiltinPlugin* plugin) { delete plugin; } \
    }

// ==================== 插件函数注册宏 ====================
// 统一的内置函数注册宏，使用函数原型来解析参数
#define REGISTER_BUILTIN_FUNCTION(scopeManager, name, func, description) \
    scopeManager.defineFunction(name, new BuiltinFunction(func, description))

#endif // BUILTIN_PLUGIN_H
