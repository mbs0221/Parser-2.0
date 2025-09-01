#include <iostream>
#include <string>
#include "lexer/lexer.h"
#include "lexer/token.h"

using namespace std;

int main() {
    cout << "=== 测试Lexer的open方法 ===" << endl;
    
    // 测试1：从字符串创建流
    cout << "\n测试1：从字符串创建流" << endl;
    lexer::Lexer lex1;
    string code = "let a = 1;\nlet b = 2;\nlet c = a + b;";
    if (lex1.open(code)) {
        cout << "成功从字符串创建流" << endl;
        cout << "开始词法分析..." << endl;
        
        lexer::Token* token;
        int tokenCount = 0;
        while ((token = lex1.scan()) != nullptr && token != lexer::Token::END_OF_FILE) {
            tokenCount++;
            cout << "Token " << tokenCount << ": ";
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
            cout << " at Line " << lex1.line << ", Col " << lex1.column << endl;
            if (tokenCount > 100) {
                cout << "警告: Token数量过多，可能存在循环" << endl;
                break;
            }
        }
        cout << "词法分析完成，共生成 " << tokenCount << " 个token" << endl;
    } else {
        cout << "从字符串创建流失败" << endl;
    }
    
    // 测试2：从文件创建流
    cout << "\n测试2：从文件创建流" << endl;
    lexer::Lexer lex2;
    if (lex2.open("tests/lexer/basic_syntax.txt")) {
        cout << "成功从文件创建流" << endl;
        cout << "开始词法分析..." << endl;
        
        lexer::Token* token;
        int tokenCount = 0;
        while ((token = lex2.scan()) != nullptr && token != lexer::Token::END_OF_FILE) {
            tokenCount++;
            cout << "Token " << tokenCount << ": ";
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
            cout << " at Line " << lex2.line << ", Col " << lex2.column << endl;
            if (tokenCount > 100) {
                cout << "警告: Token数量过多，可能存在循环" << endl;
                break;
            }
        }
        cout << "词法分析完成，共生成 " << tokenCount << " 个token" << endl;
    } else {
        cout << "从文件创建流失败" << endl;
    }
    
    return 0;
} 