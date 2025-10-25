#include "interpreter/plugins/builtin_plugin.h"
#include "common/logger.h"
#include "interpreter/values/value.h"
#include "interpreter/scope/scope.h"
// MethodReference classes are defined in value.h
#include "interpreter/types/types.h"
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
        registerPluginFunctions(pair.second, scopeManager);
    }
}

void PluginManager::registerPluginFunctions(BuiltinPlugin* plugin, ScopeManager& scopeManager) {
    if (!plugin) return;
    
    // 获取插件的函数列表
    std::vector<FunctionInfo> functions = plugin->getFunctions();
    
    // 注册每个函数到作用域管理器
    for (const FunctionInfo& funcInfo : functions) {
        // 创建BuiltinFunction对象
        BuiltinFunction* builtinFunc = new BuiltinFunction(funcInfo.name, funcInfo.function, funcInfo.parameterNames);
        
        // 注册到作用域管理器
        scopeManager.defineFunction(funcInfo.name, builtinFunc);
        
        LOG_DEBUG("Registered function: " + funcInfo.name + " from plugin " + plugin->getPluginInfo().name);
    }
    
    LOG_INFO("Registered " + std::to_string(functions.size()) + " functions from plugin " + plugin->getPluginInfo().name);
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

// ==================== PluginManager辅助方法实现 ====================
// 通过类型系统调用方法
Value* PluginManager::callMethodOnValue(Value* value, const string& methodName, vector<Value*>& args) {
    if (!value) return nullptr;
    
    LOG_DEBUG("PluginManager::callMethodOnValue called: " + methodName + " on type " + value->getBuiltinTypeName());
    
    // 直接调用类型的方法，而不是通过类型系统
    if (String* str = dynamic_cast<String*>(value)) {
        if (methodName == "length") {
            size_t len = str->length();
            return new Integer(static_cast<int>(len));
        } else if (methodName == "to_upper" || methodName == "toUpper") {
            return str->toUpperCase();
        } else if (methodName == "to_lower" || methodName == "toLower") {
            return str->toLowerCase();
        } else if (methodName == "substring" && args.size() >= 2) {
            if (Integer* start = dynamic_cast<Integer*>(args[0])) {
                if (Integer* length = dynamic_cast<Integer*>(args[1])) {
                    return str->substring(start->getValue(), start->getValue() + length->getValue());
                }
            }
        }
    } else if (Array* arr = dynamic_cast<Array*>(value)) {
        if (methodName == "length" || methodName == "size") {
            size_t size = arr->getSize();
            return new Integer(static_cast<int>(size));
        } else if (methodName == "push" && args.size() >= 1) {
            arr->addElement(args[0]);
            return new Bool(true);
        } else if (methodName == "pop") {
            if (arr->getSize() > 0) {
                Value* lastElement = arr->getElement(arr->getSize() - 1);
                arr->removeElement(arr->getSize() - 1);
                return lastElement;
            }
            return nullptr;
        }
    }
    
    LOG_ERROR("Method not found: " + methodName + " on type " + value->getBuiltinTypeName());
    return nullptr;
}
