#include "compiler/ssa_converter.h"
#include <iostream>

using namespace std;

int main() {
    cout << "SSA Template Converter Test" << endl;
    
    // 测试不同类型的SSA转换器
    
    // 1. 生成SSA表达式的转换器
    SSAExpressionConverter ssaConverter;
    cout << "SSA Expression Converter created successfully!" << endl;
    
    // 2. 生成汇编代码的转换器
    AssemblyConverter assemblyConverter;
    cout << "Assembly Converter created successfully!" << endl;
    
    // 3. 生成SSA指令的转换器
    SSAInstructionConverter instructionConverter;
    cout << "SSA Instruction Converter created successfully!" << endl;
    
    // 4. 默认编译器
    DefaultCompiler compiler;
    cout << "Default Compiler created successfully!" << endl;
    
    cout << "All template instantiations successful!" << endl;
    return 0;
}
