#include "interpreter/plugins/builtin_plugin.h"
#include "interpreter/utils/logger.h"
#include "interpreter/values/value.h"
#include "interpreter/scope/scope.h"
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

// ==================== BuiltinPlugin辅助方法实现 ====================

void BuiltinPlugin::defineBuiltinFunction(ScopeManager& scopeManager, const string& name, BuiltinFunctionPtr func) {
    if (func) {
        // 创建BuiltinFunction对象并注册到作用域
        BuiltinFunction* builtinFunc = new BuiltinFunction(name, func);
        scopeManager.defineFunction(name, builtinFunc);
    }
}

void BuiltinPlugin::defineBuiltinFunctions(ScopeManager& scopeManager, const map<string, BuiltinFunctionPtr>& functions) {
    // 获取参数信息
    map<string, vector<string>> paramMap = getFunctionParameters();
    
    for (const auto& pair : functions) {
        const string& name = pair.first;
        BuiltinFunctionPtr func = pair.second;
        
        // 查找参数信息
        auto paramIt = paramMap.find(name);
        if (paramIt != paramMap.end()) {
            // 创建带有参数信息的BuiltinFunction对象
            BuiltinFunction* builtinFunc = new BuiltinFunction(name, func, paramIt->second);
            scopeManager.defineFunction(name, builtinFunc);
        } else {
            // 创建没有参数信息的BuiltinFunction对象
            BuiltinFunction* builtinFunc = new BuiltinFunction(name, func);
            scopeManager.defineFunction(name, builtinFunc);
        }
    }
}

// 向后兼容的默认实现
map<string, BuiltinFunctionPtr> BuiltinPlugin::getFunctionMap() const {
    map<string, BuiltinFunctionPtr> result;
    map<string, FunctionInfo> funcInfoMap = getFunctionInfoMap();
    
    for (const auto& pair : funcInfoMap) {
        const string& funcName = pair.first;
        const FunctionInfo& funcInfo = pair.second;
        result[funcName] = std::get<0>(funcInfo); // 获取函数指针
    }
    return result;
}

// 向后兼容的默认实现
map<string, vector<string>> BuiltinPlugin::getFunctionParameters() const {
    map<string, vector<string>> result;
    map<string, FunctionInfo> funcInfoMap = getFunctionInfoMap();
    
    for (const auto& pair : funcInfoMap) {
        const string& funcName = pair.first;
        const FunctionInfo& funcInfo = pair.second;
        result[funcName] = std::get<1>(funcInfo); // 获取参数列表
    }
    return result;
}

// 默认实现：从getFunctionInfoMap构建BuiltinFunction对象映射
map<string, BuiltinFunction*> BuiltinPlugin::getFunctionObjectMap() const {
    map<string, BuiltinFunction*> result;
    // 调用子类实现的getFunctionInfoMap方法
    map<string, FunctionInfo> funcInfoMap = getFunctionInfoMap();
    
    for (const auto& pair : funcInfoMap) {
        const string& funcName = pair.first;
        const FunctionInfo& funcInfo = pair.second;
        
        BuiltinFunctionPtr func = std::get<0>(funcInfo);      // 函数指针
        const vector<string>& params = std::get<1>(funcInfo); // 参数列表
        const string& description = std::get<2>(funcInfo);     // 描述
        
        // 创建带有完整信息的BuiltinFunction对象
        result[funcName] = new BuiltinFunction(funcName, func, params);
    }
    return result;
}

// 默认实现：注册所有函数到作用域管理器
void BuiltinPlugin::registerFunctions(ScopeManager& scopeManager) {
    LOG_DEBUG("BuiltinPlugin::registerFunctions called");
    
    // 使用getFunctionObjectMap来获取带有参数信息的BuiltinFunction对象
    map<string, BuiltinFunction*> funcObjects = getFunctionObjectMap();
    
    LOG_DEBUG("getFunctionObjectMap returned " + to_string(funcObjects.size()) + " functions");
    
    for (const auto& pair : funcObjects) {
        const string& name = pair.first;
        BuiltinFunction* func = pair.second;
        LOG_DEBUG("Registering function '" + name + "' with " + to_string(func->getParameters().size()) + " parameters");
        scopeManager.defineFunction(name, func);
    }
}
