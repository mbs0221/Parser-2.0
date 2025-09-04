#include "interpreter/plugins/dynamic_library_plugin.h"
#include "interpreter/scope/scope_manager.h"
#include "interpreter/values/value.h"
#include <iostream>
#include <memory>

using namespace DynamicLoader;

int main() {
    std::cout << "动态库加载示例" << std::endl;
    
    // 创建作用域管理器
    ScopeManager scopeManager;
    
    // 方式1: 从配置文件加载
    std::cout << "从配置文件加载数学库..." << std::endl;
    auto math_plugin = std::make_unique<DynamicLibraryPlugin>("configs/math_library.json");
    
    if (math_plugin->isValid()) {
        // 注册函数到作用域管理器
        math_plugin->getLoader()->registerFunctions(scopeManager);
        
        // 创建作用域并测试函数
        auto scope = std::make_unique<Scope>();
        scope->setArgument("x", new Double(1.0));
        
        auto sin_func = scopeManager.getFunction("sin");
        if (sin_func) {
            auto result = sin_func->call(scope.get());
            if (result) {
                std::cout << "sin(1.0) = " << dynamic_cast<Double*>(result)->getValue() << std::endl;
            }
        }
    }
    
    // 方式2: 直接指定库路径加载
    std::cout << "直接加载字符串库..." << std::endl;
    auto string_plugin = std::make_unique<DynamicLibraryPlugin>("/lib/x86_64-linux-gnu/libc.so.6", "string");
    
    if (string_plugin->isValid()) {
        string_plugin->getLoader()->registerFunctions(scopeManager);
        
        // 测试字符串函数
        auto scope = std::make_unique<Scope>();
        scope->setArgument("str", new String("Hello"));
        
        auto strlen_func = scopeManager.getFunction("strlen");
        if (strlen_func) {
            auto result = strlen_func->call(scope.get());
            if (result) {
                std::cout << "strlen(\"Hello\") = " << dynamic_cast<Integer*>(result)->getValue() << std::endl;
            }
        }
    }
    
    return 0;
}
