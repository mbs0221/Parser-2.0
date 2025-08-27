#include <iostream>
#include <string>

using namespace std;

// 模拟内置函数调用的测试
int main() {
    cout << "=== 内置函数调用测试 ===" << endl;
    
    // 测试1：模拟print函数调用
    cout << "\n1. 模拟print函数调用:" << endl;
    cout << "print(\"Hello World\")" << endl;
    cout << "输出: Hello World" << endl;
    
    // 测试2：模拟length函数调用
    cout << "\n2. 模拟length函数调用:" << endl;
    string testStr = "Hello World";
    cout << "length(\"" << testStr << "\")" << endl;
    cout << "输出: " << testStr.length() << endl;
    
    // 测试3：模拟字符串拼接
    cout << "\n3. 模拟字符串拼接:" << endl;
    string str1 = "Hello, ";
    string str2 = "World!";
    string result = str1 + str2;
    cout << "\"" << str1 << "\" + \"" << str2 << "\"" << endl;
    cout << "输出: " << result << endl;
    
    // 测试4：模拟数组访问
    cout << "\n4. 模拟数组访问:" << endl;
    string fruits[] = {"apple", "banana", "cherry"};
    cout << "fruits[0]" << endl;
    cout << "输出: " << fruits[0] << endl;
    
    // 测试5：模拟字典访问
    cout << "\n5. 模拟字典访问:" << endl;
    cout << "person[\"name\"]" << endl;
    cout << "输出: John" << endl;
    
    // 测试6：模拟cin函数调用
    cout << "\n6. 模拟cin函数调用:" << endl;
    cout << "cin()" << endl;
    cout << "输出: [用户输入]" << endl;
    
    // 测试7：模拟count函数调用
    cout << "\n7. 模拟count函数调用:" << endl;
    cout << "count([1, 2, 3, 4, 5])" << endl;
    cout << "输出: 5" << endl;
    
    // 测试8：模拟substring函数调用
    cout << "\n8. 模拟substring函数调用:" << endl;
    string text = "Hello World";
    string sub = text.substr(0, 5);
    cout << "substring(\"" << text << "\", 0, 5)" << endl;
    cout << "输出: " << sub << endl;
    
    cout << "\n=== 所有内置函数测试完成 ===" << endl;
    
    return 0;
}
