#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include "interpreter/types/type_registry.h"
#include "interpreter/values/function_types.h"
#include "interpreter/values/value_factory.h"

using namespace std;

// ==================== 测试用的内置函数 ====================

// 数学函数
Value* builtin_add(vector<Value*>& args) {
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

Value* builtin_multiply(vector<Value*>& args) {
    if (args.size() != 2) {
        return new String("Error: multiply function expects 2 arguments");
    }
    
    if (Integer* a = dynamic_cast<Integer*>(args[0])) {
        if (Integer* b = dynamic_cast<Integer*>(args[1])) {
            return new Integer(a->getValue() * b->getValue());
        } else if (Double* b = dynamic_cast<Double*>(args[1])) {
            return new Double(a->getValue() * b->getValue());
        }
    } else if (Double* a = dynamic_cast<Double*>(args[0])) {
        if (Integer* b = dynamic_cast<Integer*>(args[1])) {
            return new Double(a->getValue() * b->getValue());
        } else if (Double* b = dynamic_cast<Double*>(args[1])) {
            return new Double(a->getValue() * b->getValue());
        }
    }
    
    return new String("Error: multiply function expects numeric arguments");
}

// 字符串函数
Value* builtin_concat(vector<Value*>& args) {
    if (args.size() < 2) {
        return new String("Error: concat function expects at least 2 arguments");
    }
    
    string result;
    for (Value* arg : args) {
        result += arg->toString();
    }
    
    return new String(result);
}

Value* builtin_length(vector<Value*>& args) {
    if (args.size() != 1) {
        return new String("Error: length function expects 1 argument");
    }
    
    if (String* str = dynamic_cast<String*>(args[0])) {
        return new Integer(str->getValue().length());
    }
    
    return new String("Error: length function expects string argument");
}

// 打印函数
Value* builtin_print(vector<Value*>& args) {
    cout << "Print function called with " << args.size() << " arguments:" << endl;
    for (size_t i = 0; i < args.size(); ++i) {
        cout << "  Arg " << i << ": " << args[i]->toString() << endl;
    }
    return new String("Print completed");
}

Value* builtin_println(vector<Value*>& args) {
    cout << "Println function called with " << args.size() << " arguments:" << endl;
    for (size_t i = 0; i < args.size(); ++i) {
        cout << "  Arg " << i << ": " << args[i]->toString() << endl;
    }
    cout << endl;  // 额外的换行
    return new String("Println completed");
}

// 类型转换函数
Value* builtin_to_string(vector<Value*>& args) {
    if (args.size() != 1) {
        return new String("Error: to_string function expects 1 argument");
    }
    
    return new String(args[0]->toString());
}

Value* builtin_to_int(vector<Value*>& args) {
    if (args.size() != 1) {
        return new String("Error: to_int function expects 1 argument");
    }
    
    if (String* str = dynamic_cast<String*>(args[0])) {
        try {
            int value = stoi(str->getValue());
            return new Integer(value);
        } catch (...) {
            return new String("Error: Cannot convert string to integer");
        }
    } else if (Double* dbl = dynamic_cast<Double*>(args[0])) {
        return new Integer(static_cast<int>(dbl->getValue()));
    }
    
    return new String("Error: to_int function expects string or double argument");
}

// ==================== 函数管理器类 ====================
class FunctionManager {
private:
    map<string, BuiltinFunction*> builtinFunctions;
    map<string, UserFunction*> userFunctions;
    
public:
    FunctionManager() {
        registerBuiltinFunctions();
    }
    
    ~FunctionManager() {
        // 清理内置函数
        for (auto& pair : builtinFunctions) {
            delete pair.second;
        }
        // 清理用户函数
        for (auto& pair : userFunctions) {
            delete pair.second;
        }
    }
    
    void registerBuiltinFunctions() {
        // 注册数学函数
        builtinFunctions["add"] = new BuiltinFunction("add", builtin_add, {"a", "b"});
        builtinFunctions["multiply"] = new BuiltinFunction("multiply", builtin_multiply, {"a", "b"});
        
        // 注册字符串函数
        builtinFunctions["concat"] = new BuiltinFunction("concat", builtin_concat, {"str1", "str2", "..."});
        builtinFunctions["length"] = new BuiltinFunction("length", builtin_length, {"str"});
        
        // 注册打印函数
        builtinFunctions["print"] = new BuiltinFunction("print", builtin_print, {"value", "..."});
        builtinFunctions["println"] = new BuiltinFunction("println", builtin_println, {"value", "..."});
        
        // 注册类型转换函数
        builtinFunctions["to_string"] = new BuiltinFunction("to_string", builtin_to_string, {"value"});
        builtinFunctions["to_int"] = new BuiltinFunction("to_int", builtin_to_int, {"value"});
        
        cout << "✓ 注册了 " << builtinFunctions.size() << " 个内置函数" << endl;
    }
    
    // 调用内置函数
    Value* callBuiltinFunction(const string& name, vector<Value*>& args) {
        auto it = builtinFunctions.find(name);
        if (it != builtinFunctions.end()) {
            return it->second->call(args);
        }
        return new String("Error: Builtin function '" + name + "' not found");
    }
    
    // 获取内置函数列表
    vector<string> getBuiltinFunctionNames() const {
        vector<string> names;
        for (const auto& pair : builtinFunctions) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    // 显示函数信息
    void showFunctionInfo(const string& name) {
        auto it = builtinFunctions.find(name);
        if (it != builtinFunctions.end()) {
            BuiltinFunction* func = it->second;
            cout << "函数名称: " << func->getName() << endl;
            cout << "函数类型: " << func->getFunctionType() << endl;
            cout << "函数签名: " << func->getSignature() << endl;
            cout << "参数数量: " << func->getParameters().size() << endl;
            if (!func->getParameters().empty()) {
                cout << "参数列表: ";
                for (size_t i = 0; i < func->getParameters().size(); ++i) {
                    if (i > 0) cout << ", ";
                    cout << func->getParameters()[i];
                }
                cout << endl;
            }
        } else {
            cout << "函数 '" << name << "' 未找到" << endl;
        }
    }
};

// ==================== 主函数 ====================
int main() {
    cout << "=== 增强的函数系统演示 ===" << endl;
    
    // 获取全局类型注册表
    TypeRegistry* registry = TypeRegistry::getGlobalInstance();
    if (!registry) {
        cerr << "错误：无法获取全局类型注册表" << endl;
        return 1;
    }
    
    cout << "✓ 全局类型注册表获取成功" << endl;
    
    // 检查function类型是否存在
    ObjectType* functionType = registry->getType("function");
    if (!functionType) {
        cerr << "错误：function类型未找到" << endl;
        return 1;
    }
    
    cout << "✓ function类型注册成功" << endl;
    
    // 创建函数管理器
    FunctionManager funcManager;
    
    // 显示所有可用的内置函数
    cout << "\n=== 可用的内置函数 ===" << endl;
    vector<string> funcNames = funcManager.getBuiltinFunctionNames();
    for (const string& name : funcNames) {
        cout << "- " << name << endl;
    }
    
    // 测试数学函数
    cout << "\n=== 测试数学函数 ===" << endl;
    
    // 测试加法
    vector<Value*> addArgs = {new Integer(10), new Integer(20)};
    Value* addResult = funcManager.callBuiltinFunction("add", addArgs);
    cout << "add(10, 20) = " << addResult->toString() << endl;
    delete addResult;
    
    // 测试乘法
    vector<Value*> mulArgs = {new Integer(5), new Integer(6)};
    Value* mulResult = funcManager.callBuiltinFunction("multiply", mulArgs);
    cout << "multiply(5, 6) = " << mulResult->toString() << endl;
    delete mulResult;
    
    // 测试混合类型
    vector<Value*> mixedArgs = {new Integer(3), new Double(2.5)};
    Value* mixedResult = funcManager.callBuiltinFunction("add", mixedArgs);
    cout << "add(3, 2.5) = " << mixedResult->toString() << endl;
    delete mixedResult;
    
    // 测试字符串函数
    cout << "\n=== 测试字符串函数 ===" << endl;
    
    vector<Value*> concatArgs = {new String("Hello"), new String(" "), new String("World")};
    Value* concatResult = funcManager.callBuiltinFunction("concat", concatArgs);
    cout << "concat('Hello', ' ', 'World') = " << concatResult->toString() << endl;
    delete concatResult;
    
    vector<Value*> lengthArgs = {new String("Hello World")};
    Value* lengthResult = funcManager.callBuiltinFunction("length", lengthArgs);
    cout << "length('Hello World') = " << lengthResult->toString() << endl;
    delete lengthResult;
    
    // 测试类型转换函数
    cout << "\n=== 测试类型转换函数 ===" << endl;
    
    vector<Value*> toStringArgs = {new Integer(42)};
    Value* toStringResult = funcManager.callBuiltinFunction("to_string", toStringArgs);
    cout << "to_string(42) = " << toStringResult->toString() << endl;
    delete toStringResult;
    
    vector<Value*> toIntArgs = {new String("123")};
    Value* toIntResult = funcManager.callBuiltinFunction("to_int", toIntArgs);
    cout << "to_int('123') = " << toIntResult->toString() << endl;
    delete toIntResult;
    
    // 测试打印函数
    cout << "\n=== 测试打印函数 ===" << endl;
    
    vector<Value*> printArgs = {new String("Hello from print function")};
    Value* printResult = funcManager.callBuiltinFunction("print", printArgs);
    cout << "print result: " << printResult->toString() << endl;
    delete printResult;
    
    // 显示函数详细信息
    cout << "\n=== 函数详细信息 ===" << endl;
    funcManager.showFunctionInfo("add");
    cout << endl;
    funcManager.showFunctionInfo("concat");
    
    // 测试错误处理
    cout << "\n=== 测试错误处理 ===" << endl;
    
    // 参数数量错误
    vector<Value*> wrongArgs = {new Integer(1)};
    Value* wrongResult = funcManager.callBuiltinFunction("add", wrongArgs);
    cout << "add(1) = " << wrongResult->toString() << endl;
    delete wrongResult;
    
    // 类型错误
    vector<Value*> typeErrorArgs = {new String("hello"), new String("world")};
    Value* typeErrorResult = funcManager.callBuiltinFunction("add", typeErrorArgs);
    cout << "add('hello', 'world') = " << typeErrorResult->toString() << endl;
    delete typeErrorResult;
    
    // 清理参数
    for (Value* arg : addArgs) delete arg;
    for (Value* arg : mulArgs) delete arg;
    for (Value* arg : mixedArgs) delete arg;
    for (Value* arg : concatArgs) delete arg;
    for (Value* arg : lengthArgs) delete arg;
    for (Value* arg : toStringArgs) delete arg;
    for (Value* arg : toIntArgs) delete arg;
    for (Value* arg : printArgs) delete arg;
    for (Value* arg : wrongArgs) delete arg;
    for (Value* arg : typeErrorArgs) delete arg;
    
    cout << "\n=== 演示完成 ===" << endl;
    return 0;
} 