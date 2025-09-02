// 改进的方法访问系统示例
// 展示如何通过函数签名进行精确的方法匹配

#include <iostream>
#include <string>
#include <vector>

/*
// 在用户代码中，可以这样定义重载方法：

class Calculator {
    // 重载的 add 方法
    public int add(int a, int b) {
        return a + b;
    }
    
    public double add(double a, double b) {
        return a + b;
    }
    
    public string add(string a, string b) {
        return a + b;
    }
    
    // 重载的 multiply 方法
    public int multiply(int a, int b) {
        return a * b;
    }
    
    public int multiply(int a, int b, int c) {
        return a * b * c;
    }
    
    // 带默认参数的方法
    public void print(string message, string prefix = "INFO: ") {
        print(prefix + message);
    }
}

// 使用示例：
Calculator calc = new Calculator();

// 这些调用现在会通过函数签名精确匹配：
calc.add(5, 3);           // 匹配 add(int, int)
calc.add(3.14, 2.86);     // 匹配 add(double, double)
calc.add("Hello", "World"); // 匹配 add(string, string)

calc.multiply(2, 3);      // 匹配 multiply(int, int)
calc.multiply(2, 3, 4);   // 匹配 multiply(int, int, int)

calc.print("Hello");       // 匹配 print(string, string) - 使用默认参数
calc.print("Hello", "DEBUG: "); // 匹配 print(string, string)
*/

// 系统内部实现示例
class ImprovedMethodAccessExample {
public:
    void demonstrateMethodMatching() {
        cout << "=== 改进的方法访问系统演示 ===" << endl;
        
        // 1. 创建类型和方法
        ClassType* calculatorType = new ClassType("Calculator");
        
        // 注册重载的 add 方法
        calculatorType->addUserMethod(
            FunctionSignature("add", {"int", "int"}),
            new UserFunction("add", {"int", "int"}, nullptr)
        );
        
        calculatorType->addUserMethod(
            FunctionSignature("add", {"double", "double"}),
            new UserFunction("add", {"double", "double"}, nullptr)
        );
        
        calculatorType->addUserMethod(
            FunctionSignature("add", {"string", "string"}),
            new UserFunction("add", {"string", "string"}, nullptr)
        );
        
        // 注册重载的 multiply 方法
        calculatorType->addUserMethod(
            FunctionSignature("multiply", {"int", "int"}),
            new UserFunction("multiply", {"int", "int"}, nullptr)
        );
        
        calculatorType->addUserMethod(
            FunctionSignature("multiply", {"int", "int", "int"}),
            new UserFunction("multiply", {"int", "int", "int"}, nullptr)
        );
        
        // 2. 创建实例
        Value* calculator = new String("Calculator"); // 简化，实际应该是 Calculator 实例
        
        // 3. 访问方法（现在返回 MethodReference）
        Value* addMethod = accessMethod(calculator, calculatorType, "add");
        Value* multiplyMethod = accessMethod(calculator, calculatorType, "multiply");
        
        cout << "访问方法 'add': " << addMethod->toString() << endl;
        cout << "访问方法 'multiply': " << multiplyMethod->toString() << endl;
        
        // 4. 调用方法（现在会进行精确匹配）
        testMethodCall(addMethod, "add", {createIntValue(5), createIntValue(3)});
        testMethodCall(addMethod, "add", {createDoubleValue(3.14), createDoubleValue(2.86)});
        testMethodCall(addMethod, "add", {createStringValue("Hello"), createStringValue("World")});
        
        testMethodCall(multiplyMethod, "multiply", {createIntValue(2), createIntValue(3)});
        testMethodCall(multiplyMethod, "multiply", {createIntValue(2), createIntValue(3), createIntValue(4)});
        
        cout << "=== 演示结束 ===" << endl;
    }
    
private:
    // 模拟方法访问
    Value* accessMethod(Value* instance, ObjectType* type, const string& methodName) {
        if (type->supportsMethods()) {
            IMethodSupport* methodSupport = dynamic_cast<IMethodSupport*>(type);
            if (methodSupport && methodSupport->hasUserMethod(methodName)) {
                // 返回 MethodReference，延迟解析
                return new MethodReference(type, instance, methodName, false);
            }
        }
        return nullptr;
    }
    
    // 测试方法调用
    void testMethodCall(Value* method, const string& name, const vector<Value*>& args) {
        cout << "调用方法: " << name << "(";
        for (size_t i = 0; i < args.size(); ++i) {
            if (i > 0) cout << ", ";
            cout << args[i]->getValueType()->getTypeName();
        }
        cout << ")" << endl;
        
        if (Function* func = dynamic_cast<Function*>(method)) {
            if (MethodReference* methodRef = dynamic_cast<MethodReference*>(func)) {
                cout << "  使用 MethodReference 进行精确匹配" << endl;
                cout << "  目标类型: " << methodRef->getTargetType()->getTypeName() << endl;
                cout << "  方法名称: " << methodRef->getMethodName() << endl;
                cout << "  是否为静态: " << (methodRef->isStaticMethod() ? "是" : "否") << endl;
                
                // 调用方法（会进行精确匹配）
                Value* result = methodRef->call(args);
                if (result) {
                    cout << "  调用成功，结果类型: " << result->getValueType()->getTypeName() << endl;
                } else {
                    cout << "  调用失败，没有找到匹配的方法签名" << endl;
                }
            } else {
                cout << "  使用传统函数调用" << endl;
                func->call(args);
            }
        }
        
        cout << endl;
    }
    
    // 辅助方法：创建测试用的值对象
    Value* createIntValue(int value) {
        // 这里应该创建实际的Integer对象
        return nullptr;
    }
    
    Value* createDoubleValue(double value) {
        // 这里应该创建实际的Double对象
        return nullptr;
    }
    
    Value* createStringValue(const string& value) {
        // 这里应该创建实际的String对象
        return nullptr;
    }
};

int main() {
    ImprovedMethodAccessExample example;
    example.demonstrateMethodMatching();
    return 0;
}
