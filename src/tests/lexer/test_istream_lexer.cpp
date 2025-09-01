#include <iostream>
#include <sstream>
#include <string>
#include "lexer/lexer.h"
#include "lexer/token.h"

using namespace std;

int main() {
    cout << "=== 测试Lexer输入流构造函数 ===" << endl;
    
    // 创建一个包含简单代码的字符串
    string code = "let a = 1;\nlet b = 2;\nlet c = a + b;";
    
    cout << "测试代码:" << endl;
    cout << code << endl;
    cout << endl;
    
    // 创建字符串输入流
    istringstream input(code);
    
    // 使用输入流构造函数创建lexer
    lexer::Lexer lex(input);
    
    cout << "开始词法分析..." << endl;
    
    // 开始扫描token
    lexer::Token* token;
    int tokenCount = 0;
    
    while ((token = lex.scan()) != nullptr && token != lexer::Token::END_OF_FILE) {
        tokenCount++;
        cout << "Token " << tokenCount << ": ";
        
        // 显示token信息
        if (token->Tag == lexer::ID) {
            cout << "ID (" << token->str() << ")";
        } else if (token->Tag == lexer::NUM) {
            cout << "NUM (" << token->str() << ")";
        } else if (token->Tag == lexer::STR) {
            cout << "STR (" << token->str() << ")";
        } else if (token->Tag == lexer::LET) {
            cout << "LET";
        } else {
            cout << "UNKNOWN(" << token->Tag << ")";
        }
        
        cout << " at Line " << lex.line << ", Col " << lex.column << endl;
        
        // 防止无限循环
        if (tokenCount > 100) {
            cout << "警告: Token数量过多，可能存在循环" << endl;
            break;
        }
    }
    
    cout << endl;
    cout << "词法分析完成，共生成 " << tokenCount << " 个token" << endl;
    
    return 0;
} 