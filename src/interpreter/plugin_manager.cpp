#include "interpreter/builtin_plugin.h"
#include "interpreter/logger.h"
#include <iostream>
#include <dlfcn.h>
#include <filesystem>

using namespace std;

// 插件导出函数类型定义
typedef BuiltinPlugin* (*CreatePluginFunc)();
typedef void (*DestroyPluginFunc)(BuiltinPlugin*);

PluginManager::~PluginManager() {
    // 卸载所有插件
    // 创建插件名称的副本，避免在遍历时修改容器
    vector<string> pluginNames;
    for (const auto& pair : plugins) {
        pluginNames.push_back(pair.first);
    }
    
    // 使用副本进行卸载
    for (const string& pluginName : pluginNames) {
        unloadPlugin(pluginName);
    }
}

bool PluginManager::loadPlugin(const string& pluginPath) {
    // 打开动态库
    void* handle = dlopen(pluginPath.c_str(), RTLD_LAZY);
    if (!handle) {
        LOG_ERROR("无法加载插件 " + pluginPath + ": " + string(dlerror()));
        return false;
    }
    
    // 获取创建插件函数
    CreatePluginFunc createFunc = (CreatePluginFunc)dlsym(handle, "createPlugin");
    if (!createFunc) {
        LOG_ERROR("无法找到createPlugin函数: " + string(dlerror()));
        dlclose(handle);
        return false;
    }
    
    // 创建插件实例
    BuiltinPlugin* plugin = createFunc();
    if (!plugin) {
        LOG_ERROR("创建插件实例失败");
        dlclose(handle);
        return false;
    }
    
    // 获取插件信息
    PluginInfo info = plugin->getPluginInfo();
    string pluginName = info.name;
    
    // 检查插件是否已加载
    if (plugins.find(pluginName) != plugins.end()) {
        LOG_ERROR("插件 " + pluginName + " 已加载");
        delete plugin;
        dlclose(handle);
        return false;
    }
    
    // 保存插件和句柄
    plugins[pluginName] = plugin;
    pluginHandles[pluginName] = handle;
    
    LOG_INFO("成功加载插件: " + pluginName + " v" + info.version);
    LOG_INFO("描述: " + info.description);
    LOG_INFO("函数: ");
    for (const string& func : info.functions) {
        LOG_INFO(func);
    }
    
    return true;
}

bool PluginManager::unloadPlugin(const string& pluginName) {
    auto pluginIt = plugins.find(pluginName);
    auto handleIt = pluginHandles.find(pluginName);
    
    if (pluginIt == plugins.end() || handleIt == pluginHandles.end()) {
        LOG_ERROR("插件 " + pluginName + " 未找到");
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
    
    LOG_INFO("成功卸载插件: " + pluginName);
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
