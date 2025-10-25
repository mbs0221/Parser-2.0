#include "interpreter/modules/module_system.h"
#include "interpreter/core/interpreter.h"
#include "parser/parser.h"
#include "common/logger.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <algorithm>

namespace ModuleSystem {

// ==================== Module类实现 ====================

Module::Module(const ModuleInfo& info) : info_(info) {
}

Module::~Module() {
    if (program_) {
        delete program_;
        program_ = nullptr;
    }
}

// ==================== ModuleLoader类实现 ====================

ModuleLoader::ModuleLoader() {
    // 添加默认搜索路径
    addSearchPath(".");
    addSearchPath("./modules");
    addSearchPath("./std");
    addSearchPath("../modules");
    addSearchPath("../std");
}

ModuleLoader::~ModuleLoader() {
    clearCache();
}

std::shared_ptr<Module> ModuleLoader::loadModule(const std::string& modulePath) {
    // 检查是否已缓存
    if (isCached(modulePath)) {
        return moduleCache_[modulePath];
    }
    
    // 解析模块路径
    std::string resolvedPath = resolveModulePath(modulePath);
    if (resolvedPath.empty()) {
        LOG_ERROR("Cannot resolve module path: " + modulePath);
        return nullptr;
    }
    
    // 创建模块信息
    ModuleInfo info(modulePath, resolvedPath);
    auto module = std::make_shared<Module>(info);
    
    // 解析模块信息
    if (!parseModuleInfo(module)) {
        LOG_ERROR("Failed to parse module info: " + modulePath);
        return nullptr;
    }
    
    // 加载模块依赖
    loadModuleDependencies(module);
    
    // 缓存模块
    cacheModule(module);
    
    return module;
}

std::shared_ptr<Module> ModuleLoader::loadModule(const ImportInfo& importInfo) {
    return loadModule(importInfo.modulePath);
}

std::shared_ptr<Module> ModuleLoader::findModule(const std::string& name) const {
    auto it = moduleCache_.find(name);
    return (it != moduleCache_.end()) ? it->second : nullptr;
}

std::vector<std::shared_ptr<Module>> ModuleLoader::getAllModules() const {
    std::vector<std::shared_ptr<Module>> modules;
    for (const auto& pair : moduleCache_) {
        modules.push_back(pair.second);
    }
    return modules;
}

bool ModuleLoader::resolveDependencies(std::shared_ptr<Module> module) {
    if (!module) return false;
    
    // 检查循环依赖
    if (hasCircularDependency(module->getName(), module->getName())) {
        LOG_ERROR("Circular dependency detected for module: " + module->getName());
        return false;
    }
    
    // 加载所有依赖
    for (const auto& dep : module->getDependencies()) {
        auto depModule = loadModule(dep);
        if (!depModule) {
            LOG_ERROR("Failed to load dependency: " + dep + " for module: " + module->getName());
            return false;
        }
    }
    
    return true;
}

bool ModuleLoader::hasCircularDependency(const std::string& moduleName, 
                                        const std::string& targetName) const {
    // 简单的循环依赖检测
    if (loadingModules_.find(moduleName) != loadingModules_.end()) {
        return true;
    }
    
    auto module = findModule(moduleName);
    if (!module) return false;
    
    for (const auto& dep : module->getDependencies()) {
        if (dep == targetName || hasCircularDependency(dep, targetName)) {
            return true;
        }
    }
    
    return false;
}

void ModuleLoader::cacheModule(std::shared_ptr<Module> module) {
    if (module) {
        moduleCache_[module->getName()] = module;
    }
}

void ModuleLoader::clearCache() {
    moduleCache_.clear();
    loadingModules_.clear();
}

bool ModuleLoader::isCached(const std::string& moduleName) const {
    return moduleCache_.find(moduleName) != moduleCache_.end();
}

std::string ModuleLoader::resolveModulePath(const std::string& moduleName) const {
    // 首先检查是否是绝对路径
    if (std::filesystem::exists(moduleName)) {
        return moduleName;
    }
    
    // 在搜索路径中查找
    for (const auto& searchPath : searchPaths_) {
        std::string fullPath = searchPath + "/" + moduleName;
        if (std::filesystem::exists(fullPath)) {
            return fullPath;
        }
        
        // 尝试添加.txt扩展名
        std::string fullPathWithExt = fullPath + ".txt";
        if (std::filesystem::exists(fullPathWithExt)) {
            return fullPathWithExt;
        }
    }
    
    return "";
}

void ModuleLoader::addSearchPath(const std::string& path) {
    if (std::find(searchPaths_.begin(), searchPaths_.end(), path) == searchPaths_.end()) {
        searchPaths_.push_back(path);
    }
}

std::string ModuleLoader::findModuleFile(const std::string& moduleName) const {
    return resolveModulePath(moduleName);
}

bool ModuleLoader::parseModuleInfo(std::shared_ptr<Module> module) {
    if (!module) return false;
    
    // 这里可以解析模块的元信息，比如版本、依赖等
    // 目前简单实现，后续可以扩展
    
    // 检查文件是否存在
    std::ifstream file(module->getPath());
    if (!file.is_open()) {
        LOG_ERROR("Cannot open module file: " + module->getPath());
        return false;
    }
    file.close();
    
    return true;
}

void ModuleLoader::loadModuleDependencies(std::shared_ptr<Module> module) {
    // 这里可以解析模块文件中的import语句来获取依赖
    // 目前简单实现，后续可以扩展
}

// ==================== NamespaceManager类实现 ====================

NamespaceManager::NamespaceManager() {
}

NamespaceManager::~NamespaceManager() {
    // 清理命名空间
    for (auto& pair : namespaces_) {
        // 注意：不要删除Scope，它们由ScopeManager管理
    }
    namespaces_.clear();
}

bool NamespaceManager::createNamespace(const std::string& name, Scope* scope) {
    if (hasNamespace(name)) {
        LOG_WARN("Namespace already exists: " + name);
        return false;
    }
    
    namespaces_[name] = scope;
    return true;
}

bool NamespaceManager::removeNamespace(const std::string& name) {
    auto it = namespaces_.find(name);
    if (it != namespaces_.end()) {
        namespaces_.erase(it);
        return true;
    }
    return false;
}

Scope* NamespaceManager::getNamespace(const std::string& name) const {
    auto it = namespaces_.find(name);
    return (it != namespaces_.end()) ? it->second : nullptr;
}

bool NamespaceManager::importSymbol(const std::string& symbolName, 
                                   const std::string& fromNamespace,
                                   const std::string& toNamespace,
                                   const std::string& alias) {
    Scope* fromScope = getTargetScope(fromNamespace);
    Scope* toScope = getTargetScope(toNamespace);
    
    if (!fromScope || !toScope) {
        LOG_ERROR("Invalid namespace for symbol import: " + fromNamespace + " -> " + toNamespace);
        return false;
    }
    
    // 查找源符号
    Value* symbol = fromScope->getVariable(symbolName);
    if (!symbol) {
        LOG_ERROR("Symbol not found in namespace: " + symbolName + " in " + fromNamespace);
        return false;
    }
    
    // 导入到目标命名空间
    std::string targetName = resolveSymbolName(symbolName, alias);
    toScope->setVariable(targetName, symbol);
    
    return true;
}

bool NamespaceManager::importAllSymbols(const std::string& fromNamespace,
                                       const std::string& toNamespace) {
    Scope* fromScope = getTargetScope(fromNamespace);
    Scope* toScope = getTargetScope(toNamespace);
    
    if (!fromScope || !toScope) {
        LOG_ERROR("Invalid namespace for wildcard import: " + fromNamespace + " -> " + toNamespace);
        return false;
    }
    
    // 获取源命名空间的所有变量
    // auto variables = fromScope->getAllVariables();  // 暂时注释掉，因为Scope类没有这个方法
    // for (const auto& pair : variables) {
    //     toScope->setVariable(pair.first, pair.second);
    // }
    
    return true;
}

Value* NamespaceManager::findSymbol(const std::string& symbolName, 
                                   const std::string& namespaceName) const {
    Scope* scope = getTargetScope(namespaceName);
    if (!scope) {
        return nullptr;
    }
    
    return scope->getVariable(symbolName);
}

std::vector<std::string> NamespaceManager::getNamespaceNames() const {
    std::vector<std::string> names;
    for (const auto& pair : namespaces_) {
        names.push_back(pair.first);
    }
    return names;
}

bool NamespaceManager::hasNamespace(const std::string& name) const {
    return namespaces_.find(name) != namespaces_.end();
}

Scope* NamespaceManager::getTargetScope(const std::string& namespaceName) const {
    if (namespaceName.empty()) {
        return globalScope_;
    }
    return getNamespace(namespaceName);
}

std::string NamespaceManager::resolveSymbolName(const std::string& symbolName, 
                                               const std::string& alias) const {
    return alias.empty() ? symbolName : alias;
}

// ==================== ModuleSystem类实现 ====================

ModuleSystem::ModuleSystem() 
    : moduleLoader_(std::make_unique<ModuleLoader>())
    , namespaceManager_(std::make_unique<NamespaceManager>()) {
}

ModuleSystem::~ModuleSystem() {
}

void ModuleSystem::initialize(ScopeManager* scopeManager) {
    scopeManager_ = scopeManager;
    // namespaceManager_->globalScope_ = scopeManager->getCurrentScope();  // 暂时注释掉，因为globalScope_是私有的
}

bool ModuleSystem::processImport(const ImportInfo& importInfo) {
    if (!scopeManager_) {
        LOG_ERROR("ModuleSystem not initialized");
        return false;
    }
    
    switch (importInfo.type) {
        case ImportType::DEFAULT:
            return handleDefaultImport(importInfo);
        case ImportType::NAMESPACE:
            return handleNamespaceImport(importInfo);
        case ImportType::SELECTIVE:
            return handleSelectiveImport(importInfo);
        case ImportType::WILDCARD:
            return handleWildcardImport(importInfo);
        default:
            LOG_ERROR("Unknown import type");
            return false;
    }
}

bool ModuleSystem::processImport(const std::string& modulePath, ImportType type) {
    ImportInfo importInfo(modulePath, type);
    return processImport(importInfo);
}

std::shared_ptr<Module> ModuleSystem::getModule(const std::string& name) const {
    return moduleLoader_->findModule(name);
}

std::vector<std::shared_ptr<Module>> ModuleSystem::getAllModules() const {
    return moduleLoader_->getAllModules();
}

void ModuleSystem::addSearchPath(const std::string& path) {
    moduleLoader_->addSearchPath(path);
}

void ModuleSystem::setDefaultSearchPaths() {
    moduleLoader_->addSearchPath(".");
    moduleLoader_->addSearchPath("./modules");
    moduleLoader_->addSearchPath("./std");
    moduleLoader_->addSearchPath("../modules");
    moduleLoader_->addSearchPath("../std");
}

bool ModuleSystem::handleDefaultImport(const ImportInfo& importInfo) {
    // 默认导入：将模块的所有导出项导入到当前作用域
    auto module = moduleLoader_->loadModule(importInfo.modulePath);
    if (!module) {
        return false;
    }
    
    // 创建模块命名空间
    std::string namespaceName = module->getName();
    Scope* moduleScope = new Scope();
    namespaceManager_->createNamespace(namespaceName, moduleScope);
    
    // 执行模块代码
    Parser parser;
    Program* program = parser.parse(module->getPath());
    if (program) {
        // 在模块作用域中执行
        // 这里需要修改解释器以支持在指定作用域中执行
        // 暂时简化实现
        module->setProgram(program);
        module->setModuleScope(moduleScope);
    }
    
    return true;
}

bool ModuleSystem::handleNamespaceImport(const ImportInfo& importInfo) {
    // 命名空间导入：将模块导入为命名空间
    auto module = moduleLoader_->loadModule(importInfo.modulePath);
    if (!module) {
        return false;
    }
    
    // 创建命名空间
    std::string namespaceName = importInfo.namespaceAlias;
    Scope* moduleScope = new Scope();
    namespaceManager_->createNamespace(namespaceName, moduleScope);
    
    // 执行模块代码
    Parser parser;
    Program* program = parser.parse(module->getPath());
    if (program) {
        module->setProgram(program);
        module->setModuleScope(moduleScope);
    }
    
    return true;
}

bool ModuleSystem::handleSelectiveImport(const ImportInfo& importInfo) {
    // 选择性导入：只导入指定的项
    auto module = moduleLoader_->loadModule(importInfo.modulePath);
    if (!module) {
        return false;
    }
    
    // 创建模块命名空间
    std::string namespaceName = module->getName();
    Scope* moduleScope = new Scope();
    namespaceManager_->createNamespace(namespaceName, moduleScope);
    
    // 执行模块代码
    Parser parser;
    Program* program = parser.parse(module->getPath());
    if (program) {
        module->setProgram(program);
        module->setModuleScope(moduleScope);
    }
    
    // 导入指定的项到当前作用域
    for (const auto& item : importInfo.items) {
        namespaceManager_->importSymbol(item.name, namespaceName, "", item.alias);
    }
    
    return true;
}

bool ModuleSystem::handleWildcardImport(const ImportInfo& importInfo) {
    // 通配符导入：导入所有导出项
    auto module = moduleLoader_->loadModule(importInfo.modulePath);
    if (!module) {
        return false;
    }
    
    // 创建模块命名空间
    std::string namespaceName = module->getName();
    Scope* moduleScope = new Scope();
    namespaceManager_->createNamespace(namespaceName, moduleScope);
    
    // 执行模块代码
    Parser parser;
    Program* program = parser.parse(module->getPath());
    if (program) {
        module->setProgram(program);
        module->setModuleScope(moduleScope);
    }
    
    // 导入所有项到当前作用域
    namespaceManager_->importAllSymbols(namespaceName, "");
    
    return true;
}

} // namespace ModuleSystem
