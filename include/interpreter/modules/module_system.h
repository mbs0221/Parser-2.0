#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <set>
#include <functional>
#include "interpreter/values/value.h"
#include "interpreter/scope/scope.h"

namespace ModuleSystem {

// 前向声明
class Module;
class ModuleLoader;
class NamespaceManager;

// 导入类型枚举
enum class ImportType {
    DEFAULT,        // import "module";
    NAMESPACE,      // import "module" as ns;
    SELECTIVE,      // import { func1, func2 } from "module";
    WILDCARD        // import * from "module";
};

// 导入项信息
struct ImportItem {
    std::string name;           // 导入的名称
    std::string alias;          // 别名（可选）
    bool isWildcard = false;    // 是否为通配符导入
    
    ImportItem(const std::string& n, const std::string& a = "") 
        : name(n), alias(a.empty() ? n : a) {}
};

// 导入语句信息
struct ImportInfo {
    std::string modulePath;     // 模块路径
    ImportType type;            // 导入类型
    std::string namespaceAlias; // 命名空间别名
    std::vector<ImportItem> items; // 导入项列表
    
    ImportInfo(const std::string& path, ImportType t = ImportType::DEFAULT)
        : modulePath(path), type(t) {}
};

// 模块信息结构
struct ModuleInfo {
    std::string name;           // 模块名称
    std::string path;           // 模块路径
    std::string version;        // 模块版本
    std::vector<std::string> dependencies; // 依赖列表
    std::set<std::string> exports; // 导出项列表
    bool isLoaded = false;      // 是否已加载
    bool isInitialized = false; // 是否已初始化
    
    ModuleInfo(const std::string& n, const std::string& p) 
        : name(n), path(p) {}
};

// 模块类
class Module {
public:
    Module(const ModuleInfo& info);
    ~Module();
    
    // 基本信息
    const std::string& getName() const { return info_.name; }
    const std::string& getPath() const { return info_.path; }
    const std::string& getVersion() const { return info_.version; }
    
    // 状态管理
    bool isLoaded() const { return info_.isLoaded; }
    bool isInitialized() const { return info_.isInitialized; }
    void setLoaded(bool loaded) { info_.isLoaded = loaded; }
    void setInitialized(bool initialized) { info_.isInitialized = initialized; }
    
    // 依赖管理
    const std::vector<std::string>& getDependencies() const { return info_.dependencies; }
    void addDependency(const std::string& dep) { info_.dependencies.push_back(dep); }
    
    // 导出管理
    const std::set<std::string>& getExports() const { return info_.exports; }
    void addExport(const std::string& name) { info_.exports.insert(name); }
    bool isExported(const std::string& name) const { 
        return info_.exports.find(name) != info_.exports.end(); 
    }
    
    // 作用域管理
    Scope* getModuleScope() { return moduleScope_; }
    void setModuleScope(Scope* scope) { moduleScope_ = scope; }
    
    // 程序管理
    class Program* getProgram() { return program_; }
    void setProgram(class Program* prog) { program_ = prog; }

private:
    ModuleInfo info_;
    Scope* moduleScope_ = nullptr;
    class Program* program_ = nullptr;
};

// 模块加载器类
class ModuleLoader {
public:
    ModuleLoader();
    ~ModuleLoader();
    
    // 模块加载
    std::shared_ptr<Module> loadModule(const std::string& modulePath);
    std::shared_ptr<Module> loadModule(const ImportInfo& importInfo);
    
    // 模块查找
    std::shared_ptr<Module> findModule(const std::string& name) const;
    std::vector<std::shared_ptr<Module>> getAllModules() const;
    
    // 依赖解析
    bool resolveDependencies(std::shared_ptr<Module> module);
    bool hasCircularDependency(const std::string& moduleName, 
                              const std::string& targetName) const;
    
    // 模块缓存
    void cacheModule(std::shared_ptr<Module> module);
    void clearCache();
    bool isCached(const std::string& moduleName) const;
    
    // 路径解析
    std::string resolveModulePath(const std::string& moduleName) const;
    void addSearchPath(const std::string& path);
    const std::vector<std::string>& getSearchPaths() const { return searchPaths_; }

private:
    std::map<std::string, std::shared_ptr<Module>> moduleCache_;
    std::vector<std::string> searchPaths_;
    std::set<std::string> loadingModules_; // 用于检测循环依赖
    
    // 辅助方法
    std::string findModuleFile(const std::string& moduleName) const;
    bool parseModuleInfo(std::shared_ptr<Module> module);
    void loadModuleDependencies(std::shared_ptr<Module> module);
};

// 命名空间管理器类
class NamespaceManager {
public:
    NamespaceManager();
    ~NamespaceManager();
    
    // 命名空间创建和管理
    bool createNamespace(const std::string& name, Scope* scope);
    bool removeNamespace(const std::string& name);
    Scope* getNamespace(const std::string& name) const;
    
    // 符号导入
    bool importSymbol(const std::string& symbolName, 
                     const std::string& fromNamespace,
                     const std::string& toNamespace = "",
                     const std::string& alias = "");
    
    bool importAllSymbols(const std::string& fromNamespace,
                         const std::string& toNamespace = "");
    
    // 符号查找
    Value* findSymbol(const std::string& symbolName, 
                     const std::string& namespaceName = "") const;
    
    // 命名空间列表
    std::vector<std::string> getNamespaceNames() const;
    bool hasNamespace(const std::string& name) const;

private:
    std::map<std::string, Scope*> namespaces_;
    Scope* globalScope_ = nullptr;
    
    // 辅助方法
    Scope* getTargetScope(const std::string& namespaceName) const;
    std::string resolveSymbolName(const std::string& symbolName, 
                                 const std::string& alias) const;
};

// 模块系统主类
class ModuleSystem {
public:
    ModuleSystem();
    ~ModuleSystem();
    
    // 初始化
    void initialize(ScopeManager* scopeManager);
    
    // 导入处理
    bool processImport(const ImportInfo& importInfo);
    bool processImport(const std::string& modulePath, ImportType type = ImportType::DEFAULT);
    
    // 模块管理
    std::shared_ptr<Module> getModule(const std::string& name) const;
    std::vector<std::shared_ptr<Module>> getAllModules() const;
    
    // 命名空间管理
    NamespaceManager* getNamespaceManager() { return namespaceManager_.get(); }
    
    // 模块加载器
    ModuleLoader* getModuleLoader() { return moduleLoader_.get(); }
    
    // 作用域管理
    ScopeManager* getScopeManager() { return scopeManager_; }
    
    // 配置
    void addSearchPath(const std::string& path);
    void setDefaultSearchPaths();

private:
    std::unique_ptr<ModuleLoader> moduleLoader_;
    std::unique_ptr<NamespaceManager> namespaceManager_;
    ScopeManager* scopeManager_ = nullptr;
    
    // 辅助方法
    bool handleDefaultImport(const ImportInfo& importInfo);
    bool handleNamespaceImport(const ImportInfo& importInfo);
    bool handleSelectiveImport(const ImportInfo& importInfo);
    bool handleWildcardImport(const ImportInfo& importInfo);
};

} // namespace ModuleSystem
