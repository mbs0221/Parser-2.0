#include "Parser/builtin_functions.h"
#include <iostream>

using namespace std;

int main() {
    cout << "测试内置函数重构..." << endl;
    
    // 创建内置函数管理器
    BuiltinFunctionManager manager;
    
    // 测试内置函数注册
    vector<string> funcNames = manager.getBuiltinFunctionNames();
    cout << "已注册的内置函数: ";
    for (const string& name : funcNames) {
        cout << name << " ";
    }
    cout << endl;
    
    // 测试函数存在性检查
    cout << "print函数存在: " << (manager.isBuiltinFunction("print") ? "是" : "否") << endl;
    cout << "count函数存在: " << (manager.isBuiltinFunction("count") ? "是" : "否") << endl;
    cout << "cin函数存在: " << (manager.isBuiltinFunction("cin") ? "是" : "否") << endl;
    cout << "unknown函数存在: " << (manager.isBuiltinFunction("unknown") ? "是" : "否") << endl;
    
    cout << "内置函数重构测试完成！" << endl;
    return 0;
}
