#include <iostream>
#include <string>
#include <vector>
#include <functional>

using namespace std;

// 简化的Value基类
class Value {
protected:
    string valueType;

public:
    Value(const string& type) : valueType(type) {}
    virtual ~Value() = default;
    
    virtual string toString() const = 0;
    string getValueType() const { return valueType; }
};

// 简化的Integer类
class Integer : public Value {
private:
    int value;

public:
    Integer(int val) : Value("int"), value(val) {}
    
    int getValue() const { return value; }
    string toString() const override {
        return to_string(value);
    }
};

// 简化的String类
class String : public Value {
private:
    string value;

public:
    String(const string& val) : Value("string"), value(val) {}
    
    string getValue() const { return value; }
    string toString() const override {
        return value;
    }
};

// 函数执行器接口
class FunctionExecutor {
public:
    virtual ~FunctionExecutor() = default;
    
    // 执行用户函数
    virtual Value* executeFunction(const vector<Value*>& args, const string& funcName) = 0;
    
    // 获取执行器名称
    virtual string getExecutorName() const = 0;
};

// 简化的Function基类
class Function : public Value {
protected:
    string name;
    vector<string> parameters;

public:
    Function(const string& funcName, const vector<string>& params = {})
        : Value("function"), name(funcName), parameters(params) {}
    
    virtual ~Function() = default;
    
    string getName() const { return name; }
    const vector<string>& getParameters() const { return parameters; }
    
    virtual Value* call(vector<Value*> args) = 0;
    virtual string getFunctionType() const = 0;
};

// 简化的BuiltinFunction类
class BuiltinFunction : public Function {
private:
    function<Value*(vector<Value*>)> func;

public:
    BuiltinFunction(const string& funcName, function<Value*(vector<Value*>)> f, const vector<string>& params = {})
        : Function(funcName, params), func(f) {}
    
    Value* call(vector<Value*> args) override {
        if (func) {
            try {
                return func(args);
            } catch (const exception& e) {
                cout << "Error calling builtin function '" << name << "': " << e.what() << endl;
                return nullptr;
            }
        }
        return nullptr;
    }
    
    string getFunctionType() const override { return "builtin"; }
};

// 简化的UserFunction类（使用依赖注入）
class UserFunction : public Function {
private:
    string body;                    // 简化的函数体表示
    FunctionExecutor* executor;     // 函数执行器（依赖注入）

public:
    UserFunction(const string& funcName, const string& funcBody, const vector<string>& params = {})
        : Function(funcName, params), body(funcBody), executor(nullptr) {}
    
    Value* call(vector<Value*> args) override {
        // 检查参数数量
        if (args.size() != parameters.size()) {
            cout << "Error: Function '" << name << "' expects " << parameters.size() 
                 << " arguments, but got " << args.size() << endl;
            return nullptr;
        }
        
        // 检查是否有执行器
        if (!executor) {
            cout << "Error: No executor available for function '" << name << "'" << endl;
            cout << "Hint: Use setExecutor() to provide a FunctionExecutor" << endl;
            return nullptr;
        }
        
        // 通过执行器执行函数
        try {
            Value* result = executor->executeFunction(args, name);
            if (result) {
                cout << "User function '" << name << "' executed successfully using " 
                     << executor->getExecutorName() << endl;
                return result;
            } else {
                cout << "Warning: Function executor returned null for function '" << name << "'" << endl;
                return new String("Function execution returned null");
            }
        } catch (const exception& e) {
            cout << "Error executing user function '" << name << "': " << e.what() << endl;
            return new String("Function execution error: " + string(e.what()));
        }
    }
    
    string getFunctionType() const override { return "user"; }
    
    // 设置函数执行器
    void setExecutor(FunctionExecutor* exec) { executor = exec; }
    
    // 获取函数执行器
    FunctionExecutor* getExecutor() const { return executor; }
};

// 具体的函数执行器实现
class InterpreterFunctionExecutor : public FunctionExecutor {
private:
    string interpreterName;

public:
    InterpreterFunctionExecutor(const string& name = "DefaultInterpreter") 
        : interpreterName(name) {}
    
    Value* executeFunction(const vector<Value*>& args, const string& funcName) override {
        cout << "InterpreterFunctionExecutor: Executing function '" << funcName << "' with " 
             << args.size() << " arguments" << endl;
        
        // 模拟函数执行逻辑
        cout << "Parameters: ";
        for (size_t i = 0; i < args.size(); ++i) {
            cout << args[i]->toString();
            if (i < args.size() - 1) cout << ", ";
        }
        cout << endl;
        
        // 返回一个模拟的结果
        return new String("Function '" + funcName + "' executed by " + interpreterName);
    }
    
    string getExecutorName() const override { return interpreterName; }
};

// 另一个执行器实现（用于演示多态性）
class DebugFunctionExecutor : public FunctionExecutor {
public:
    Value* executeFunction(const vector<Value*>& args, const string& funcName) override {
        cout << "=== DEBUG MODE ===" << endl;
        cout << "Function: " << funcName << endl;
        cout << "Arguments: " << args.size() << endl;
        
        for (size_t i = 0; i < args.size(); ++i) {
            cout << "  Arg[" << i << "]: " << args[i]->toString() 
                 << " (type: " << args[i]->getValueType() << ")" << endl;
        }
        
        cout << "=== END DEBUG ===" << endl;
        return new String("Debug execution completed for " + funcName);
    }
    
    string getExecutorName() const override { return "DebugFunctionExecutor"; }
};

// 测试用的内置函数
Value* test_add(vector<Value*>& args) {
    if (args.size() != 2) {
        return new String("Error: add function expects 2 arguments");
    }
    
    if (Integer* a = dynamic_cast<Integer*>(args[0])) {
        if (Integer* b = dynamic_cast<Integer*>(args[1])) {
            return new Integer(a->getValue() + b->getValue());
        }
    }
    
    return new String("Error: add function expects integer arguments");
}

int main() {
    cout << "=== 依赖注入模式演示 ===" << endl;
    
    // 1. 创建内置函数
    cout << "\n--- 内置函数测试 ---" << endl;
    
    BuiltinFunction* addFunc = new BuiltinFunction("add", test_add, {"a", "b"});
    cout << "创建内置函数: " << addFunc->getName() << " (" << addFunc->getFunctionType() << ")" << endl;
    
    // 测试内置函数
    vector<Value*> args1 = {new Integer(5), new Integer(3)};
    Value* result1 = addFunc->call(args1);
    if (result1) {
        cout << "add(5, 3) = " << result1->toString() << endl;
        delete result1;
    }
    
    // 2. 创建用户函数（无执行器）
    cout << "\n--- 用户函数测试（无执行器）---" << endl;
    
    UserFunction* userFunc1 = new UserFunction("greet", "print greeting message", {"name", "age"});
    cout << "创建用户函数: " << userFunc1->getName() << " (" << userFunc1->getFunctionType() << ")" << endl;
    
    // 尝试调用（应该失败）
    vector<Value*> args2 = {new String("Alice"), new Integer(25)};
    Value* result2 = userFunc1->call(args2);
    if (result2) {
        cout << "用户函数返回: " << result2->toString() << endl;
        delete result2;
    }
    
    // 3. 使用依赖注入创建用户函数
    cout << "\n--- 依赖注入测试 ---" << endl;
    
    // 创建执行器
    FunctionExecutor* executor1 = new InterpreterFunctionExecutor("MainInterpreter");
    FunctionExecutor* executor2 = new DebugFunctionExecutor();
    
    // 创建用户函数并注入执行器
    UserFunction* userFunc2 = new UserFunction("calculate", "perform calculation", {"x", "y"});
    userFunc2->setExecutor(executor1);
    
    UserFunction* userFunc3 = new UserFunction("debug_func", "debug function", {"data"});
    userFunc3->setExecutor(executor2);
    
    cout << "用户函数已注入执行器:" << endl;
    cout << "  " << userFunc2->getName() << " -> " << userFunc2->getExecutor()->getExecutorName() << endl;
    cout << "  " << userFunc3->getName() << " -> " << userFunc3->getExecutor()->getExecutorName() << endl;
    
    // 测试执行器注入后的函数调用
    vector<Value*> args3 = {new Integer(10), new Integer(20)};
    Value* result3 = userFunc2->call(args3);
    if (result3) {
        cout << "calculate函数返回: " << result3->toString() << endl;
        delete result3;
    }
    
    vector<Value*> args4 = {new String("test data")};
    Value* result4 = userFunc3->call(args4);
    if (result4) {
        cout << "debug_func函数返回: " << result4->toString() << endl;
        delete result4;
    }
    
    // 4. 演示执行器切换
    cout << "\n--- 执行器切换演示 ---" << endl;
    
    cout << "将calculate函数的执行器从 " << userFunc2->getExecutor()->getExecutorName() 
         << " 切换到 " << executor2->getExecutorName() << endl;
    
    userFunc2->setExecutor(executor2);
    
    // 再次调用，应该使用新的执行器
    Value* result5 = userFunc2->call(args3);
    if (result5) {
        cout << "切换执行器后的结果: " << result5->toString() << endl;
        delete result5;
    }
    
    // 5. 测试函数克隆（执行器引用应该被复制）
    cout << "\n--- 函数克隆测试 ---" << endl;
    
    UserFunction* clonedFunc = new UserFunction(*userFunc2);  // 假设有拷贝构造函数
    cout << "克隆函数: " << clonedFunc->getName() << endl;
    cout << "克隆函数的执行器: " << clonedFunc->getExecutor()->getExecutorName() << endl;
    
    // 清理资源
    delete addFunc;
    delete userFunc1;
    delete userFunc2;
    delete userFunc3;
    delete clonedFunc;
    delete executor1;
    delete executor2;
    
    // 清理测试参数
    for (Value* arg : args1) delete arg;
    for (Value* arg : args2) delete arg;
    for (Value* arg : args3) delete arg;
    for (Value* arg : args4) delete arg;
    
    cout << "\n=== 演示完成 ===" << endl;
    return 0;
} 