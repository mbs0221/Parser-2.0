#include "lexer/lexer.h"
#include <iostream>

int main() {
    Lexer lexer;
    
    // 检查words映射中是否有"let"
    auto it = lexer.words.find("let");
    if (it != lexer.words.end()) {
        std::cout << "Found 'let' in words map with Tag: " << it->second->Tag << std::endl;
    } else {
        std::cout << "'let' not found in words map!" << std::endl;
    }
    
    // 检查其他关键字
    std::cout << "Checking other keywords:" << std::endl;
    std::cout << "if: " << (lexer.words.find("if") != lexer.words.end() ? "found" : "not found") << std::endl;
    std::cout << "while: " << (lexer.words.find("while") != lexer.words.end() ? "found" : "not found") << std::endl;
    std::cout << "for: " << (lexer.words.find("for") != lexer.words.end() ? "found" : "not found") << std::endl;
    
    return 0;
}
