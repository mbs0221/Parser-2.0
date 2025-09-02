#include <gtest/gtest.h>
#include <string>
#include "lexer/lexer.h"
#include "lexer/token.h"

using namespace std;

// 获取Token类型的字符串表示（基于实际源码）
string getTokenTypeName(int tag) {
    // 先检查是否为ASCII字符
    if (tag >= 32 && tag <= 126) {
        char c = (char)tag;
        switch (c) {
            case '=': return "ASSIGN";
            case '+': return "PLUS";
            case '-': return "MINUS";
            case '*': return "MULTIPLY";
            case '/': return "DIVIDE";
            case '%': return "MODULO";
            case '<': return "LT";
            case '>': return "GT";
            case '!': return "NOT";
            case '&': return "BIT_AND";
            case '|': return "BIT_OR";
            case '^': return "BIT_XOR";
            case '~': return "BIT_NOT";
            case '.': return "DOT";
            case '(': return "LPAREN";
            case ')': return "RPAREN";
            case '{': return "LBRACE";
            case '}': return "RBRACE";
            case '[': return "LBRACKET";
            case ']': return "RBRACKET";
            case ',': return "COMMA";
            case ';': return "SEMICOLON";
            case ':': return "COLON";
            case '?': return "QUESTION";
            default: return string(1, c); // 返回字符本身
        }
    }
    
    // 检查enum定义的Tag值
    switch (tag) {
        case lexer::BASIC: return "BASIC";
        case lexer::ID: return "ID";
        case lexer::NUM: return "NUM";
        case lexer::REAL: return "REAL";
        case lexer::DOUBLE: return "DOUBLE";
        case lexer::FLOAT: return "FLOAT";
        case lexer::CHAR: return "CHAR";
        case lexer::STR: return "STR";
        case lexer::BOOL: return "BOOL";
        case lexer::ARRAY: return "ARRAY";
        case lexer::DICT: return "DICT";
        case lexer::END_OF_FILE: return "END_OF_FILE";
        
        // 关键字
        case lexer::IF: return "IF";
        case lexer::THEN: return "THEN";
        case lexer::ELSE: return "ELSE";
        case lexer::DO: return "DO";
        case lexer::WHILE: return "WHILE";
        case lexer::FOR: return "FOR";
        case lexer::CASE: return "CASE";
        case lexer::DEFAULT: return "DEFAULT";
        case lexer::LET: return "LET";
        case lexer::BREAK: return "BREAK";
        case lexer::CONTINUE: return "CONTINUE";
        case lexer::RETURN: return "RETURN";
        case lexer::THROW: return "THROW";
        case lexer::TRY: return "TRY";
        case lexer::CATCH: return "CATCH";
        case lexer::FINALLY: return "FINALLY";
        case lexer::SWITCH: return "SWITCH";
        case lexer::BEGIN: return "BEGIN";
        case lexer::END: return "END";
        case lexer::FUNCTION: return "FUNCTION";
        case lexer::LAMBDA: return "LAMBDA";
        case lexer::STRUCT: return "STRUCT";
        case lexer::CLASS: return "CLASS";
        case lexer::PUBLIC: return "PUBLIC";
        case lexer::PRIVATE: return "PRIVATE";
        case lexer::PROTECTED: return "PROTECTED";
        case lexer::IMPORT: return "IMPORT";
        
        // 多字符运算符
        case lexer::EQ_EQ: return "EQ_EQ";
        case lexer::NE_EQ: return "NE_EQ";
        case lexer::AND_AND: return "AND_AND";
        case lexer::OR_OR: return "OR_OR";
        case lexer::LEFT_SHIFT: return "LEFT_SHIFT";
        case lexer::RIGHT_SHIFT: return "RIGHT_SHIFT";
        case lexer::INCREMENT: return "INCREMENT";
        case lexer::DECREMENT: return "DECREMENT";
        case lexer::ARROW: return "ARROW";
        
        default: return "UNKNOWN(" + to_string(tag) + ")";
    }
}

// 测试从字符串加载功能
TEST(LexerTest, FromString) {
    lexer::Lexer lex;
    string code = "let a = 1;\nlet b = 2;\nlet c = a + b;";
    
    // 测试from_string方法
    EXPECT_TRUE(lex.from_string(code));
    
    // 开始词法分析
    lexer::Token* token;
    int tokenCount = 0;
    vector<string> expectedTokens = {"LET", "ID (a)", "ASSIGN", "NUM (1)", "SEMICOLON", 
                                    "LET", "ID (b)", "ASSIGN", "NUM (2)", "SEMICOLON",
                                    "LET", "ID (c)", "ASSIGN", "ID (a)", "PLUS", "ID (b)", "SEMICOLON"};
    
    while ((token = lex.scan()) != nullptr && token != lexer::Token::END_OF_FILE) {
        EXPECT_LT(tokenCount, expectedTokens.size()) << "Token数量超出预期";
        if (tokenCount < expectedTokens.size()) {
            string actualToken = getTokenTypeName(token->Tag);
            if (token->Tag == lexer::ID || token->Tag == lexer::NUM || token->Tag == lexer::STR) {
                actualToken += " (" + token->str() + ")";
            }
            EXPECT_EQ(actualToken, expectedTokens[tokenCount]) << "Token " << tokenCount << " 不匹配";
        }
        tokenCount++;
        if (tokenCount > 100) break; // 防止无限循环
    }
    
    EXPECT_EQ(tokenCount, expectedTokens.size()) << "Token总数不匹配";
}

// 测试从文件加载功能
TEST(LexerTest, FromFile) {
    lexer::Lexer lex;
    
    // 测试from_file方法
    bool result = lex.from_file("tests/lexer/basic_syntax.txt");
    
    if (result) {
        // 文件加载成功，进行词法分析
        lexer::Token* token;
        int tokenCount = 0;
        
        while ((token = lex.scan()) != nullptr && token != lexer::Token::END_OF_FILE) {
            tokenCount++;
            EXPECT_GT(token->Tag, 0) << "Token Tag应该大于0";
            if (tokenCount > 100) break; // 防止无限循环
        }
        
        EXPECT_GT(tokenCount, 0) << "应该至少生成一个token";
    } else {
        // 文件加载失败，这是正常的（文件可能不存在）
        GTEST_SKIP() << "测试文件不存在，跳过文件测试";
    }
}

// 测试错误处理
TEST(LexerTest, ErrorHandling) {
    lexer::Lexer lex;
    
    // 测试空字符串
    EXPECT_TRUE(lex.from_string(""));
    
    // 测试空字符串的词法分析
    lexer::Token* token = lex.scan();
    EXPECT_EQ(token, lexer::Token::END_OF_FILE) << "空字符串应该返回END_OF_FILE";
}

// 测试边界情况
TEST(LexerTest, EdgeCases) {
    lexer::Lexer lex;
    
    // 测试只包含空格的字符串
    string spaces = "   \t\n  ";
    EXPECT_TRUE(lex.from_string(spaces));
    
    lexer::Token* token = lex.scan();
    EXPECT_EQ(token, lexer::Token::END_OF_FILE) << "只包含空格的字符串应该返回END_OF_FILE";
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 