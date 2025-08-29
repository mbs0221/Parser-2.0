#include "interpreter/interpreter.h"
#include "parser/parser.h"
#include "lexer/lexer.h"
#include <iostream>
#include <fstream>
#include <cstdio>

using namespace std;

int main() {
    cout << "=== 插件系统测试 ===" << endl;
    
    // 创建解释器
    Interpreter interpreter;
    
    // 显示已加载的插件
    cout << "\n1. 检查已加载的插件:" << endl;
    vector<string> loadedPlugins = interpreter.getLoadedPlugins();
    if (loadedPlugins.empty()) {
        cout << "   当前没有加载任何插件" << endl;
    } else {
        for (const string& plugin : loadedPlugins) {
            cout << "   - " << plugin << endl;
        }
    }
    
    // 测试基础内置函数
    cout << "\n2. 测试基础内置函数:" << endl;
    string testCode1 = R"(
        print("Hello from basic builtin functions!");
        let x = 10;
        let y = 20;
        print("max(", x, ",", y, ") =", max(x, y));
        print("min(", x, ",", y, ") =", min(x, y));
        print("abs(-15) =", abs(-15));
        print("pow(2, 3) =", pow(2, 3));
    )";
    
    try {
        // 创建临时文件来测试
        ofstream tempFile("temp_test.txt");
        tempFile << testCode1;
        tempFile.close();
        
        Parser parser;
        Program* program = parser.parse("temp_test.txt");
        interpreter.execute(program);
        delete program;
        
        // 清理临时文件
        remove("temp_test.txt");
    } catch (const exception& e) {
        cout << "   错误: " << e.what() << endl;
    }
    
    cout << "\n=== 插件系统测试完成 ===" << endl;
    return 0;
}
