#ifndef CONCURRENCY_H
#define CONCURRENCY_H

#include "interpreter/values/value.h"
#include "interpreter/types/types.h"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <atomic>
#include <future>
#include <chrono>

namespace InterpreterCore {

// 协程状态
enum class CoroutineState {
    CREATED,    // 已创建
    RUNNING,    // 运行中
    SUSPENDED,  // 暂停
    COMPLETED,  // 完成
    CANCELLED   // 取消
};

// 协程类
class Coroutine {
public:
    Coroutine(std::function<Value*()> func, const std::string& name = "");
    ~Coroutine();
    
    // 协程控制
    Value* resume();
    void suspend();
    void cancel();
    
    // 状态查询
    CoroutineState getState() const { return state; }
    const std::string& getName() const { return name; }
    bool isCompleted() const { return state == CoroutineState::COMPLETED; }
    bool isRunning() const { return state == CoroutineState::RUNNING; }
    bool isSuspended() const { return state == CoroutineState::SUSPENDED; }
    
    // 获取结果
    Value* getResult() const { return result; }
    std::string getErrorMessage() const { return errorMessage; }
    
    // 设置优先级
    void setPriority(int priority) { this->priority = priority; }
    int getPriority() const { return priority; }

private:
    std::function<Value*()> function;
    std::string name;
    CoroutineState state;
    Value* result;
    std::string errorMessage;
    int priority;
    
    // 协程执行上下文
    std::thread::id threadId;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point endTime;
};

// 协程调度器
class CoroutineScheduler {
public:
    static CoroutineScheduler& getInstance();
    
    // 协程管理
    std::shared_ptr<Coroutine> createCoroutine(std::function<Value*()> func, const std::string& name = "");
    void scheduleCoroutine(std::shared_ptr<Coroutine> coroutine);
    void cancelCoroutine(std::shared_ptr<Coroutine> coroutine);
    
    // 调度控制
    void start();
    void stop();
    void pause();
    void resume();
    
    // 状态查询
    bool isRunning() const { return running; }
    size_t getActiveCoroutineCount() const;
    size_t getSuspendedCoroutineCount() const;
    std::vector<std::shared_ptr<Coroutine>> getAllCoroutines() const;
    
    // 配置
    void setMaxConcurrentCoroutines(size_t max);
    void setSchedulingPolicy(const std::string& policy); // "fifo", "priority", "round_robin"
    void setTimeSlice(std::chrono::milliseconds timeSlice);

private:
    CoroutineScheduler();
    ~CoroutineScheduler();
    
    // 禁用拷贝构造和赋值
    CoroutineScheduler(const CoroutineScheduler&) = delete;
    CoroutineScheduler& operator=(const CoroutineScheduler&) = delete;
    
    // 调度线程
    void schedulerLoop();
    std::shared_ptr<Coroutine> selectNextCoroutine();
    
    // 协程队列
    std::queue<std::shared_ptr<Coroutine>> readyQueue;
    std::vector<std::shared_ptr<Coroutine>> runningCoroutines;
    std::vector<std::shared_ptr<Coroutine>> suspendedCoroutines;
    std::vector<std::shared_ptr<Coroutine>> completedCoroutines;
    
    // 线程和同步
    std::thread schedulerThread;
    std::mutex queueMutex;
    std::condition_variable queueCondition;
    std::atomic<bool> running;
    std::atomic<bool> paused;
    
    // 配置
    size_t maxConcurrentCoroutines;
    std::string schedulingPolicy;
    std::chrono::milliseconds timeSlice;
};

// 线程池任务
class ThreadPoolTask {
public:
    ThreadPoolTask(std::function<Value*()> func, const std::string& name = "");
    ~ThreadPoolTask();
    
    // 任务执行
    Value* execute();
    
    // 状态查询
    bool isCompleted() const { return completed; }
    bool isRunning() const { return running; }
    bool hasError() const { return hasErrorFlag; }
    
    // 获取结果
    Value* getResult() const { return result; }
    std::string getErrorMessage() const { return errorMessage; }
    
    // 等待完成
    void waitForCompletion();
    bool waitForCompletion(std::chrono::milliseconds timeout);
    
    // 设置优先级
    void setPriority(int priority) { this->priority = priority; }
    int getPriority() const { return priority; }

private:
    std::function<Value*()> function;
    std::string name;
    std::atomic<bool> completed;
    std::atomic<bool> running;
    std::atomic<bool> hasErrorFlag;
    Value* result;
    std::string errorMessage;
    int priority;
    
    // 同步
    std::mutex resultMutex;
    std::condition_variable completionCondition;
    
    // 时间信息
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point endTime;
};

// 线程池
class ThreadPool {
public:
    static ThreadPool& getInstance();
    
    // 线程池管理
    void initialize(size_t threadCount = std::thread::hardware_concurrency());
    void shutdown();
    void resize(size_t newSize);
    
    // 任务提交
    std::shared_ptr<ThreadPoolTask> submitTask(std::function<Value*()> func, const std::string& name = "");
    std::vector<std::shared_ptr<ThreadPoolTask>> submitTasks(const std::vector<std::function<Value*()>>& functions);
    
    // 状态查询
    bool isInitialized() const { return initialized; }
    size_t getThreadCount() const { return threads.size(); }
    size_t getActiveTaskCount() const;
    size_t getQueuedTaskCount() const;
    size_t getCompletedTaskCount() const;
    
    // 配置
    void setMaxQueueSize(size_t maxSize);
    void setTaskTimeout(std::chrono::milliseconds timeout);
    void setThreadPriority(int priority);

private:
    ThreadPool();
    ~ThreadPool();
    
    // 禁用拷贝构造和赋值
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    
    // 工作线程函数
    void workerThread();
    void processTask(std::shared_ptr<ThreadPoolTask> task);
    
    // 线程管理
    std::vector<std::thread> threads;
    std::queue<std::shared_ptr<ThreadPoolTask>> taskQueue;
    std::vector<std::shared_ptr<ThreadPoolTask>> activeTasks;
    std::vector<std::shared_ptr<ThreadPoolTask>> completedTasks;
    
    // 同步
    std::mutex queueMutex;
    std::condition_variable queueCondition;
    std::condition_variable completionCondition;
    std::atomic<bool> initialized;
    std::atomic<bool> shutdownRequested;
    
    // 配置
    size_t maxQueueSize;
    std::chrono::milliseconds taskTimeout;
    int threadPriority;
    
    // 统计
    std::atomic<size_t> totalTasksProcessed;
    std::atomic<size_t> totalTasksFailed;
};

// 注意：直接使用标准库的std::mutex、std::condition_variable和std::semaphore
// 不需要额外的封装层

// 并发管理器
class ConcurrencyManager {
public:
    static ConcurrencyManager& getInstance();
    
    // 初始化
    void initialize();
    void shutdown();
    
    // 协程管理
    CoroutineScheduler* getCoroutineScheduler() { return &coroutineScheduler; }
    std::shared_ptr<Coroutine> createCoroutine(std::function<Value*()> func, const std::string& name = "");
    
    // 线程池管理
    ThreadPool* getThreadPool() { return &threadPool; }
    std::shared_ptr<ThreadPoolTask> submitTask(std::function<Value*()> func, const std::string& name = "");
    
    // 注意：直接使用标准库的并发原语，不需要额外封装
    
    // 状态查询
    bool isInitialized() const { return initialized; }
    size_t getTotalActiveTasks() const;
    size_t getTotalCompletedTasks() const;
    
    // 配置
    void setMaxConcurrency(size_t max);
    void setDefaultThreadCount(size_t count);

private:
    ConcurrencyManager();
    ~ConcurrencyManager();
    
    // 禁用拷贝构造和赋值
    ConcurrencyManager(const ConcurrencyManager&) = delete;
    ConcurrencyManager& operator=(const ConcurrencyManager&) = delete;
    
    CoroutineScheduler& coroutineScheduler;
    ThreadPool& threadPool;
    std::atomic<bool> initialized;
    
    // 配置
    size_t maxConcurrency;
    size_t defaultThreadCount;
};

// 并发工具函数
namespace ConcurrencyUtils {
    // 并行执行
    std::vector<Value*> parallelExecute(const std::vector<std::function<Value*()>>& functions);
    
    // 并行映射
    template<typename T, typename U>
    std::vector<U> parallelMap(const std::vector<T>& input, std::function<U(T)> func);
    
    // 并行归约
    template<typename T>
    T parallelReduce(const std::vector<T>& input, std::function<T(T, T)> func, T initial);
    
    // 并行过滤
    template<typename T>
    std::vector<T> parallelFilter(const std::vector<T>& input, std::function<bool(T)> predicate);
    
    // 等待所有任务完成
    void waitForAll(const std::vector<std::shared_ptr<ThreadPoolTask>>& tasks);
    
    // 等待任意任务完成
    std::shared_ptr<ThreadPoolTask> waitForAny(const std::vector<std::shared_ptr<ThreadPoolTask>>& tasks);
    
    // 超时等待
    bool waitForAllWithTimeout(const std::vector<std::shared_ptr<ThreadPoolTask>>& tasks, 
                              std::chrono::milliseconds timeout);
}

} // namespace InterpreterCore

#endif // CONCURRENCY_H
