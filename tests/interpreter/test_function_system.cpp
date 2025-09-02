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

// 简化的UserFunction类
class UserFunction : public Function {
private:
    string body; // 简化的函数体表示

public:
    UserFunction(const string& funcName, const string& funcBody, const vector<string>& params = {})
        : Function(funcName, params), body(funcBody) {}
    
    Value* call(vector<Value*> args) override {
        // 检查参数数量
        if (args.size() != parameters.size()) {
            cout << "Error: Function '" << name << "' expects " << parameters.size() 
                 << " arguments, but got " << args.size() << endl;
            return nullptr;
        }
        
        // 简化的执行逻辑：打印函数调用信息
        cout << "Executing user function '" << name << "' with body: " << body << endl;
        cout << "Parameters: ";
        for (size_t i = 0; i < parameters.size(); ++i) {
            cout << parameters[i] << "=" << args[i]->toString();
            if (i < parameters.size() - 1) cout << ", ";
        }
        cout << endl;
        
        // 返回一个简单的结果
        return new String("User function executed successfully");
    }
    
    string getFunctionType() const override { return "user"; }
};

// 测试用的内置函数
Value* test_add(vector<Value*>& args) {
    if (args.size() != 2) {
        return new String("Error: add function expects 2 arguments");
    }
    
    // 尝试转换为数值类型
    if (Integer* a = dynamic_cast<Integer*>(args[0])) {
        if (Integer* b = dynamic_cast<Integer*>(args[1])) {
            return new Integer(a->getValue() + b->getValue());
        }
    }
    
    return new String("Error: add function expects integer arguments");
}

Value* test_print(vector<Value*>& args) {
    cout << "Print function called with " << args.size() << " arguments:" << endl;
    for (size_t i = 0; i < args.size(); ++i) {
        cout << "  Arg " << i << ": " << args[i]->toString() << endl;
    }
    return new String("Print completed");
}

int main() {
    cout << "=== 函数系统演示 ===" << endl;
    
    // 1. 测试内置函数
    cout << "\n--- 内置函数测试 ---" << endl;
    
    BuiltinFunction* addFunc = new BuiltinFunction("add", test_add, {"a", "b"});
    cout << "创建内置函数: " << addFunc->getName() << " (" << addFunc->getFunctionType() << ")" << endl;
    
    // 测试加法函数
    vector<Value*> args1 = {new Integer(5), new Integer(3)};
    Value* result1 = addFunc->call(args1);
    if (result1) {
        cout << "add(5, 3) = " << result1->toString() << endl;
        delete result1;
    }
    
    // 测试打印函数
    BuiltinFunction* printFunc = new BuiltinFunction("print", test_print, {"..."});
    vector<Value*> args2 = {new String("Hello"), new Integer(42)};
    Value* result2 = printFunc->call(args2);
    if (result2) {
        cout << "print函数返回: " << result2->toString() << endl;
        delete result2;
    }
    
    // 2. 测试用户函数
    cout << "\n--- 用户函数测试 ---" << endl;
    
    UserFunction* userFunc = new UserFunction("greet", "print greeting message", {"name", "age"});
    cout << "创建用户函数: " << userFunc->getName() << " (" << userFunc->getFunctionType() << ")" << endl;
    
    // 测试用户函数调用
    vector<Value*> args3 = {new String("Alice"), new Integer(25)};
    Value* result3 = userFunc->call(args3);
    if (result3) {
        cout << "用户函数返回: " << result3->toString() << endl;
        delete result3;
    }
    
    // 3. 测试函数类型系统
    cout << "\n--- 函数类型系统测试 ---" << endl;
    
    vector<Function*> functions = {addFunc, printFunc, userFunc};
    
    for (Function* func : functions) {
        cout << "函数: " << func->getName() 
             << ", 类型: " << func->getValueType()
             << ", 函数类型: " << func->getFunctionType()
             << ", 参数数量: " << func->getParameters().size() << endl;
    }
    
    // 4. 测试错误处理
    cout << "\n--- 错误处理测试 ---" << endl;
    
    // 测试参数数量不匹配
    vector<Value*> wrongArgs = {new Integer(1)};
    Value* errorResult = addFunc->call(wrongArgs);
    if (errorResult) {
        cout << "错误处理结果: " << errorResult->toString() << endl;
        delete errorResult;
    }
    
    // 清理资源
    delete addFunc;
    delete printFunc;
    delete userFunc;
    
    // 清理测试参数
    for (Value* arg : args1) delete arg;
    for (Value* arg : args2) delete arg;
    for (Value* arg : args3) delete arg;
    for (Value* arg : wrongArgs) delete arg;
    
    cout << "\n=== 演示完成 ===" << endl;
    return 0;
} 