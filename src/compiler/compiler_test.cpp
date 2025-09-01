#include "compiler/ssa_converter.h"
#include "parser/parser.h"
#include <iostream>

using namespace std;

int main() {
    cout << "SSA Converter Test" << endl;
    
    // 创建解析器
    Parser parser;
    
    // 解析一个简单的程序
    Program* ast = parser.parse("tests/test_simple_comprehensive.txt");
    
    if (!ast) {
        cout << "Failed to parse program" << endl;
        return 1;
    }
    
    // 创建编译器
    Compiler compiler;
    
    // 生成汇编代码
    string assembly = compiler.generateAssembly(ast);
    
    cout << "Generated Assembly:" << endl;
    cout << assembly << endl;
    
    // 生成二进制文件
    bool success = compiler.compileToFile(ast, "output.vmb");
    
    if (success) {
        cout << "Binary file generated: output.vmb" << endl;
    } else {
        cout << "Failed to generate binary file" << endl;
    }
    
    return 0;
}
