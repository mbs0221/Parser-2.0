#include "interpreter/core/interpreter.h"
#include "common/logger.h"
#include "common/string_utils.h"

#include <iostream>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>

using namespace std;

// 插件管理方法
void Interpreter::loadPlugin(const string& pluginPath) {
    if (pluginManager.loadPlugin(pluginPath)) {
        // 重新注册所有插件函数到作用域
        pluginManager.registerAllPlugins(scopeManager);
        LOG_INFO("插件加载成功: " + pluginPath);
    } else {
        LOG_ERROR("插件加载失败: " + pluginPath);
    }
}

void Interpreter::unloadPlugin(const string& pluginName) {
    if (pluginManager.unloadPlugin(pluginName)) {
        LOG_INFO("插件卸载成功: " + pluginName);
    } else {
        LOG_ERROR("插件卸载失败: " + pluginName);
    }
}

vector<string> Interpreter::getLoadedPlugins() const {
    return pluginManager.getLoadedPlugins();
}

// 自动加载默认插件
void Interpreter::loadDefaultPlugins(const string& pluginDir) {
    LOG_INFO("开始扫描插件目录: " + pluginDir);
    
    // 使用系统命令列出目录中的所有.so文件
    string command = "find " + pluginDir + " -name \"*.so\" 2>/dev/null";
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        LOG_ERROR("无法打开插件目录: " + pluginDir);
        return;
    }
    
    char buffer[1024];
    vector<string> pluginFiles;
    
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        string filePath = string(buffer);
        // 移除换行符
        if (!filePath.empty() && filePath[filePath.length()-1] == '\n') {
            filePath.erase(filePath.length()-1);
        }
        if (!filePath.empty()) {
            pluginFiles.push_back(filePath);
        }
    }
    
    pclose(pipe);
    
    if (pluginFiles.empty()) {
        LOG_WARN("插件目录中没有找到.so文件: " + pluginDir);
        return;
    }
    
    LOG_INFO("找到 " + to_string(pluginFiles.size()) + " 个插件文件");
    
    // 加载每个插件
    for (const string& pluginPath : pluginFiles) {
        LOG_DEBUG("尝试加载插件: " + pluginPath);
        
        if (pluginManager.loadPlugin(pluginPath)) {
            // 获取插件信息
            string pluginName = extractPluginName(pluginPath);
            PluginInfo info = pluginManager.getPluginInfo(pluginName);
            
            LOG_INFO("成功加载插件: " + info.name + " v" + info.version);
            LOG_DEBUG("插件描述: " + info.description);
            LOG_DEBUG("插件函数: " + interpreter::utils::join(info.functions, ", "));
        } else {
            LOG_ERROR("加载插件失败: " + pluginPath);
        }
    }
    
    LOG_INFO("插件加载完成，共加载 " + to_string(pluginManager.getLoadedPlugins().size()) + " 个插件");
}

// 重载版本，使用默认插件目录
void Interpreter::loadDefaultPlugins() {
    // 尝试多个可能的插件目录路径
    vector<string> possiblePaths = {
        "./plugins/",           // 当前目录
        "../plugins/",          // 上级目录
        "../../plugins/",       // 上上级目录
        "./build/plugins/"      // build子目录
    };
    
    for (const string& path : possiblePaths) {
        LOG_DEBUG("尝试插件路径: " + path);
        if (access((path + "libcore_plugin.so").c_str(), F_OK) == 0) {
            LOG_INFO("找到插件目录: " + path);
            loadDefaultPlugins(path);
            return;
        }
    }
    
    LOG_WARN("未找到有效的插件目录");
}

// 辅助函数：从文件路径提取插件名称
string Interpreter::extractPluginName(const string& filePath) {
    size_t lastSlash = filePath.find_last_of('/');
    if (lastSlash == string::npos) {
        lastSlash = -1;
    }
    
    string fileName = filePath.substr(lastSlash + 1);
    
    // 移除lib前缀和.so后缀
    if (fileName.substr(0, 3) == "lib") {
        fileName = fileName.substr(3);
    }
    if (fileName.length() > 3 && fileName.substr(fileName.length() - 3) == ".so") {
        fileName = fileName.substr(0, fileName.length() - 3);
    }
    
    return fileName;
}
