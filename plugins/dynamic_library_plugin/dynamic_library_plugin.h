#pragma once

#include "interpreter/plugins/builtin_plugin.h"
#include "dynamic_library_loader.h"
#include <memory>

namespace DynamicLoader {

// 动态库插件类，继承自BuiltinPlugin
class DynamicLibraryPlugin : public BuiltinPlugin {
public:
    DynamicLibraryPlugin(const std::string& config_file);
    DynamicLibraryPlugin(const std::string& library_path, const std::string& library_name);
    ~DynamicLibraryPlugin() override = default;
    
    // 实现BuiltinPlugin接口
    PluginInfo getPluginInfo() const override;
    std::vector<FunctionInfo> getFunctions() override;
    
    // 获取库加载器
    DynamicLibraryLoader* getLoader() const { return loader_.get(); }
    
    // 检查插件是否有效
    bool isValid() const;

private:
    std::unique_ptr<DynamicLibraryLoader> loader_;
    std::string library_name_;
    std::string description_;
    std::vector<std::string> function_names_;
    
    void initializePlugin();
};

} // namespace DynamicLoader
