#ifndef ASYNC_RUNTIME_H
#define ASYNC_RUNTIME_H

#include "interpreter/values/value.h"
#include <future>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <memory>
#include <vector>
#include <atomic>

namespace InterpreterCore {

// 异步任务状态
enum class AsyncTaskState {
    PENDING,    // 等待执行
    RUNNING,    // 正在执行
    COMPLETED,  // 已完成
    FAILED      // 执行失败
};

// 异步任务
class AsyncTask {
public:
    using TaskFunction = std::function<Value*()>;
    
    AsyncTask(TaskFunction func, const std::string& name = "");
    ~AsyncTask();
    
    // 执行任务
    void execute();
    
    // 获取结果
    Value* getResult();
    
    // 获取异常
    std::exception_ptr getException();
    
    // 获取状态
    AsyncTaskState getState() const { return state; }
    
    // 获取任务名称
    const std::string& getName() const { return name; }
    
    // 等待完成
    void wait();
    
    // 检查是否完成
    bool isCompleted() const { return state == AsyncTaskState::COMPLETED; }
    
    // 检查是否失败
    bool isFailed() const { return state == AsyncTaskState::FAILED; }

private:
    TaskFunction taskFunction;
    std::string name;
    std::atomic<AsyncTaskState> state;
    Value* result;
    std::exception_ptr exception;
    std::mutex resultMutex;
    std::condition_variable resultCondition;
};

// Promise类 - 表示异步操作的结果
class Promise : public Value {
public:
    Promise();
    Promise(std::shared_ptr<AsyncTask> task);
    ~Promise();
    
    // Value接口实现
    std::string toString() const override;
    Value* clone() const override;
    bool equals(const Value* other) const override;
    
    // Promise特定方法
    void resolve(Value* value);
    void reject(std::exception_ptr exception);
    
    // 等待Promise完成
    Value* await();
    
    // 检查状态
    bool isResolved() const { return resolved; }
    bool isRejected() const { return rejected; }
    bool isPending() const { return !resolved && !rejected; }
    
    // 获取结果
    Value* getValue();
    std::exception_ptr getException();
    
    // 添加回调
    void then(std::function<void(Value*)> onResolve);
    void catch_(std::function<void(std::exception_ptr)> onReject);
    void finally(std::function<void()> onFinally);

private:
    std::shared_ptr<AsyncTask> task;
    std::atomic<bool> resolved;
    std::atomic<bool> rejected;
    Value* value;
    std::exception_ptr exception;
    std::mutex promiseMutex;
    std::condition_variable promiseCondition;
    
    // 回调函数
    std::vector<std::function<void(Value*)>> resolveCallbacks;
    std::vector<std::function<void(std::exception_ptr)>> rejectCallbacks;
    std::vector<std::function<void()>> finallyCallbacks;
};

// 异步运行时 - 管理异步任务的执行
class AsyncRuntime {
public:
    static AsyncRuntime& getInstance();
    
    // 启动运行时
    void start(int threadCount = std::thread::hardware_concurrency());
    
    // 停止运行时
    void stop();
    
    // 提交异步任务
    std::shared_ptr<AsyncTask> submitTask(AsyncTask::TaskFunction func, const std::string& name = "");
    
    // 创建Promise
    std::shared_ptr<Promise> createPromise();
    
    // 等待所有任务完成
    void waitForAllTasks();
    
    // 获取运行时状态
    bool isRunning() const { return running; }
    size_t getActiveTaskCount() const { return activeTasks; }
    size_t getCompletedTaskCount() const { return completedTasks; }

private:
    AsyncRuntime();
    ~AsyncRuntime();
    
    // 禁用拷贝构造和赋值
    AsyncRuntime(const AsyncRuntime&) = delete;
    AsyncRuntime& operator=(const AsyncRuntime&) = delete;
    
    // 工作线程函数
    void workerThread();
    
    // 成员变量
    std::atomic<bool> running;
    std::atomic<size_t> activeTasks;
    std::atomic<size_t> completedTasks;
    
    std::vector<std::thread> workerThreads;
    std::queue<std::shared_ptr<AsyncTask>> taskQueue;
    std::mutex queueMutex;
    std::condition_variable queueCondition;
};

// 异步函数包装器
class AsyncFunction {
public:
    AsyncFunction(std::function<Value*()> func);
    ~AsyncFunction();
    
    // 调用异步函数
    std::shared_ptr<Promise> call();
    
    // 获取函数
    std::function<Value*()> getFunction() const { return function; }

private:
    std::function<Value*()> function;
};

// 异步迭代器 - 用于异步序列处理
class AsyncIterator {
public:
    AsyncIterator(std::function<std::shared_ptr<Promise>()> nextFunc);
    ~AsyncIterator();
    
    // 获取下一个值
    std::shared_ptr<Promise> next();
    
    // 检查是否还有更多值
    bool hasNext() const { return hasMore; }
    
    // 异步迭代
    std::shared_ptr<Promise> forEach(std::function<void(Value*)> callback);
    std::shared_ptr<Promise> map(std::function<Value*(Value*)> mapper);
    std::shared_ptr<Promise> filter(std::function<bool(Value*)> predicate);
    std::shared_ptr<Promise> reduce(std::function<Value*(Value*, Value*)> reducer, Value* initial);

private:
    std::function<std::shared_ptr<Promise>()> nextFunction;
    std::atomic<bool> hasMore;
};

} // namespace InterpreterCore

#endif // ASYNC_RUNTIME_H
