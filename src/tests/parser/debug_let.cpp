#include "lexer/value.h"
#include <iostream>

int main() {
    std::cout << "BASIC = " << BASIC << std::endl;
    std::cout << "LET = " << LET << std::endl;
    std::cout << "ID = " << ID << std::endl;
    std::cout << "NUM = " << NUM << std::endl;
    std::cout << "REAL = " << REAL << std::endl;
    std::cout << "DOUBLE = " << DOUBLE << std::endl;
    std::cout << "CHAR = " << CHAR << std::endl;
    std::cout << "STR = " << STR << std::endl;
    std::cout << "BOOL = " << BOOL << std::endl;
    std::cout << "END_OF_FILE = " << END_OF_FILE << std::endl;
    std::cout << "IF = " << IF << std::endl;
    std::cout << "THEN = " << THEN << std::endl;
    std::cout << "ELSE = " << ELSE << std::endl;
    std::cout << "DO = " << DO << std::endl;
    std::cout << "WHILE = " << WHILE << std::endl;
    std::cout << "FOR = " << FOR << std::endl;
    std::cout << "CASE = " << CASE << std::endl;
    std::cout << "DEFAULT = " << DEFAULT << std::endl;
    return 0;
}
