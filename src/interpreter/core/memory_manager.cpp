#include "interpreter/core/memory_manager.h"
#include "common/logger.h"
#include <iostream>
#include <sstream>
#include <algorithm>

namespace InterpreterCore {

// 静态成员初始化
std::unique_ptr<MemoryManager> MemoryManager::instance = nullptr;
std::mutex MemoryManager::instanceMutex;

// 全局内存池实例
ValuePool<Integer> g_intPool(1000);
ValuePool<Double> g_doublePool(1000);
ValuePool<Bool> g_boolPool(1000);
ValuePool<String> g_stringPool(1000);
ValuePool<Char> g_charPool(1000);

// 单例模式实现
MemoryManager& MemoryManager::getInstance() {
    std::lock_guard<std::mutex> lock(instanceMutex);
    if (!instance) {
        instance = std::make_unique<MemoryManager>();
    }
    return *instance;
}

void MemoryManager::destroyInstance() {
    std::lock_guard<std::mutex> lock(instanceMutex);
    instance.reset();
}

// 析构函数
MemoryManager::~MemoryManager() {
    if (enableLeakDetection && !trackedValues.empty()) {
        std::cerr << "警告: 检测到内存泄漏，还有 " << trackedValues.size() 
                  << " 个Value对象未被释放" << std::endl;
        
        // 输出泄漏的对象信息
        for (Value* value : trackedValues) {
            if (value) {
                std::cerr << "泄漏的对象: " << value->toString() 
                          << " (类型: " << value->getTypeName() << ")" << std::endl;
            }
        }
    }
    
    // 执行清理回调
    for (const auto& callback : cleanupCallbacks) {
        try {
            callback();
        } catch (...) {
            // 忽略清理回调中的异常
        }
    }
    
    // 清理所有跟踪的值
    cleanupAll();
}

// 内存跟踪
void MemoryManager::trackValue(Value* value) {
    if (!value) return;
    
    std::lock_guard<std::mutex> lock(valuesMutex);
    trackedValues.insert(value);
    totalAllocated.fetch_add(sizeof(*value));
    currentAllocated.fetch_add(sizeof(*value));
    allocationCount.fetch_add(1);
    
    LOG_DEBUG("跟踪Value对象: " + value->toString() + " (地址: " + 
              std::to_string(reinterpret_cast<uintptr_t>(value)) + ")");
}

void MemoryManager::untrackValue(Value* value) {
    if (!value) return;
    
    std::lock_guard<std::mutex> lock(valuesMutex);
    auto it = trackedValues.find(value);
    if (it != trackedValues.end()) {
        trackedValues.erase(it);
        totalFreed.fetch_add(sizeof(*value));
        currentAllocated.fetch_sub(sizeof(*value));
        deallocationCount.fetch_add(1);
        
        LOG_DEBUG("取消跟踪Value对象: " + value->toString() + " (地址: " + 
                  std::to_string(reinterpret_cast<uintptr_t>(value)) + ")");
    }
}

bool MemoryManager::isTracked(Value* value) const {
    if (!value) return false;
    
    std::lock_guard<std::mutex> lock(valuesMutex);
    return trackedValues.find(value) != trackedValues.end();
}

// 获取所有跟踪的值
std::vector<Value*> MemoryManager::getTrackedValues() const {
    std::lock_guard<std::mutex> lock(valuesMutex);
    return std::vector<Value*>(trackedValues.begin(), trackedValues.end());
}

// 清理所有跟踪的值
void MemoryManager::cleanupAll() {
    std::lock_guard<std::mutex> lock(valuesMutex);
    
    if (enableLeakDetection && !trackedValues.empty()) {
        std::cerr << "清理 " << trackedValues.size() << " 个Value对象" << std::endl;
    }
    
    for (Value* value : trackedValues) {
        if (value) {
            delete value;
        }
    }
    trackedValues.clear();
    currentAllocated.store(0);
}

// 注册清理回调
void MemoryManager::registerCleanupCallback(std::function<void()> callback) {
    std::lock_guard<std::mutex> lock(valuesMutex);
    cleanupCallbacks.push_back(callback);
}

// 生成内存报告
std::string MemoryManager::generateMemoryReport() const {
    std::ostringstream oss;
    
    oss << "=== 内存使用报告 ===" << std::endl;
    oss << "总分配内存: " << totalAllocated.load() << " 字节" << std::endl;
    oss << "总释放内存: " << totalFreed.load() << " 字节" << std::endl;
    oss << "当前使用内存: " << currentAllocated.load() << " 字节" << std::endl;
    oss << "分配次数: " << allocationCount.load() << std::endl;
    oss << "释放次数: " << deallocationCount.load() << std::endl;
    oss << "当前跟踪对象数: " << trackedValues.size() << std::endl;
    
    if (currentAllocated.load() > 0) {
        oss << "内存使用率: " << std::fixed << std::setprecision(2)
            << (static_cast<double>(currentAllocated.load()) / totalAllocated.load() * 100.0) << "%" << std::endl;
    }
    
    // 按类型统计对象
    std::map<std::string, int> typeCount;
    {
        std::lock_guard<std::mutex> lock(valuesMutex);
        for (Value* value : trackedValues) {
            if (value) {
                typeCount[value->getTypeName()]++;
            }
        }
    }
    
    if (!typeCount.empty()) {
        oss << "\n按类型统计:" << std::endl;
        for (const auto& pair : typeCount) {
            oss << "  " << pair.first << ": " << pair.second << " 个对象" << std::endl;
        }
    }
    
    return oss.str();
}

// 检查内存泄漏
bool MemoryManager::checkForLeaks() const {
    std::lock_guard<std::mutex> lock(valuesMutex);
    
    if (trackedValues.empty()) {
        return false; // 没有泄漏
    }
    
    std::cerr << "检测到内存泄漏:" << std::endl;
    for (Value* value : trackedValues) {
        if (value) {
            std::cerr << "  - " << value->toString() 
                      << " (类型: " << value->getTypeName() << ")" << std::endl;
        }
    }
    
    return true; // 有泄漏
}

// 重置统计
void MemoryManager::resetStatistics() {
    std::lock_guard<std::mutex> lock(valuesMutex);
    totalAllocated.store(0);
    totalFreed.store(0);
    currentAllocated.store(0);
    allocationCount.store(0);
    deallocationCount.store(0);
}

} // namespace InterpreterCore
