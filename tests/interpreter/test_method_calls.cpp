#include <iostream>
#include <string>
#include "interpreter/core/interpreter.h"
#include "interpreter/types/types.h"
#include "interpreter/values/value.h"
#include "interpreter/scope/scope.h"
// ObjectFactory在value.h中定义

using namespace std;

int main() {
    try {
        cout << "=== 测试类方法调用功能 ===" << endl;
        
        // 创建解释器
        Interpreter interpreter;
        
        // 创建一个简单的类
        ClassType* testClass = new ClassType("TestClass");
        
        // 添加一个成员变量
        ObjectType* intType = new ObjectType("int");
        testClass->addMember("value", intType, VIS_PUBLIC);
        
        // 添加一个方法
        auto getValueMethod = [](Scope* scope) -> Value* {
            // 获取this对象
            Value* thisValue = scope->getVariable("this");
            if (!thisValue) {
                cout << "错误：找不到this对象" << endl;
                return nullptr;
            }
            
            // 转换为ObjectValue
            ObjectValue* objValue = dynamic_cast<ObjectValue*>(thisValue);
            if (!objValue) {
                cout << "错误：this对象不是ObjectValue类型" << endl;
                return nullptr;
            }
            
            // 获取value成员
            Value* value = objValue->getProperty("value");
            if (!value) {
                cout << "错误：找不到value成员" << endl;
                return nullptr;
            }
            
            cout << "getValue方法被调用，返回value: " << value->toString() << endl;
            return value;
        };
        
        // 注册方法
        BuiltinFunction* getValueFunc = new BuiltinFunction(getValueMethod, "getValue()");
        testClass->addUserMethod(getValueFunc, VIS_PUBLIC);
        
        // 添加一个带参数的方法
        auto setValueMethod = [](Scope* scope) -> Value* {
            // 获取参数
            Value* newValue = scope->getVariable("newValue");
            if (!newValue) {
                cout << "错误：找不到newValue参数" << endl;
                return nullptr;
            }
            
            // 获取this对象
            Value* thisValue = scope->getVariable("this");
            if (!thisValue) {
                cout << "错误：找不到this对象" << endl;
                return nullptr;
            }
            
            // 转换为ObjectValue
            ObjectValue* objValue = dynamic_cast<ObjectValue*>(thisValue);
            if (!objValue) {
                cout << "错误：this对象不是ObjectValue类型" << endl;
                return nullptr;
            }
            
            // 设置value成员
            objValue->setProperty("value", newValue);
            cout << "setValue方法被调用，设置value为: " << newValue->toString() << endl;
            return newValue;
        };
        
        // 注册带参数的方法
        BuiltinFunction* setValueFunc = new BuiltinFunction(setValueMethod, "setValue(newValue:int)");
        testClass->addUserMethod(setValueFunc, VIS_PUBLIC);
        
        // 创建类实例
        ObjectFactory factory;
        ObjectValue* instance = dynamic_cast<ObjectValue*>(factory.createValue(testClass));
        if (!instance) {
            cout << "错误：无法创建类实例" << endl;
            return 1;
        }
        
        // 设置初始值
        Integer* initialValue = factory.createInteger(42);
        instance->setProperty("value", initialValue);
        cout << "创建实例，初始value: " << initialValue->toString() << endl;
        
        // 创建调用scope
        Scope* callScope = new Scope();
        callScope->setVariable("this", instance);
        
        // 测试getValue方法调用
        cout << "\n--- 测试getValue方法调用 ---" << endl;
        MethodReference* getValueRef = new InstanceMethodReference(testClass, instance, "getValue");
        Value* result1 = getValueRef->call(callScope);
        if (result1) {
            cout << "getValue调用成功，返回值: " << result1->toString() << endl;
        } else {
            cout << "getValue调用失败" << endl;
        }
        
        // 测试setValue方法调用
        cout << "\n--- 测试setValue方法调用 ---" << endl;
        Integer* newValue = factory.createInteger(100);
        callScope->setVariable("newValue", newValue);
        
        MethodReference* setValueRef = new InstanceMethodReference(testClass, instance, "setValue");
        Value* result2 = setValueRef->call(callScope);
        if (result2) {
            cout << "setValue调用成功，返回值: " << result2->toString() << endl;
        } else {
            cout << "setValue调用失败" << endl;
        }
        
        // 再次测试getValue方法调用
        cout << "\n--- 再次测试getValue方法调用 ---" << endl;
        Value* result3 = getValueRef->call(callScope);
        if (result3) {
            cout << "getValue调用成功，返回值: " << result3->toString() << endl;
        } else {
            cout << "getValue调用失败" << endl;
        }
        
        cout << "\n=== 测试完成 ===" << endl;
        
        // 清理
        delete callScope;
        delete getValueRef;
        delete setValueRef;
        delete instance;
        delete testClass;
        delete intType;
        // 注意：factory创建的对象会自动管理内存
        
        return 0;
        
    } catch (const exception& e) {
        cout << "异常: " << e.what() << endl;
        return 1;
    }
}
