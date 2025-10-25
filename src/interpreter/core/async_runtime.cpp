#include "interpreter/core/async_runtime.h"
#include "common/logger.h"
#include <chrono>
#include <algorithm>

namespace InterpreterCore {

// ==================== AsyncTask 实现 ====================

AsyncTask::AsyncTask(TaskFunction func, const std::string& name)
    : taskFunction(func), name(name), state(AsyncTaskState::PENDING), result(nullptr) {
}

AsyncTask::~AsyncTask() {
    if (result) {
        delete result;
    }
}

void AsyncTask::execute() {
    state = AsyncTaskState::RUNNING;
    
    try {
        result = taskFunction();
        state = AsyncTaskState::COMPLETED;
    } catch (...) {
        exception = std::current_exception();
        state = AsyncTaskState::FAILED;
    }
    
    resultCondition.notify_all();
}

Value* AsyncTask::getResult() {
    std::unique_lock<std::mutex> lock(resultMutex);
    resultCondition.wait(lock, [this] { return state == AsyncTaskState::COMPLETED || state == AsyncTaskState::FAILED; });
    
    if (state == AsyncTaskState::COMPLETED) {
        return result;
    }
    return nullptr;
}

std::exception_ptr AsyncTask::getException() {
    std::unique_lock<std::mutex> lock(resultMutex);
    resultCondition.wait(lock, [this] { return state == AsyncTaskState::COMPLETED || state == AsyncTaskState::FAILED; });
    
    return exception;
}

void AsyncTask::wait() {
    std::unique_lock<std::mutex> lock(resultMutex);
    resultCondition.wait(lock, [this] { return state == AsyncTaskState::COMPLETED || state == AsyncTaskState::FAILED; });
}

// ==================== Promise 实现 ====================

Promise::Promise() : resolved(false), rejected(false), value(nullptr) {
}

Promise::Promise(std::shared_ptr<AsyncTask> task) 
    : task(task), resolved(false), rejected(false), value(nullptr) {
}

Promise::~Promise() {
    if (value) {
        delete value;
    }
}

std::string Promise::toString() const {
    if (resolved) {
        return "Promise(resolved: " + (value ? value->toString() : "null") + ")";
    } else if (rejected) {
        return "Promise(rejected)";
    } else {
        return "Promise(pending)";
    }
}

Value* Promise::clone() const {
    return new Promise(*this);
}

bool Promise::equals(const Value* other) const {
    if (const Promise* otherPromise = dynamic_cast<const Promise*>(other)) {
        return this == otherPromise; // Promise对象比较使用指针比较
    }
    return false;
}

void Promise::resolve(Value* val) {
    std::lock_guard<std::mutex> lock(promiseMutex);
    
    if (resolved || rejected) {
        return; // Promise只能被resolve或reject一次
    }
    
    value = val;
    resolved = true;
    
    // 执行resolve回调
    for (auto& callback : resolveCallbacks) {
        callback(val);
    }
    
    // 执行finally回调
    for (auto& callback : finallyCallbacks) {
        callback();
    }
    
    promiseCondition.notify_all();
}

void Promise::reject(std::exception_ptr ex) {
    std::lock_guard<std::mutex> lock(promiseMutex);
    
    if (resolved || rejected) {
        return; // Promise只能被resolve或reject一次
    }
    
    exception = ex;
    rejected = true;
    
    // 执行reject回调
    for (auto& callback : rejectCallbacks) {
        callback(ex);
    }
    
    // 执行finally回调
    for (auto& callback : finallyCallbacks) {
        callback();
    }
    
    promiseCondition.notify_all();
}

Value* Promise::await() {
    std::unique_lock<std::mutex> lock(promiseMutex);
    promiseCondition.wait(lock, [this] { return resolved || rejected; });
    
    if (resolved) {
        return value;
    } else {
        std::rethrow_exception(exception);
    }
}

Value* Promise::getValue() {
    std::lock_guard<std::mutex> lock(promiseMutex);
    return value;
}

std::exception_ptr Promise::getException() {
    std::lock_guard<std::mutex> lock(promiseMutex);
    return exception;
}

void Promise::then(std::function<void(Value*)> onResolve) {
    std::lock_guard<std::mutex> lock(promiseMutex);
    
    if (resolved) {
        onResolve(value);
    } else if (!rejected) {
        resolveCallbacks.push_back(onResolve);
    }
}

void Promise::catch_(std::function<void(std::exception_ptr)> onReject) {
    std::lock_guard<std::mutex> lock(promiseMutex);
    
    if (rejected) {
        onReject(exception);
    } else if (!resolved) {
        rejectCallbacks.push_back(onReject);
    }
}

void Promise::finally(std::function<void()> onFinally) {
    std::lock_guard<std::mutex> lock(promiseMutex);
    
    if (resolved || rejected) {
        onFinally();
    } else {
        finallyCallbacks.push_back(onFinally);
    }
}

// ==================== AsyncRuntime 实现 ====================

AsyncRuntime& AsyncRuntime::getInstance() {
    static AsyncRuntime instance;
    return instance;
}

AsyncRuntime::AsyncRuntime() : running(false), activeTasks(0), completedTasks(0) {
}

AsyncRuntime::~AsyncRuntime() {
    stop();
}

void AsyncRuntime::start(int threadCount) {
    if (running) {
        return;
    }
    
    running = true;
    workerThreads.reserve(threadCount);
    
    for (int i = 0; i < threadCount; ++i) {
        workerThreads.emplace_back(&AsyncRuntime::workerThread, this);
    }
    
    LOG_INFO("AsyncRuntime started with " + std::to_string(threadCount) + " threads");
}

void AsyncRuntime::stop() {
    if (!running) {
        return;
    }
    
    running = false;
    queueCondition.notify_all();
    
    for (auto& thread : workerThreads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    
    workerThreads.clear();
    LOG_INFO("AsyncRuntime stopped");
}

std::shared_ptr<AsyncTask> AsyncRuntime::submitTask(AsyncTask::TaskFunction func, const std::string& name) {
    auto task = std::make_shared<AsyncTask>(func, name);
    
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        taskQueue.push(task);
    }
    
    queueCondition.notify_one();
    return task;
}

std::shared_ptr<Promise> AsyncRuntime::createPromise() {
    return std::make_shared<Promise>();
}

void AsyncRuntime::waitForAllTasks() {
    while (activeTasks > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void AsyncRuntime::workerThread() {
    while (running) {
        std::shared_ptr<AsyncTask> task;
        
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            queueCondition.wait(lock, [this] { return !taskQueue.empty() || !running; });
            
            if (!running) {
                break;
            }
            
            if (!taskQueue.empty()) {
                task = taskQueue.front();
                taskQueue.pop();
            }
        }
        
        if (task) {
            activeTasks++;
            task->execute();
            activeTasks--;
            completedTasks++;
        }
    }
}

// ==================== AsyncFunction 实现 ====================

AsyncFunction::AsyncFunction(std::function<Value*()> func) : function(func) {
}

AsyncFunction::~AsyncFunction() {
}

std::shared_ptr<Promise> AsyncFunction::call() {
    auto& runtime = AsyncRuntime::getInstance();
    auto promise = runtime.createPromise();
    
    auto task = runtime.submitTask([this, promise]() -> Value* {
        try {
            Value* result = function();
            promise->resolve(result);
            return result;
        } catch (...) {
            promise->reject(std::current_exception());
            throw;
        }
    });
    
    return promise;
}

// ==================== AsyncIterator 实现 ====================

AsyncIterator::AsyncIterator(std::function<std::shared_ptr<Promise>()> nextFunc)
    : nextFunction(nextFunc), hasMore(true) {
}

AsyncIterator::~AsyncIterator() {
}

std::shared_ptr<Promise> AsyncIterator::next() {
    if (!hasMore) {
        auto promise = std::make_shared<Promise>();
        promise->resolve(nullptr); // 表示迭代结束
        return promise;
    }
    
    return nextFunction();
}

std::shared_ptr<Promise> AsyncIterator::forEach(std::function<void(Value*)> callback) {
    auto promise = std::make_shared<Promise>();
    
    auto processNext = [this, callback, promise]() {
        if (!hasMore) {
            promise->resolve(nullptr);
            return;
        }
        
        next()->then([this, callback, promise](Value* value) {
            if (value) {
                callback(value);
                forEach(callback)->then([promise](Value*) {
                    promise->resolve(nullptr);
                });
            } else {
                hasMore = false;
                promise->resolve(nullptr);
            }
        });
    };
    
    processNext();
    return promise;
}

std::shared_ptr<Promise> AsyncIterator::map(std::function<Value*(Value*)> mapper) {
    auto promise = std::make_shared<Promise>();
    std::vector<Value*> results;
    
    auto processNext = [this, mapper, promise, &results]() {
        if (!hasMore) {
            promise->resolve(new Array(results));
            return;
        }
        
        next()->then([this, mapper, promise, &results](Value* value) {
            if (value) {
                Value* mapped = mapper(value);
                results.push_back(mapped);
                map(mapper)->then([promise](Value* result) {
                    promise->resolve(result);
                });
            } else {
                hasMore = false;
                promise->resolve(new Array(results));
            }
        });
    };
    
    processNext();
    return promise;
}

std::shared_ptr<Promise> AsyncIterator::filter(std::function<bool(Value*)> predicate) {
    auto promise = std::make_shared<Promise>();
    std::vector<Value*> results;
    
    auto processNext = [this, predicate, promise, &results]() {
        if (!hasMore) {
            promise->resolve(new Array(results));
            return;
        }
        
        next()->then([this, predicate, promise, &results](Value* value) {
            if (value) {
                if (predicate(value)) {
                    results.push_back(value);
                }
                filter(predicate)->then([promise](Value* result) {
                    promise->resolve(result);
                });
            } else {
                hasMore = false;
                promise->resolve(new Array(results));
            }
        });
    };
    
    processNext();
    return promise;
}

std::shared_ptr<Promise> AsyncIterator::reduce(std::function<Value*(Value*, Value*)> reducer, Value* initial) {
    auto promise = std::make_shared<Promise>();
    Value* accumulator = initial;
    
    auto processNext = [this, reducer, promise, accumulator]() {
        if (!hasMore) {
            promise->resolve(accumulator);
            return;
        }
        
        next()->then([this, reducer, promise, accumulator](Value* value) {
            if (value) {
                Value* newAccumulator = reducer(accumulator, value);
                reduce(reducer, newAccumulator)->then([promise](Value* result) {
                    promise->resolve(result);
                });
            } else {
                hasMore = false;
                promise->resolve(accumulator);
            }
        });
    };
    
    processNext();
    return promise;
}

} // namespace InterpreterCore
