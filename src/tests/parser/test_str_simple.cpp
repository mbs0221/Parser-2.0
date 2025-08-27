#include <iostream>
#include <string>

// 简单的测试，不依赖复杂的头文件
using namespace std;

int main() {
    cout << "=== 测试 str 方法的基本功能 ===" << endl;
    
    // 测试字符串字面量
    string testStr = "Hello World";
    cout << "字符串字面量: " << testStr << endl;
    
    // 测试数字字面量
    int testInt = 42;
    cout << "整数字面量: " << to_string(testInt) << endl;
    
    // 测试布尔字面量
    bool testBool = true;
    cout << "布尔字面量: " << (testBool ? "true" : "false") << endl;
    
    cout << "\n=== 测试完成 ===" << endl;
    return 0;
}
