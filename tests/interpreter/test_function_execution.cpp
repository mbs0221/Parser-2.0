#include <iostream>
#include <vector>
#include "interpreter/types/type_registry.h"
#include "interpreter/values/function_types.h"
#include "interpreter/values/value_factory.h"

using namespace std;

// 测试用的内置函数
Value* test_add(vector<Value*>& args) {
    if (args.size() != 2) {
        return new String("Error: add function expects 2 arguments");
    }
    
    // 尝试转换为数值类型
    if (Integer* a = dynamic_cast<Integer*>(args[0])) {
        if (Integer* b = dynamic_cast<Integer*>(args[1])) {
            return new Integer(a->getValue() + b->getValue());
        } else if (Double* b = dynamic_cast<Double*>(args[1])) {
            return new Double(a->getValue() + b->getValue());
        }
    } else if (Double* a = dynamic_cast<Double*>(args[0])) {
        if (Integer* b = dynamic_cast<Integer*>(args[1])) {
            return new Double(a->getValue() + b->getValue());
        } else if (Double* b = dynamic_cast<Double*>(args[1])) {
            return new Double(a->getValue() + b->getValue());
        }
    }
    
    return new String("Error: add function expects numeric arguments");
}

Value* test_print(vector<Value*>& args) {
    cout << "Print function called with " << args.size() << " arguments:" << endl;
    for (size_t i = 0; i < args.size(); ++i) {
        cout << "  Arg " << i << ": " << args[i]->toString() << endl;
    }
    return new String("Print completed");
}

Value* test_concat(vector<Value*>& args) {
    if (args.size() < 2) {
        return new String("Error: concat function expects at least 2 arguments");
    }
    
    string result;
    for (Value* arg : args) {
        result += arg->toString();
    }
    
    return new String(result);
}

int main() {
    cout << "=== 测试函数调用功能 ===" << endl;
    
    // 获取全局类型注册表
    TypeRegistry* registry = TypeRegistry::getGlobalInstance();
    if (!registry) {
        cerr << "错误：无法获取全局类型注册表" << endl;
        return 1;
    }
    
    cout << "全局类型注册表获取成功" << endl;
    
    // 检查function类型是否存在
    ObjectType* functionType = registry->getType("function");
    if (!functionType) {
        cerr << "错误：function类型未找到" << endl;
        return 1;
    }
    
    cout << "✓ function类型注册成功" << endl;
    
    // 创建内置函数
    cout << "\n=== 测试内置函数 ===" << endl;
    
    // 创建加法函数
    BuiltinFunction* addFunc = new BuiltinFunction("add", test_add, {"a", "b"});
    if (addFunc) {
        cout << "✓ add函数创建成功" << endl;
        cout << "  函数名称: " << addFunc->getName() << endl;
        cout << "  函数类型: " << addFunc->getFunctionType() << endl;
        
        // 测试函数调用
        vector<Value*> args1 = {new Integer(5), new Integer(3)};
        Value* result1 = addFunc->call(args1);
        if (result1) {
            cout << "  add(5, 3) = " << result1->toString() << endl;
            delete result1;
        }
        
        // 测试混合类型
        vector<Value*> args2 = {new Integer(10), new Double(2.5)};
        Value* result2 = addFunc->call(args2);
        if (result2) {
            cout << "  add(10, 2.5) = " << result2->toString() << endl;
            delete result2;
        }
        
        // 测试错误参数
        vector<Value*> args3 = {new String("hello"), new Integer(5)};
        Value* result3 = addFunc->call(args3);
        if (result3) {
            cout << "  add(\"hello\", 5) = " << result3->toString() << endl;
            delete result3;
        }
        
        delete addFunc;
    }
    
    // 创建打印函数
    BuiltinFunction* printFunc = new BuiltinFunction("print", test_print, {"..."});
    if (printFunc) {
        cout << "\n✓ print函数创建成功" << endl;
        
        // 测试函数调用
        vector<Value*> args = {new String("Hello"), new Integer(42), new Double(3.14)};
        Value* result = printFunc->call(args);
        if (result) {
            cout << "  print函数返回: " << result->toString() << endl;
            delete result;
        }
        
        delete printFunc;
    }
    
    // 创建连接函数
    BuiltinFunction* concatFunc = new BuiltinFunction("concat", test_concat, {"..."});
    if (concatFunc) {
        cout << "\n✓ concat函数创建成功" << endl;
        
        // 测试函数调用
        vector<Value*> args = {new String("Hello "), new String("World "), new Integer(2024)};
        Value* result = concatFunc->call(args);
        if (result) {
            cout << "  concat结果: " << result->toString() << endl;
            delete result;
        }
        
        delete concatFunc;
    }
    
    // 测试函数类型的方法调用
    cout << "\n=== 测试函数类型的方法调用 ===" << endl;
    
    BuiltinFunction* testFunc = new BuiltinFunction("test", [](vector<Value*>) -> Value* { 
        return new String("test function called"); 
    });
    
    if (testFunc && testFunc->getValueType()) {
        cout << "✓ 测试函数创建成功，类型: " << testFunc->getValueType()->getTypeName() << endl;
        
        // 通过类型系统调用方法
        vector<Value*> methodArgs;
        Value* methodResult = testFunc->getValueType()->callMethod(testFunc, "get_name", methodArgs);
        if (methodResult) {
            cout << "  通过类型系统调用get_name方法: " << methodResult->toString() << endl;
            delete methodResult;
        }
        
        delete testFunc;
    }
    
    // 测试函数克隆
    cout << "\n=== 测试函数克隆 ===" << endl;
    
    BuiltinFunction* originalFunc = new BuiltinFunction("original", test_add, {"x", "y"});
    if (originalFunc) {
        cout << "✓ 原始函数创建成功" << endl;
        
        // 克隆函数
        Value* clonedFunc = originalFunc->clone();
        if (clonedFunc) {
            cout << "✓ 函数克隆成功" << endl;
            cout << "  原始函数类型: " << originalFunc->getFunctionType() << endl;
            cout << "  克隆函数类型: " << clonedFunc->getBuiltinTypeName() << endl;
            
            // 测试克隆的函数
            if (BuiltinFunction* cloned = dynamic_cast<BuiltinFunction*>(clonedFunc)) {
                vector<Value*> args = {new Integer(7), new Integer(8)};
                Value* result = cloned->call(args);
                if (result) {
                    cout << "  克隆函数调用结果: " << result->toString() << endl;
                    delete result;
                }
            }
            
            delete clonedFunc;
        }
        
        delete originalFunc;
    }
    
    cout << "\n=== 测试完成 ===" << endl;
    return 0;
} 