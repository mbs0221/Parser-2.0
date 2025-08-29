#include "interpreter/builtin_plugin.h"
#include <iostream>
#include <dlfcn.h>
#include <filesystem>

using namespace std;

// 插件导出函数类型定义
typedef BuiltinPlugin* (*CreatePluginFunc)();
typedef void (*DestroyPluginFunc)(BuiltinPlugin*);

PluginManager::~PluginManager() {
    // 卸载所有插件
    for (const auto& pair : plugins) {
        unloadPlugin(pair.first);
    }
}

bool PluginManager::loadPlugin(const string& pluginPath) {
    // 打开动态库
    void* handle = dlopen(pluginPath.c_str(), RTLD_LAZY);
    if (!handle) {
        cerr << "无法加载插件 " << pluginPath << ": " << dlerror() << endl;
        return false;
    }
    
    // 获取创建插件函数
    CreatePluginFunc createFunc = (CreatePluginFunc)dlsym(handle, "createPlugin");
    if (!createFunc) {
        cerr << "无法找到createPlugin函数: " << dlerror() << endl;
        dlclose(handle);
        return false;
    }
    
    // 创建插件实例
    BuiltinPlugin* plugin = createFunc();
    if (!plugin) {
        cerr << "创建插件实例失败" << endl;
        dlclose(handle);
        return false;
    }
    
    // 获取插件信息
    PluginInfo info = plugin->getPluginInfo();
    string pluginName = info.name;
    
    // 检查插件是否已加载
    if (plugins.find(pluginName) != plugins.end()) {
        cerr << "插件 " << pluginName << " 已加载" << endl;
        delete plugin;
        dlclose(handle);
        return false;
    }
    
    // 保存插件和句柄
    plugins[pluginName] = plugin;
    pluginHandles[pluginName] = handle;
    
    cout << "成功加载插件: " << pluginName << " v" << info.version << endl;
    cout << "描述: " << info.description << endl;
    cout << "函数: ";
    for (const string& func : info.functions) {
        cout << func << " ";
    }
    cout << endl;
    
    return true;
}

bool PluginManager::unloadPlugin(const string& pluginName) {
    auto pluginIt = plugins.find(pluginName);
    auto handleIt = pluginHandles.find(pluginName);
    
    if (pluginIt == plugins.end() || handleIt == pluginHandles.end()) {
        cerr << "插件 " << pluginName << " 未找到" << endl;
        return false;
    }
    
    // 获取销毁函数
    DestroyPluginFunc destroyFunc = (DestroyPluginFunc)dlsym(handleIt->second, "destroyPlugin");
    if (destroyFunc) {
        destroyFunc(pluginIt->second);
    } else {
        delete pluginIt->second;
    }
    
    // 关闭动态库
    dlclose(handleIt->second);
    
    // 从映射中移除
    plugins.erase(pluginIt);
    pluginHandles.erase(handleIt);
    
    cout << "成功卸载插件: " << pluginName << endl;
    return true;
}

void PluginManager::registerAllPlugins(ScopeManager& scopeManager) {
    for (const auto& pair : plugins) {
        pair.second->registerFunctions(scopeManager);
    }
}

vector<string> PluginManager::getLoadedPlugins() const {
    vector<string> pluginNames;
    for (const auto& pair : plugins) {
        pluginNames.push_back(pair.first);
    }
    return pluginNames;
}

bool PluginManager::isPluginLoaded(const string& pluginName) const {
    return plugins.find(pluginName) != plugins.end();
}

PluginInfo PluginManager::getPluginInfo(const string& pluginName) const {
    auto it = plugins.find(pluginName);
    if (it != plugins.end()) {
        return it->second->getPluginInfo();
    }
    return PluginInfo{"", "", "", {}};
}
