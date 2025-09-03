#ifndef THREAD_EXECUTOR_H
#define THREAD_EXECUTOR_H

#include <string>
#include <thread>
#include <atomic>

#include "interpreter/values/value.h"
#include "parser/parser.h"

#include <iostream>
#include <chrono>

using namespace std;

// 前向声明
class Value;
class Program;

// 线程执行器类
class ThreadExecutor {
public:
    // 解析器实例
    Interpreter* interpreter;

    // 构造函数
    ThreadExecutor(Interpreter* interpreter);

    // 析构函数
    ~ThreadExecutor();

    // 在线程中执行程序，返回执行结果
    Value* executeWithThread(const string& filename);
    
    // 设置当前线程的退出码
    void setCurrentThreadExitCode(int code);
    
    // 获取当前线程的退出码
    int getCurrentThreadExitCode();
    
private:
    // 线程执行入口点
    void threadExecutionEntry(Program* program, Value* result, int exitCode);
    
    // 线程本地存储的退出码
    thread_local int currentThreadExitCode;
};

#endif // THREAD_EXECUTOR_H 
