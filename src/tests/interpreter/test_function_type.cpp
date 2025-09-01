#include <iostream>
#include "interpreter/types/type_registry.h"
#include "interpreter/values/function_types.h"

using namespace std;

int main() {
    cout << "=== 测试函数类型注册 ===" << endl;
    
    // 获取全局类型注册表
    TypeRegistry* registry = TypeRegistry::getGlobalInstance();
    if (!registry) {
        cerr << "错误：无法获取全局类型注册表" << endl;
        return 1;
    }
    
    cout << "全局类型注册表获取成功" << endl;
    
    // 检查function类型是否存在
    ObjectType* functionType = registry->getType("function");
    if (functionType) {
        cout << "✓ function类型注册成功" << endl;
        cout << "  类型名称: " << functionType->getTypeName() << endl;
        cout << "  类型信息: " << functionType->getTypeInfo() << endl;
    } else {
        cerr << "✗ function类型未找到" << endl;
        return 1;
    }
    
    // 列出所有已注册的类型
    cout << "\n已注册的类型:" << endl;
    vector<string> typeNames = registry->getAllTypeNames();
    for (const string& name : typeNames) {
        cout << "  - " << name << endl;
    }
    
    // 测试创建Function对象
    cout << "\n=== 测试Function对象创建 ===" << endl;
    
    // 创建一个内置函数
    BuiltinFunction* builtinFunc = new BuiltinFunction("test_func", 
        [](vector<Value*>) -> Value* { return new Integer(42); });
    
    if (builtinFunc) {
        cout << "✓ BuiltinFunction创建成功" << endl;
        cout << "  函数名称: " << builtinFunc->getName() << endl;
        cout << "  函数类型: " << builtinFunc->getFunctionType() << endl;
        
        // 检查类型信息
        ObjectType* funcType = builtinFunc->getValueType();
        if (funcType) {
            cout << "  类型名称: " << funcType->getTypeName() << endl;
            cout << "  类型信息: " << funcType->getTypeInfo() << endl;
        } else {
            cerr << "  警告：函数对象的类型信息为空" << endl;
        }
        
        // 测试函数调用
        vector<Value*> args;
        Value* result = builtinFunc->call(args);
        if (result) {
            cout << "  函数调用结果: " << result->toString() << endl;
            delete result;
        }
        
        delete builtinFunc;
    } else {
        cerr << "✗ BuiltinFunction创建失败" << endl;
        return 1;
    }
    
    cout << "\n=== 测试完成 ===" << endl;
    return 0;
} 