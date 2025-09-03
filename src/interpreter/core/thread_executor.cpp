#include "interpreter/core/thread_executor.h"
#include "interpreter/core/interpreter.h"
#include "parser/parser.h"
#include "interpreter/values/value.h"
#include "interpreter/scope/scope.h"
#include "interpreter/scope/scope_manager.h"

#include <iostream>
#include <chrono>

using namespace std;

ThreadExecutor::ThreadExecutor(Interpreter* interpreter) : interpreter(interpreter) {
    parser = interpreter->parser;
}

ThreadExecutor::~ThreadExecutor() {
    delete interpreter;
}

void ThreadExecutor::setCurrentThreadExitCode(int code) {
    currentThreadExitCode = code;
}

int ThreadExecutor::getCurrentThreadExitCode() {
    return currentThreadExitCode;
}

void ThreadExecutor::threadExecutionEntry(Program* program, Value* result, int exitCode) {
    currentThreadExitCode = exitCode;
    interpreter->visit(program);
}

// 解析并在线程中执行程序，返回执行结果
Value* ThreadExecutor::executeWithThread(const std::string& filename) {
    if (!parser) {
        reportError("Parser not initialized");
        return new Integer(0);
    }
    
    try {
        // 使用解析器解析文件
        Program* program = parser->parse(filename);
        if (!program) {
            return new Integer(-1);
        }
        
        // 在新线程中执行程序
        Value* result = new Integer(0);
        std::thread executionThread([this, program, result]() {
            try {
                // 在新线程中创建新的解释器实例
                Interpreter threadInterpreter(false); // 不加载插件，避免冲突
                
                // 进入新作用域
                threadInterpreter.scopeManager.enterScope();
                
                // 执行程序
                threadInterpreter.visit(program);
                
                // 退出作用域
                threadInterpreter.scopeManager.exitScope();
                
            } catch (const ReturnException& e) {
                return e.getValue();
            } catch (const std::exception& e) {
                return new Integer(-1);
            } catch (...) {
                return new Integer(-1);
            }
        });
        
        // 等待线程完成
        if (executionThread.joinable()) {
            executionThread.join();
        }
        
        // 清理解析后的程序
        delete program;
    } catch (const std::exception& e) {
        return new Integer(-1);
    }
}

