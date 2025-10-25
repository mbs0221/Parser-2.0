#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <memory>
#include <unordered_set>
#include <mutex>
#include <atomic>
#include <iostream>
#include <vector>
#include <functional>

#include "interpreter/values/value.h"

namespace InterpreterCore {

// 内存管理器类 - 用于跟踪和管理Value对象的内存
class MemoryManager {
private:
    static std::unique_ptr<MemoryManager> instance;
    static std::mutex instanceMutex;
    
    std::unordered_set<Value*> trackedValues;
    std::mutex valuesMutex;
    std::atomic<size_t> totalAllocated;
    std::atomic<size_t> totalFreed;
    std::atomic<size_t> currentAllocated;
    
    // 内存统计
    std::atomic<size_t> allocationCount;
    std::atomic<size_t> deallocationCount;
    
    // 内存泄漏检测
    bool enableLeakDetection;
    std::vector<std::function<void()>> cleanupCallbacks;
    
    MemoryManager() : totalAllocated(0), totalFreed(0), currentAllocated(0),
                     allocationCount(0), deallocationCount(0), enableLeakDetection(true) {}
    
public:
    // 单例模式
    static MemoryManager& getInstance();
    static void destroyInstance();
    
    // 禁用拷贝构造和赋值
    MemoryManager(const MemoryManager&) = delete;
    MemoryManager& operator=(const MemoryManager&) = delete;
    
    // 析构函数
    ~MemoryManager();
    
    // 内存跟踪
    void trackValue(Value* value);
    void untrackValue(Value* value);
    bool isTracked(Value* value) const;
    
    // 内存统计
    size_t getTotalAllocated() const { return totalAllocated.load(); }
    size_t getTotalFreed() const { return totalFreed.load(); }
    size_t getCurrentAllocated() const { return currentAllocated.load(); }
    size_t getAllocationCount() const { return allocationCount.load(); }
    size_t getDeallocationCount() const { return deallocationCount.load(); }
    
    // 内存泄漏检测
    void setLeakDetection(bool enable) { enableLeakDetection = enable; }
    bool isLeakDetectionEnabled() const { return enableLeakDetection; }
    
    // 获取所有跟踪的值
    std::vector<Value*> getTrackedValues() const;
    
    // 清理所有跟踪的值
    void cleanupAll();
    
    // 注册清理回调
    void registerCleanupCallback(std::function<void()> callback);
    
    // 生成内存报告
    std::string generateMemoryReport() const;
    
    // 检查内存泄漏
    bool checkForLeaks() const;
    
    // 重置统计
    void resetStatistics();
};

// 智能指针包装器 - 自动管理Value对象的内存
template<typename T>
class ValuePtr {
private:
    T* ptr;
    bool managed;
    
public:
    // 构造函数
    explicit ValuePtr(T* p = nullptr, bool manage = true) : ptr(p), managed(manage) {
        if (ptr && managed) {
            MemoryManager::getInstance().trackValue(ptr);
        }
    }
    
    // 拷贝构造函数
    ValuePtr(const ValuePtr& other) : ptr(other.ptr), managed(other.managed) {
        if (ptr && managed) {
            MemoryManager::getInstance().trackValue(ptr);
        }
    }
    
    // 移动构造函数
    ValuePtr(ValuePtr&& other) noexcept : ptr(other.ptr), managed(other.managed) {
        other.ptr = nullptr;
        other.managed = false;
    }
    
    // 析构函数
    ~ValuePtr() {
        if (ptr && managed) {
            MemoryManager::getInstance().untrackValue(ptr);
            delete ptr;
        }
    }
    
    // 赋值操作符
    ValuePtr& operator=(const ValuePtr& other) {
        if (this != &other) {
            if (ptr && managed) {
                MemoryManager::getInstance().untrackValue(ptr);
                delete ptr;
            }
            ptr = other.ptr;
            managed = other.managed;
            if (ptr && managed) {
                MemoryManager::getInstance().trackValue(ptr);
            }
        }
        return *this;
    }
    
    // 移动赋值操作符
    ValuePtr& operator=(ValuePtr&& other) noexcept {
        if (this != &other) {
            if (ptr && managed) {
                MemoryManager::getInstance().untrackValue(ptr);
                delete ptr;
            }
            ptr = other.ptr;
            managed = other.managed;
            other.ptr = nullptr;
            other.managed = false;
        }
        return *this;
    }
    
    // 解引用操作符
    T& operator*() const { return *ptr; }
    T* operator->() const { return ptr; }
    
    // 获取原始指针
    T* get() const { return ptr; }
    
    // 释放所有权
    T* release() {
        if (ptr && managed) {
            MemoryManager::getInstance().untrackValue(ptr);
        }
        T* oldPtr = ptr;
        ptr = nullptr;
        managed = false;
        return oldPtr;
    }
    
    // 重置指针
    void reset(T* p = nullptr) {
        if (ptr && managed) {
            MemoryManager::getInstance().untrackValue(ptr);
            delete ptr;
        }
        ptr = p;
        if (ptr && managed) {
            MemoryManager::getInstance().trackValue(ptr);
        }
    }
    
    // 检查是否为空
    bool isNull() const { return ptr == nullptr; }
    
    // 布尔转换
    explicit operator bool() const { return ptr != nullptr; }
};

// 便捷的类型别名
using IntPtr = ValuePtr<Integer>;
using DoublePtr = ValuePtr<Double>;
using BoolPtr = ValuePtr<Bool>;
using StringPtr = ValuePtr<String>;
using CharPtr = ValuePtr<Char>;

// 内存安全的Value创建函数
template<typename T, typename... Args>
ValuePtr<T> makeValue(Args&&... args) {
    return ValuePtr<T>(new T(std::forward<Args>(args)...), true);
}

// 内存安全的Value克隆函数
template<typename T>
ValuePtr<T> cloneValue(const T& value) {
    return ValuePtr<T>(value.clone(), true);
}

// RAII作用域管理器 - 自动清理作用域中的值
class ScopeValueManager {
private:
    std::vector<Value*> values;
    bool autoCleanup;
    
public:
    explicit ScopeValueManager(bool cleanup = true) : autoCleanup(cleanup) {}
    
    ~ScopeValueManager() {
        if (autoCleanup) {
            cleanup();
        }
    }
    
    // 添加值到管理
    void addValue(Value* value) {
        if (value) {
            values.push_back(value);
        }
    }
    
    // 手动清理
    void cleanup() {
        for (Value* value : values) {
            if (value) {
                MemoryManager::getInstance().untrackValue(value);
                delete value;
            }
        }
        values.clear();
    }
    
    // 获取管理的值数量
    size_t size() const { return values.size(); }
    
    // 检查是否为空
    bool empty() const { return values.empty(); }
};

// 内存池 - 用于高效分配和回收Value对象
template<typename T>
class ValuePool {
private:
    std::vector<T*> pool;
    std::mutex poolMutex;
    size_t maxSize;
    
public:
    explicit ValuePool(size_t max = 1000) : maxSize(max) {}
    
    ~ValuePool() {
        std::lock_guard<std::mutex> lock(poolMutex);
        for (T* value : pool) {
            delete value;
        }
    }
    
    // 获取对象
    T* acquire() {
        std::lock_guard<std::mutex> lock(poolMutex);
        if (!pool.empty()) {
            T* value = pool.back();
            pool.pop_back();
            return value;
        }
        return new T();
    }
    
    // 归还对象
    void release(T* value) {
        if (!value) return;
        
        std::lock_guard<std::mutex> lock(poolMutex);
        if (pool.size() < maxSize) {
            pool.push_back(value);
        } else {
            delete value;
        }
    }
    
    // 获取池大小
    size_t size() const {
        std::lock_guard<std::mutex> lock(poolMutex);
        return pool.size();
    }
};

// 全局内存池实例
extern ValuePool<Integer> g_intPool;
extern ValuePool<Double> g_doublePool;
extern ValuePool<Bool> g_boolPool;
extern ValuePool<String> g_stringPool;
extern ValuePool<Char> g_charPool;

} // namespace InterpreterCore

#endif // MEMORY_MANAGER_H
