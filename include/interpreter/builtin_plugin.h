#ifndef BUILTIN_PLUGIN_H
#define BUILTIN_PLUGIN_H

#include "lexer/value.h"
#include "parser/function.h"
#include "interpreter/scope.h"
#include <vector>
#include <string>
#include <functional>
#include <map>

using namespace std;

// 插件函数类型定义
typedef Value* (*BuiltinFunctionPtr)(vector<Variable*>&);

// 插件信息结构
struct PluginInfo {
    string name;
    string version;
    string description;
    vector<string> functions;
};

// 插件接口类
class BuiltinPlugin {
protected:
    // 辅助方法：注册内置函数到作用域
    void defineBuiltinFunction(ScopeManager& scopeManager, const string& name, BuiltinFunctionPtr func) {
        scopeManager.defineIdentifier(name, new BuiltinFunction(name, func));
    }
    
    // 辅助方法：批量注册函数
    void defineBuiltinFunctions(ScopeManager& scopeManager, const map<string, BuiltinFunctionPtr>& functions) {
        for (const auto& pair : functions) {
            defineBuiltinFunction(scopeManager, pair.first, pair.second);
        }
    }

public:
    virtual ~BuiltinPlugin() = default;
    virtual PluginInfo getPluginInfo() const = 0;
    virtual void registerFunctions(ScopeManager& scopeManager) = 0;
    virtual map<string, BuiltinFunctionPtr> getFunctionMap() const = 0;
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
};

// 插件导出宏
#define EXPORT_PLUGIN(PluginClass) \
    extern "C" { \
        BuiltinPlugin* createPlugin() { return new PluginClass(); } \
        void destroyPlugin(BuiltinPlugin* plugin) { delete plugin; } \
    }

// 简化插件开发的宏
#define REGISTER_BUILTIN_FUNCTION(scopeManager, funcName, funcPtr) \
    scopeManager.defineIdentifier(funcName, new BuiltinFunction(funcName, funcPtr))

#define REGISTER_BUILTIN_FUNCTIONS(scopeManager, ...) \
    do { \
        std::map<std::string, BuiltinFunctionPtr> funcMap = {__VA_ARGS__}; \
        for (const auto& pair : funcMap) { \
            scopeManager.defineIdentifier(pair.first, new BuiltinFunction(pair.first, pair.second)); \
        } \
    } while(0)

#endif // BUILTIN_PLUGIN_H
