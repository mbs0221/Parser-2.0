#include "interpreter/interpreter.h"
#include <iostream>

int main() {
    std::cout << "=== 自定义插件目录测试 ===" << std::endl;
    
    // 创建解释器实例，不自动加载插件
    Interpreter interpreter;
    
    // 测试自定义插件目录
    std::cout << "\n1. 测试默认插件目录:" << std::endl;
    interpreter.loadDefaultPlugins("./plugins/");
    
    std::cout << "\n2. 测试不存在的插件目录:" << std::endl;
    interpreter.loadDefaultPlugins("./nonexistent_plugins/");
    
    std::cout << "\n3. 测试当前目录:" << std::endl;
    interpreter.loadDefaultPlugins("./");
    
    std::cout << "\n=== 测试完成 ===" << std::endl;
    return 0;
}
