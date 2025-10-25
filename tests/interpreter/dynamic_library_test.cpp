#include "dynamic_library_plugin.h"
#include "interpreter/scope/scope_manager.h"
#include "interpreter/values/value.h"
#include "common/logger.h"
#include <iostream>
#include <memory>

using namespace DynamicLoader;

int main() {
    std::cout << "=== 动态库加载测试 ===" << std::endl;
    
    try {
        // 创建作用域管理器
        ScopeManager scopeManager;
        
        // 测试数学库加载
        std::cout << "\n1. 加载数学库..." << std::endl;
        auto math_plugin = std::make_unique<DynamicLibraryPlugin>("configs/math_library.json");
        
        if (math_plugin->isValid()) {
            std::cout << "✓ 数学库加载成功" << std::endl;
            
            // 获取插件信息
            auto info = math_plugin->getPluginInfo();
            std::cout << "库名称: " << info.name << std::endl;
            std::cout << "描述: " << info.description << std::endl;
            std::cout << "函数数量: " << info.functions.size() << std::endl;
            
            // 注册函数
            math_plugin->getLoader()->registerFunctions(scopeManager);
            std::cout << "✓ 数学函数注册成功" << std::endl;
            
            // 测试函数调用
            std::cout << "\n2. 测试数学函数调用..." << std::endl;
            
            // 创建测试作用域
            auto scope = std::make_unique<Scope>();
            
            // 测试sin函数
            auto sin_value = new Double(3.14159 / 2); // π/2
            scope->setArgument("x", sin_value);
            
            auto sin_func = scopeManager.getFunction("sin");
            if (sin_func) {
                auto result = sin_func->call(scope.get());
                if (result) {
                    std::cout << "sin(π/2) = " << dynamic_cast<Double*>(result)->getValue() << std::endl;
                }
            }
            
            // 测试sqrt函数
            auto sqrt_value = new Double(16.0);
            scope->setArgument("x", sqrt_value);
            
            auto sqrt_func = scopeManager.getFunction("sqrt");
            if (sqrt_func) {
                auto result = sqrt_func->call(scope.get());
                if (result) {
                    std::cout << "sqrt(16) = " << dynamic_cast<Double*>(result)->getValue() << std::endl;
                }
            }
            
        } else {
            std::cout << "✗ 数学库加载失败" << std::endl;
        }
        
        // 测试字符串库加载
        std::cout << "\n3. 加载字符串库..." << std::endl;
        auto string_plugin = std::make_unique<DynamicLibraryPlugin>("configs/string_library.json");
        
        if (string_plugin->isValid()) {
            std::cout << "✓ 字符串库加载成功" << std::endl;
            
            // 注册函数
            string_plugin->getLoader()->registerFunctions(scopeManager);
            std::cout << "✓ 字符串函数注册成功" << std::endl;
            
            // 测试字符串函数
            std::cout << "\n4. 测试字符串函数调用..." << std::endl;
            
            auto test_scope = std::make_unique<Scope>();
            
            // 测试strlen函数
            auto str_value = new String("Hello, World!");
            test_scope->setArgument("str", str_value);
            
            auto strlen_func = scopeManager.getFunction("strlen");
            if (strlen_func) {
                auto result = strlen_func->call(test_scope.get());
                if (result) {
                    std::cout << "strlen(\"Hello, World!\") = " << dynamic_cast<Integer*>(result)->getValue() << std::endl;
                }
            }
            
            // 测试strcmp函数
            auto str1_value = new String("hello");
            auto str2_value = new String("world");
            test_scope->setArgument("str1", str1_value);
            test_scope->setArgument("str2", str2_value);
            
            auto strcmp_func = scopeManager.getFunction("strcmp");
            if (strcmp_func) {
                auto result = strcmp_func->call(test_scope.get());
                if (result) {
                    std::cout << "strcmp(\"hello\", \"world\") = " << dynamic_cast<Integer*>(result)->getValue() << std::endl;
                }
            }
            
        } else {
            std::cout << "✗ 字符串库加载失败" << std::endl;
        }
        
        std::cout << "\n=== 测试完成 ===" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "测试过程中出错: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
