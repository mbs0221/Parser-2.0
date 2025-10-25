#include "interpreter/core/concurrency.h"
#include "common/logger.h"
#include <algorithm>
#include <chrono>
#include <random>

namespace InterpreterCore {

// ==================== Coroutine 实现 ====================

Coroutine::Coroutine(std::function<Value*()> func, const std::string& name)
    : function(func), name(name), state(CoroutineState::CREATED), result(nullptr), priority(0) {
    startTime = std::chrono::steady_clock::now();
}

Coroutine::~Coroutine() {
    if (result) {
        delete result;
    }
}

Value* Coroutine::resume() {
    if (state == CoroutineState::COMPLETED || state == CoroutineState::CANCELLED) {
        return result;
    }
    
    state = CoroutineState::RUNNING;
    threadId = std::this_thread::get_id();
    
    try {
        result = function();
        state = CoroutineState::COMPLETED;
        endTime = std::chrono::steady_clock::now();
        return result;
    } catch (const std::exception& e) {
        errorMessage = e.what();
        state = CoroutineState::COMPLETED;
        endTime = std::chrono::steady_clock::now();
        return nullptr;
    }
}

void Coroutine::suspend() {
    if (state == CoroutineState::RUNNING) {
        state = CoroutineState::SUSPENDED;
    }
}

void Coroutine::cancel() {
    if (state != CoroutineState::COMPLETED) {
        state = CoroutineState::CANCELLED;
        endTime = std::chrono::steady_clock::now();
    }
}

// ==================== CoroutineScheduler 实现 ====================

CoroutineScheduler& CoroutineScheduler::getInstance() {
    static CoroutineScheduler instance;
    return instance;
}

CoroutineScheduler::CoroutineScheduler()
    : running(false), paused(false), maxConcurrentCoroutines(10), 
      schedulingPolicy("fifo"), timeSlice(std::chrono::milliseconds(10)) {
}

CoroutineScheduler::~CoroutineScheduler() {
    stop();
}

std::shared_ptr<Coroutine> CoroutineScheduler::createCoroutine(std::function<Value*()> func, const std::string& name) {
    auto coroutine = std::make_shared<Coroutine>(func, name);
    return coroutine;
}

void CoroutineScheduler::scheduleCoroutine(std::shared_ptr<Coroutine> coroutine) {
    if (!coroutine) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(queueMutex);
    readyQueue.push(coroutine);
    queueCondition.notify_one();
}

void CoroutineScheduler::cancelCoroutine(std::shared_ptr<Coroutine> coroutine) {
    if (!coroutine) {
        return;
    }
    
    coroutine->cancel();
    
    std::lock_guard<std::mutex> lock(queueMutex);
    // 从所有队列中移除协程
    std::queue<std::shared_ptr<Coroutine>> newQueue;
    while (!readyQueue.empty()) {
        auto coro = readyQueue.front();
        readyQueue.pop();
        if (coro != coroutine) {
            newQueue.push(coro);
        }
    }
    readyQueue = newQueue;
    
    runningCoroutines.erase(
        std::remove(runningCoroutines.begin(), runningCoroutines.end(), coroutine),
        runningCoroutines.end()
    );
    
    suspendedCoroutines.erase(
        std::remove(suspendedCoroutines.begin(), suspendedCoroutines.end(), coroutine),
        suspendedCoroutines.end()
    );
}

void CoroutineScheduler::start() {
    if (running) {
        return;
    }
    
    running = true;
    paused = false;
    schedulerThread = std::thread(&CoroutineScheduler::schedulerLoop, this);
    
    LOG_DEBUG("协程调度器已启动");
}

void CoroutineScheduler::stop() {
    if (!running) {
        return;
    }
    
    running = false;
    queueCondition.notify_all();
    
    if (schedulerThread.joinable()) {
        schedulerThread.join();
    }
    
    LOG_DEBUG("协程调度器已停止");
}

void CoroutineScheduler::pause() {
    paused = true;
    LOG_DEBUG("协程调度器已暂停");
}

void CoroutineScheduler::resume() {
    paused = false;
    queueCondition.notify_all();
    LOG_DEBUG("协程调度器已恢复");
}

size_t CoroutineScheduler::getActiveCoroutineCount() const {
    std::lock_guard<std::mutex> lock(queueMutex);
    return runningCoroutines.size();
}

size_t CoroutineScheduler::getSuspendedCoroutineCount() const {
    std::lock_guard<std::mutex> lock(queueMutex);
    return suspendedCoroutines.size();
}

std::vector<std::shared_ptr<Coroutine>> CoroutineScheduler::getAllCoroutines() const {
    std::lock_guard<std::mutex> lock(queueMutex);
    std::vector<std::shared_ptr<Coroutine>> allCoroutines;
    
    // 复制所有队列中的协程
    std::queue<std::shared_ptr<Coroutine>> tempQueue = readyQueue;
    while (!tempQueue.empty()) {
        allCoroutines.push_back(tempQueue.front());
        tempQueue.pop();
    }
    
    allCoroutines.insert(allCoroutines.end(), runningCoroutines.begin(), runningCoroutines.end());
    allCoroutines.insert(allCoroutines.end(), suspendedCoroutines.begin(), suspendedCoroutines.end());
    allCoroutines.insert(allCoroutines.end(), completedCoroutines.begin(), completedCoroutines.end());
    
    return allCoroutines;
}

void CoroutineScheduler::setMaxConcurrentCoroutines(size_t max) {
    maxConcurrentCoroutines = max;
}

void CoroutineScheduler::setSchedulingPolicy(const std::string& policy) {
    schedulingPolicy = policy;
}

void CoroutineScheduler::setTimeSlice(std::chrono::milliseconds timeSlice) {
    this->timeSlice = timeSlice;
}

void CoroutineScheduler::schedulerLoop() {
    while (running) {
        std::unique_lock<std::mutex> lock(queueMutex);
        
        // 等待任务或停止信号
        queueCondition.wait(lock, [this] { 
            return !running || (!paused && !readyQueue.empty()); 
        });
        
        if (!running) {
            break;
        }
        
        if (paused) {
            continue;
        }
        
        // 选择下一个协程
        auto coroutine = selectNextCoroutine();
        if (!coroutine) {
            continue;
        }
        
        // 检查并发限制
        if (runningCoroutines.size() >= maxConcurrentCoroutines) {
            // 将协程放回队列
            readyQueue.push(coroutine);
            continue;
        }
        
        // 将协程移到运行队列
        runningCoroutines.push_back(coroutine);
        lock.unlock();
        
        // 执行协程
        try {
            coroutine->resume();
        } catch (const std::exception& e) {
            LOG_ERROR("协程执行错误: " + std::string(e.what()));
        }
        
        // 将协程移到完成队列
        lock.lock();
        runningCoroutines.erase(
            std::remove(runningCoroutines.begin(), runningCoroutines.end(), coroutine),
            runningCoroutines.end()
        );
        
        if (coroutine->isCompleted()) {
            completedCoroutines.push_back(coroutine);
        } else if (coroutine->isSuspended()) {
            suspendedCoroutines.push_back(coroutine);
        }
    }
}

std::shared_ptr<Coroutine> CoroutineScheduler::selectNextCoroutine() {
    if (readyQueue.empty()) {
        return nullptr;
    }
    
    if (schedulingPolicy == "fifo") {
        auto coroutine = readyQueue.front();
        readyQueue.pop();
        return coroutine;
    } else if (schedulingPolicy == "priority") {
        // 优先级调度：找到优先级最高的协程
        std::shared_ptr<Coroutine> highestPriority = nullptr;
        std::queue<std::shared_ptr<Coroutine>> tempQueue;
        
        while (!readyQueue.empty()) {
            auto coroutine = readyQueue.front();
            readyQueue.pop();
            
            if (!highestPriority || coroutine->getPriority() > highestPriority->getPriority()) {
                if (highestPriority) {
                    tempQueue.push(highestPriority);
                }
                highestPriority = coroutine;
            } else {
                tempQueue.push(coroutine);
            }
        }
        
        readyQueue = tempQueue;
        return highestPriority;
    } else if (schedulingPolicy == "round_robin") {
        // 轮转调度
        auto coroutine = readyQueue.front();
        readyQueue.pop();
        return coroutine;
    }
    
    // 默认FIFO
    auto coroutine = readyQueue.front();
    readyQueue.pop();
    return coroutine;
}

// ==================== ThreadPoolTask 实现 ====================

ThreadPoolTask::ThreadPoolTask(std::function<Value*()> func, const std::string& name)
    : function(func), name(name), completed(false), running(false), hasErrorFlag(false), 
      result(nullptr), priority(0) {
    startTime = std::chrono::steady_clock::now();
}

ThreadPoolTask::~ThreadPoolTask() {
    if (result) {
        delete result;
    }
}

Value* ThreadPoolTask::execute() {
    if (completed || running) {
        return result;
    }
    
    running = true;
    
    try {
        result = function();
        completed = true;
        endTime = std::chrono::steady_clock::now();
        
        std::lock_guard<std::mutex> lock(resultMutex);
        completionCondition.notify_all();
        
        return result;
    } catch (const std::exception& e) {
        errorMessage = e.what();
        hasErrorFlag = true;
        completed = true;
        endTime = std::chrono::steady_clock::now();
        
        std::lock_guard<std::mutex> lock(resultMutex);
        completionCondition.notify_all();
        
        return nullptr;
    }
}

void ThreadPoolTask::waitForCompletion() {
    std::unique_lock<std::mutex> lock(resultMutex);
    completionCondition.wait(lock, [this] { return completed; });
}

bool ThreadPoolTask::waitForCompletion(std::chrono::milliseconds timeout) {
    std::unique_lock<std::mutex> lock(resultMutex);
    return completionCondition.wait_for(lock, timeout, [this] { return completed; });
}

// ==================== ThreadPool 实现 ====================

ThreadPool& ThreadPool::getInstance() {
    static ThreadPool instance;
    return instance;
}

ThreadPool::ThreadPool()
    : initialized(false), shutdownRequested(false), maxQueueSize(1000),
      taskTimeout(std::chrono::minutes(5)), threadPriority(0),
      totalTasksProcessed(0), totalTasksFailed(0) {
}

ThreadPool::~ThreadPool() {
    shutdown();
}

void ThreadPool::initialize(size_t threadCount) {
    if (initialized) {
        return;
    }
    
    shutdownRequested = false;
    
    for (size_t i = 0; i < threadCount; ++i) {
        threads.emplace_back(&ThreadPool::workerThread, this);
    }
    
    initialized = true;
    LOG_DEBUG("线程池已初始化，线程数: " + std::to_string(threadCount));
}

void ThreadPool::shutdown() {
    if (!initialized) {
        return;
    }
    
    shutdownRequested = true;
    queueCondition.notify_all();
    
    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    
    threads.clear();
    initialized = false;
    
    LOG_DEBUG("线程池已关闭");
}

void ThreadPool::resize(size_t newSize) {
    if (!initialized) {
        return;
    }
    
    size_t currentSize = threads.size();
    
    if (newSize > currentSize) {
        // 增加线程
        for (size_t i = currentSize; i < newSize; ++i) {
            threads.emplace_back(&ThreadPool::workerThread, this);
        }
    } else if (newSize < currentSize) {
        // 减少线程
        shutdownRequested = true;
        queueCondition.notify_all();
        
        for (size_t i = newSize; i < currentSize; ++i) {
            if (threads[i].joinable()) {
                threads[i].join();
            }
        }
        
        threads.resize(newSize);
        shutdownRequested = false;
    }
    
    LOG_DEBUG("线程池大小已调整为: " + std::to_string(newSize));
}

std::shared_ptr<ThreadPoolTask> ThreadPool::submitTask(std::function<Value*()> func, const std::string& name) {
    if (!initialized) {
        return nullptr;
    }
    
    auto task = std::make_shared<ThreadPoolTask>(func, name);
    
    std::lock_guard<std::mutex> lock(queueMutex);
    
    if (taskQueue.size() >= maxQueueSize) {
        LOG_WARNING("任务队列已满，无法提交新任务");
        return nullptr;
    }
    
    taskQueue.push(task);
    queueCondition.notify_one();
    
    return task;
}

std::vector<std::shared_ptr<ThreadPoolTask>> ThreadPool::submitTasks(const std::vector<std::function<Value*()>>& functions) {
    std::vector<std::shared_ptr<ThreadPoolTask>> tasks;
    
    for (const auto& func : functions) {
        auto task = submitTask(func);
        if (task) {
            tasks.push_back(task);
        }
    }
    
    return tasks;
}

size_t ThreadPool::getActiveTaskCount() const {
    std::lock_guard<std::mutex> lock(queueMutex);
    return activeTasks.size();
}

size_t ThreadPool::getQueuedTaskCount() const {
    std::lock_guard<std::mutex> lock(queueMutex);
    return taskQueue.size();
}

size_t ThreadPool::getCompletedTaskCount() const {
    std::lock_guard<std::mutex> lock(queueMutex);
    return completedTasks.size();
}

void ThreadPool::setMaxQueueSize(size_t maxSize) {
    maxQueueSize = maxSize;
}

void ThreadPool::setTaskTimeout(std::chrono::milliseconds timeout) {
    taskTimeout = timeout;
}

void ThreadPool::setThreadPriority(int priority) {
    threadPriority = priority;
}

void ThreadPool::workerThread() {
    while (!shutdownRequested) {
        std::unique_lock<std::mutex> lock(queueMutex);
        
        // 等待任务或关闭信号
        queueCondition.wait(lock, [this] { 
            return shutdownRequested || !taskQueue.empty(); 
        });
        
        if (shutdownRequested) {
            break;
        }
        
        if (taskQueue.empty()) {
            continue;
        }
        
        // 获取任务
        auto task = taskQueue.front();
        taskQueue.pop();
        
        // 将任务移到活动队列
        activeTasks.push_back(task);
        lock.unlock();
        
        // 执行任务
        processTask(task);
        
        // 将任务移到完成队列
        lock.lock();
        activeTasks.erase(
            std::remove(activeTasks.begin(), activeTasks.end(), task),
            activeTasks.end()
        );
        completedTasks.push_back(task);
        completionCondition.notify_all();
    }
}

void ThreadPool::processTask(std::shared_ptr<ThreadPoolTask> task) {
    try {
        task->execute();
        totalTasksProcessed++;
    } catch (const std::exception& e) {
        LOG_ERROR("任务执行错误: " + std::string(e.what()));
        totalTasksFailed++;
    }
}

// ==================== 并发原语实现 ====================
// 注意：直接使用标准库的std::mutex、std::condition_variable和std::semaphore
// 不需要额外的封装层

// ==================== ConcurrencyManager 实现 ====================

ConcurrencyManager& ConcurrencyManager::getInstance() {
    static ConcurrencyManager instance;
    return instance;
}

ConcurrencyManager::ConcurrencyManager()
    : coroutineScheduler(CoroutineScheduler::getInstance()),
      threadPool(ThreadPool::getInstance()),
      initialized(false), maxConcurrency(100), defaultThreadCount(std::thread::hardware_concurrency()) {
}

ConcurrencyManager::~ConcurrencyManager() {
    shutdown();
}

void ConcurrencyManager::initialize() {
    if (initialized) {
        return;
    }
    
    // 初始化线程池
    threadPool.initialize(defaultThreadCount);
    
    // 启动协程调度器
    coroutineScheduler.start();
    
    initialized = true;
    LOG_DEBUG("并发管理器已初始化");
}

void ConcurrencyManager::shutdown() {
    if (!initialized) {
        return;
    }
    
    // 停止协程调度器
    coroutineScheduler.stop();
    
    // 关闭线程池
    threadPool.shutdown();
    
    initialized = false;
    LOG_DEBUG("并发管理器已关闭");
}

std::shared_ptr<Coroutine> ConcurrencyManager::createCoroutine(std::function<Value*()> func, const std::string& name) {
    return coroutineScheduler.createCoroutine(func, name);
}

std::shared_ptr<ThreadPoolTask> ConcurrencyManager::submitTask(std::function<Value*()> func, const std::string& name) {
    return threadPool.submitTask(func, name);
}

// 注意：直接使用标准库的并发原语，不需要额外封装

size_t ConcurrencyManager::getTotalActiveTasks() const {
    return coroutineScheduler.getActiveCoroutineCount() + threadPool.getActiveTaskCount();
}

size_t ConcurrencyManager::getTotalCompletedTasks() const {
    return threadPool.getCompletedTaskCount();
}

void ConcurrencyManager::setMaxConcurrency(size_t max) {
    maxConcurrency = max;
    coroutineScheduler.setMaxConcurrentCoroutines(max);
}

void ConcurrencyManager::setDefaultThreadCount(size_t count) {
    defaultThreadCount = count;
}

// ==================== 并发工具函数实现 ====================

namespace ConcurrencyUtils {

std::vector<Value*> parallelExecute(const std::vector<std::function<Value*()>>& functions) {
    auto& threadPool = ThreadPool::getInstance();
    auto tasks = threadPool.submitTasks(functions);
    
    std::vector<Value*> results;
    for (auto& task : tasks) {
        task->waitForCompletion();
        results.push_back(task->getResult());
    }
    
    return results;
}

void waitForAll(const std::vector<std::shared_ptr<ThreadPoolTask>>& tasks) {
    for (auto& task : tasks) {
        task->waitForCompletion();
    }
}

std::shared_ptr<ThreadPoolTask> waitForAny(const std::vector<std::shared_ptr<ThreadPoolTask>>& tasks) {
    while (true) {
        for (auto& task : tasks) {
            if (task->isCompleted()) {
                return task;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

bool waitForAllWithTimeout(const std::vector<std::shared_ptr<ThreadPoolTask>>& tasks, 
                          std::chrono::milliseconds timeout) {
    auto startTime = std::chrono::steady_clock::now();
    
    while (true) {
        bool allCompleted = true;
        for (auto& task : tasks) {
            if (!task->isCompleted()) {
                allCompleted = false;
                break;
            }
        }
        
        if (allCompleted) {
            return true;
        }
        
        auto currentTime = std::chrono::steady_clock::now();
        if (currentTime - startTime >= timeout) {
            return false;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

} // namespace ConcurrencyUtils

} // namespace InterpreterCore
