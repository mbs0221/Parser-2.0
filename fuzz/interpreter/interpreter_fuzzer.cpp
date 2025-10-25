#include <stdint.h>
#include <stddef.h>
#include <string>
#include <iostream>
#include <sstream>
#include <memory>

// 包含interpreter头文件
#include "interpreter/core/interpreter.h"
#include "parser/parser.h"
#include "parser/inter.h"

// 内存安全检查
bool isMemorySafe(const std::string& input) {
    // 检查输入大小
    if (input.length() > 10000) {
        return false;
    }
    
    // 检查嵌套深度
    int brace_count = 0;
    int paren_count = 0;
    int bracket_count = 0;
    
    for (char c : input) {
        switch (c) {
            case '{': brace_count++; break;
            case '}': brace_count--; break;
            case '(': paren_count++; break;
            case ')': paren_count--; break;
            case '[': bracket_count++; break;
            case ']': bracket_count--; break;
        }
        
        // 检查嵌套深度
        if (brace_count > 100 || paren_count > 100 || bracket_count > 100) {
            return false;
        }
    }
    
    return true;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    // 将输入数据转换为字符串
    std::string input(reinterpret_cast<const char*>(data), size);
    
    // 限制输入大小
    if (size > 1000) {
        return 0;
    }
    
    // 检查输入是否安全
    if (!isMemorySafe(input)) {
        return 0;
    }
    
    try {
        // 创建interpreter（不加载插件以避免复杂依赖）
        Interpreter interpreter(false);
        
        // 创建parser
        Parser parser;
        
        // 创建一个临时文件来测试interpreter
        std::string temp_filename = "/tmp/fuzz_test_" + std::to_string(size) + ".txt";
        std::ofstream temp_file(temp_filename);
        if (temp_file.is_open()) {
            temp_file << input;
            temp_file.close();
            
            // 测试interpreter的解析和执行功能
            Value* result = interpreter.parseAndExecute(temp_filename);
            
            // 如果执行成功，测试结果的基本属性
            if (result) {
                // 测试值的基本属性
                std::string type_name = interpreter.getValueTypeName(result);
                
                // 测试作用域管理
                interpreter.scopeManager.enterScope();
                interpreter.scopeManager.exitScope();
                
                // 清理结果
                delete result;
            }
            
            // 清理临时文件
            std::remove(temp_filename.c_str());
        }
        
    } catch (const std::exception& e) {
        // 捕获异常但不崩溃
        return 0;
    } catch (...) {
        // 捕获所有其他异常
        return 0;
    }
    
    return 0;  // 非零返回值表示发现了一个bug
}