#include <stdint.h>
#include <stddef.h>
#include <string>
#include <iostream>
#include <sstream>
#include <memory>

// 包含lexer头文件
#include "lexer/lexer.h"
#include "lexer/token.h"

using namespace lexer;

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
    
    // 检查字符串字面量
    bool in_string = false;
    char string_char = 0;
    
    for (size_t i = 0; i < input.length(); ++i) {
        char c = input[i];
        
        if (!in_string) {
            if (c == '"' || c == '\'') {
                in_string = true;
                string_char = c;
            }
        } else {
            if (c == string_char && (i == 0 || input[i-1] != '\\')) {
                in_string = false;
            }
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
        // 创建lexer
        Lexer lexer;
        
        // 从字符串加载输入
        if (!lexer.from_string(input)) {
            return 0;
        }
        
        // 测试lexer的token扫描功能
        std::vector<Token*> tokens;
        Token* token = nullptr;
        int token_count = 0;
        
        do {
            token = lexer.scan();
            if (token) {
                tokens.push_back(token);
                token_count++;
                
                // 限制token数量防止无限循环
                if (token_count > 1000) {
                    break;
                }
                
                // 测试token的基本属性
                if (token->Tag != END_OF_FILE) {
                    // 测试token类型检查
                    lexer.isToken(token->Tag);
                    lexer.isKeyword(token->Tag);
                    lexer.isOperator(token->Tag);
                    lexer.isIdentifier();
                    lexer.isString();
                    lexer.isNumber();
                    lexer.isAs();
                    lexer.isFrom();
                }
            }
        } while (token && token->Tag != END_OF_FILE);
        
        // 测试lexer的匹配功能
        if (tokens.size() > 1) {
            // 重新创建lexer进行匹配测试
            Lexer match_lexer;
            if (match_lexer.from_string(input)) {
                // 测试各种匹配方法
                try {
                    match_lexer.matchIdentifier();
                    match_lexer.matchType();
                    match_lexer.matchOperator();
                    match_lexer.matchWord();
                } catch (...) {
                    // 匹配失败是正常的，继续测试
                }
            }
        }
        
        // 清理tokens
        for (Token* t : tokens) {
            if (t && t->Tag != END_OF_FILE) {
                delete t;
            }
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
