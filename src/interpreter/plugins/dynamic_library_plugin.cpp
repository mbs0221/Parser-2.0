#include "interpreter/plugins/dynamic_library_plugin.h"
#include "common/logger.h"

namespace DynamicLoader {

DynamicLibraryPlugin::DynamicLibraryPlugin(const std::string& config_file) {
    loader_ = std::make_unique<DynamicLibraryLoader>();
    
    if (!loader_->loadFromConfig(config_file)) {
        LOG_ERROR("无法从配置文件加载动态库: " + config_file);
        return;
    }
    
    initializePlugin();
}

DynamicLibraryPlugin::DynamicLibraryPlugin(const std::string& library_path, const std::string& library_name) {
    loader_ = std::make_unique<DynamicLibraryLoader>();
    
    if (!loader_->loadLibrary(library_path)) {
        LOG_ERROR("无法加载动态库: " + library_path);
        return;
    }
    
    library_name_ = library_name;
    description_ = "动态加载的 " + library_name + " 库插件";
    
    initializePlugin();
}

void DynamicLibraryPlugin::initializePlugin() {
    if (!loader_ || !loader_->isLoaded()) {
        return;
    }
    
    // 获取库信息
    library_name_ = loader_->getLibraryName();
    description_ = "动态加载的 " + library_name_ + " 库插件";
    
    // 获取函数列表
    auto functions = loader_->getFunctions();
    function_names_.clear();
    for (const auto& func : functions) {
        function_names_.push_back(func.name);
    }
    
    LOG_INFO("初始化动态库插件: " + library_name_ + " (函数数量: " + std::to_string(function_names_.size()) + ")");
}

PluginInfo DynamicLibraryPlugin::getPluginInfo() const {
    return {
        library_name_,
        "1.0.0",
        description_,
        function_names_
    };
}

std::vector<FunctionInfo> DynamicLibraryPlugin::getFunctions() {
    if (!loader_ || !loader_->isLoaded()) {
        return {};
    }
    
    return loader_->getFunctions();
}

bool DynamicLibraryPlugin::isValid() const {
    return loader_ && loader_->isLoaded() && !function_names_.empty();
}

} // namespace DynamicLoader
