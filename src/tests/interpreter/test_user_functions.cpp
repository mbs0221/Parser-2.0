#include <iostream>
#include <vector>
#include <string>
#include <map>
#include "interpreter/types/type_registry.h"
#include "interpreter/values/function_types.h"


using namespace std;

// ==================== 简化的用户函数定义结构 ====================
struct SimpleFunctionDefinition {
    string name;
    vector<string> parameters;
    string body;  // 简化的函数体，实际应该是AST
    
    SimpleFunctionDefinition(const string& n, const vector<string>& params, const string& b)
        : name(n), parameters(params), body(b) {}
};

// ==================== 简化的函数执行器 ====================
class SimpleFunctionExecutor : public FunctionExecutor {
private:
    map<string, SimpleFunctionDefinition*> functionDefinitions;
    
public:
    SimpleFunctionExecutor() = default;
    
    // 注册用户函数
    void registerFunction(SimpleFunctionDefinition* funcDef) {
        functionDefinitions[funcDef->name] = funcDef;
        cout << "✓ 注册用户函数: " << funcDef->name << endl;
    }
    
    // 实现FunctionExecutor接口
    Value* executeFunction(const vector<Value*>& args, struct FunctionDefinition* funcDef) override {
        // 这里我们使用简化的实现，实际应该解析AST
        cout << "执行用户函数 (简化实现)" << endl;
        cout << "参数数量: " << args.size() << endl;
        
        // 返回一个简单的字符串结果
        return new String("User function executed successfully");
    }
    
    // 执行简化的用户函数
    Value* executeSimpleFunction(const string& funcName, const vector<Value*>& args) {
        auto it = functionDefinitions.find(funcName);
        if (it == functionDefinitions.end()) {
            return new String("Error: Function '" + funcName + "' not found");
        }
        
        SimpleFunctionDefinition* funcDef = it->second;
        
        // 检查参数数量
        if (args.size() != funcDef->parameters.size()) {
            return new String("Error: Function '" + funcName + "' expects " + 
                            to_string(funcDef->parameters.size()) + " arguments, but got " + 
                            to_string(args.size()));
        }
        
        cout << "执行用户函数: " << funcDef->name << "(";
        for (size_t i = 0; i < funcDef->parameters.size(); ++i) {
            if (i > 0) cout << ", ";
            cout << funcDef->parameters[i] << " = " << args[i]->toString();
        }
        cout << ")" << endl;
        
        cout << "函数体: " << funcDef->body << endl;
        
        // 这里应该执行实际的函数逻辑
        // 现在返回一个模拟的结果
        return new String("User function '" + funcName + "' result");
    }
    
    string getExecutorName() const override {
        return "SimpleFunctionExecutor";
    }
    
    // 获取所有注册的函数
    vector<string> getRegisteredFunctions() const {
        vector<string> names;
        for (const auto& pair : functionDefinitions) {
            names.push_back(pair.first);
        }
        return names;
    }
};

// ==================== 用户函数管理器 ====================
class UserFunctionManager {
private:
    SimpleFunctionExecutor executor;
    map<string, UserFunction*> userFunctions;
    
public:
    UserFunctionManager() = default;
    
    ~UserFunctionManager() {
        // 清理用户函数
        for (auto& pair : userFunctions) {
            delete pair.second;
        }
    }
    
    // 定义用户函数
    bool defineFunction(const string& name, const vector<string>& params, const string& body) {
        // 创建简化的函数定义
        SimpleFunctionDefinition* funcDef = new SimpleFunctionDefinition(name, params, body);
        executor.registerFunction(funcDef);
        
        // 创建UserFunction对象
        // 注意：这里我们传递nullptr作为AST定义，因为我们现在使用简化版本
        UserFunction* userFunc = new UserFunction(name, nullptr, params);
        userFunc->setExecutor(&executor);
        
        userFunctions[name] = userFunc;
        
        cout << "✓ 用户函数 '" << name << "' 定义成功" << endl;
        return true;
    }
    
    // 调用用户函数
    Value* callUserFunction(const string& name, vector<Value*>& args) {
        auto it = userFunctions.find(name);
        if (it == userFunctions.end()) {
            return new String("Error: User function '" + name + "' not found");
        }
        
        UserFunction* userFunc = it->second;
        return userFunc->call(args);
    }
    
    // 获取所有用户函数
    vector<string> getUserFunctionNames() const {
        vector<string> names;
        for (const auto& pair : userFunctions) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    // 显示函数信息
    void showFunctionInfo(const string& name) {
        auto it = userFunctions.find(name);
        if (it != userFunctions.end()) {
            UserFunction* func = it->second;
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
    cout << "=== 用户函数系统演示 ===" << endl;
    
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
    
    // 创建用户函数管理器
    UserFunctionManager userFuncManager;
    
    // 定义一些用户函数
    cout << "\n=== 定义用户函数 ===" << endl;
    
    // 定义简单的数学函数
    userFuncManager.defineFunction("square", {"x"}, "return x * x");
    userFuncManager.defineFunction("power", {"base", "exponent"}, "return base ^ exponent");
    
    // 定义字符串处理函数
    userFuncManager.defineFunction("reverse", {"str"}, "return reverse(str)");
    userFuncManager.defineFunction("repeat", {"str", "count"}, "return str repeated count times");
    
    // 定义逻辑函数
    userFuncManager.defineFunction("max", {"a", "b"}, "return a > b ? a : b");
    userFuncManager.defineFunction("min", {"a", "b"}, "return a < b ? a : b");
    
    // 显示所有可用的用户函数
    cout << "\n=== 可用的用户函数 ===" << endl;
    vector<string> funcNames = userFuncManager.getUserFunctionNames();
    for (const string& name : funcNames) {
        cout << "- " << name << endl;
    }
    
    // 测试用户函数调用
    cout << "\n=== 测试用户函数调用 ===" << endl;
    
    // 测试square函数
    vector<Value*> squareArgs = {new Integer(5)};
    Value* squareResult = userFuncManager.callUserFunction("square", squareArgs);
    cout << "square(5) = " << squareResult->toString() << endl;
    delete squareResult;
    
    // 测试power函数
    vector<Value*> powerArgs = {new Integer(2), new Integer(3)};
    Value* powerResult = userFuncManager.callUserFunction("power", powerArgs);
    cout << "power(2, 3) = " << powerResult->toString() << endl;
    delete powerResult;
    
    // 测试max函数
    vector<Value*> maxArgs = {new Integer(10), new Integer(20)};
    Value* maxResult = userFuncManager.callUserFunction("max", maxArgs);
    cout << "max(10, 20) = " << maxResult->toString() << endl;
    delete maxResult;
    
    // 测试字符串函数
    vector<Value*> reverseArgs = {new String("Hello")};
    Value* reverseResult = userFuncManager.callUserFunction("reverse", reverseArgs);
    cout << "reverse('Hello') = " << reverseResult->toString() << endl;
    delete reverseResult;
    
    // 显示函数详细信息
    cout << "\n=== 函数详细信息 ===" << endl;
    userFuncManager.showFunctionInfo("square");
    cout << endl;
    userFuncManager.showFunctionInfo("power");
    
    // 测试错误处理
    cout << "\n=== 测试错误处理 ===" << endl;
    
    // 参数数量错误
    vector<Value*> wrongArgs = {new Integer(1)};
    Value* wrongResult = userFuncManager.callUserFunction("power", wrongArgs);
    cout << "power(1) = " << wrongResult->toString() << endl;
    delete wrongResult;
    
    // 函数不存在
    vector<Value*> notExistArgs = {new Integer(1)};
    Value* notExistResult = userFuncManager.callUserFunction("nonexistent", notExistArgs);
    cout << "nonexistent(1) = " << notExistResult->toString() << endl;
    delete notExistResult;
    
    // 清理参数
    for (Value* arg : squareArgs) delete arg;
    for (Value* arg : powerArgs) delete arg;
    for (Value* arg : maxArgs) delete arg;
    for (Value* arg : reverseArgs) delete arg;
    for (Value* arg : wrongArgs) delete arg;
    for (Value* arg : notExistArgs) delete arg;
    
    cout << "\n=== 演示完成 ===" << endl;
    return 0;
} 