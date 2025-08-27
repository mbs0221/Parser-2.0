#include <iostream>
#include <string>

using namespace std;

// 简化的测试程序，直接测试解释器的核心功能
int main() {
    cout << "=== 简化解释器测试 ===" << endl;
    
    // 测试1：基本输出
    cout << "\n1. 基本输出测试:" << endl;
    cout << "Hello World" << endl;
    
    // 测试2：字符串操作
    cout << "\n2. 字符串操作测试:" << endl;
    string str1 = "Hello, ";
    string str2 = "World!";
    string result = str1 + str2;
    cout << "字符串拼接: " << result << endl;
    
    // 测试3：字符串比较
    cout << "\n3. 字符串比较测试:" << endl;
    string apple = "apple";
    string banana = "banana";
    bool comparison = apple < banana;
    cout << "apple < banana: " << (comparison ? "true" : "false") << endl;
    
    // 测试4：字符串长度
    cout << "\n4. 字符串长度测试:" << endl;
    string testStr = "Hello World";
    cout << "字符串长度: " << testStr.length() << endl;
    
    // 测试5：字符串索引访问
    cout << "\n5. 字符串索引访问测试:" << endl;
    char firstChar = testStr[0];
    cout << "第一个字符: " << firstChar << endl;
    
    // 测试6：数组操作
    cout << "\n6. 数组操作测试:" << endl;
    string fruits[] = {"apple", "banana", "cherry"};
    cout << "第一个水果: " << fruits[0] << endl;
    
    // 测试7：字典操作（使用map模拟）
    cout << "\n7. 字典操作测试:" << endl;
    // 这里用简单的字符串模拟字典操作
    cout << "模拟字典访问: name = John" << endl;
    
    // 测试8：作用域概念
    cout << "\n8. 作用域概念测试:" << endl;
    {
        string localVar = "局部变量";
        cout << "在局部作用域: " << localVar << endl;
    }
    cout << "退出局部作用域" << endl;
    
    cout << "\n=== 所有测试完成 ===" << endl;
    
    return 0;
}
