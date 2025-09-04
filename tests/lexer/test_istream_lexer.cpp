#include <iostream>
#include <sstream>
#include <string>
#include "lexer/lexer.h"
#include "lexer/token.h"

using namespace std;

int main() {
    LOG_INFO("=== 测试Lexer输入流构造函数 ===");
    
    // 创建一个包含简单代码的字符串
    string code = "let a = 1;\nlet b = 2;\nlet c = a + b;";
    
    LOG_INFO("测试代码:");
    LOG_INFO(code);
    LOG_INFO("");
    
    // 创建字符串输入流
    istringstream input(code);
    
    // 使用输入流构造函数创建lexer
    lexer::Lexer lex(input);
    
    LOG_INFO("开始词法分析...");
    
    // 开始扫描token
    lexer::Token* token;
    int tokenCount = 0;
    
    while ((token = lex.scan()) != nullptr && token != lexer::Token::END_OF_FILE) {
        tokenCount++;
        LOG_INFO("Token " + to_string(tokenCount) + ": ");
        
        // 显示token信息
        if (token->Tag == lexer::ID) {
            LOG_INFO("ID (" + token->str() + ")");
        } else if (token->Tag == lexer::NUM) {
            LOG_INFO("NUM (" + token->str() + ")");
        } else if (token->Tag == lexer::STR) {
            LOG_INFO("STR (" + token->str() + ")");
        } else if (token->Tag == lexer::LET) {
            LOG_INFO("LET");
        } else {
            LOG_INFO("UNKNOWN(" + to_string(token->Tag) + ")");
        }
        
        LOG_INFO(" at Line " + to_string(lex.line) + ", Col " + to_string(lex.column));
        
        // 防止无限循环
        if (tokenCount > 100) {
            LOG_WARN("警告: Token数量过多，可能存在循环");
            break;
        }
    }
    
    LOG_INFO("词法分析完成，共生成 " + to_string(tokenCount) + " 个token");
    
    return 0;
} 