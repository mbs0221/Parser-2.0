# 插件系统使用指南

## 概述

本解释器现在支持动态库插件系统，允许像Python一样动态扩展解释器功能。插件系统基于动态库（.so文件）实现，可以在运行时加载和卸载。

## 插件系统架构

### 核心组件

1. **BuiltinPlugin接口** (`include/interpreter/builtin_plugin.h`)
   - 定义插件的基本接口
   - 包含插件信息结构和函数注册方法

2. **PluginManager类** (`src/interpreter/plugins/plugin_manager.cpp`)
   - 管理插件的加载和卸载
   - 处理动态库的打开和关闭
   - 注册插件函数到解释器作用域

3. **解释器集成** (`src/interpreter/interpreter.cpp`)
   - 在解释器中集成插件管理器
   - 提供插件加载和卸载的公共接口

## 现有插件

### 1. 基础内置函数插件 (basic_builtin_plugin)
- **功能**: 提供基础的内置函数
- **函数**: print, count, cin, abs, max, min, pow, length, substring, upper, lower, push, pop, sort, to_string, to_int, to_double, cast, random, exit
- **文件**: `src/interpreter/plugins/basic_builtin_plugin.cpp`

### 2. 数学函数插件 (math_plugin)
- **功能**: 提供高级数学运算函数
- **函数**: sin, cos, tan, sqrt, log, floor, ceil
- **文件**: `src/interpreter/plugins/math_plugin.cpp`

### 3. 字符串处理插件 (string_plugin)
- **功能**: 提供字符串处理函数
- **函数**: trim, replace, split, join, starts_with, ends_with, contains
- **文件**: `src/interpreter/plugins/string_plugin.cpp`

## 构建插件系统

### 编译项目
```bash
cd Parser-2.0
mkdir -p build
cd build
cmake ..
make
```

### 插件输出位置
编译后，插件文件将位于 `build/plugins/` 目录：
- `libbasic_builtin_plugin.so`
- `libmath_plugin.so`
- `libstring_plugin.so`

## 使用插件

### 在代码中使用插件函数

```cpp
// 创建解释器（会自动加载内置插件）
Interpreter interpreter;

// 使用基础函数
print("Hello World");
let result = max(10, 20);

// 使用数学函数
let angle = 3.14159 / 4;
let sinValue = sin(angle);
let sqrtValue = sqrt(16);

// 使用字符串函数
let text = "  Hello World  ";
let trimmed = trim(text);
let upperText = upper(text);
```

### 动态加载插件

```cpp
// 加载自定义插件
interpreter.loadPlugin("./my_custom_plugin.so");

// 卸载插件
interpreter.unloadPlugin("my_plugin_name");

// 获取已加载的插件列表
vector<string> plugins = interpreter.getLoadedPlugins();
```

## 创建自定义插件

### 1. 创建插件源文件

```cpp
#include "interpreter/builtin_plugin.h"
#include "lexer/value.h"
#include "parser/function.h"
#include "interpreter/scope.h"

using namespace std;

// 自定义函数实现
Value* my_custom_function(vector<Variable*>& args) {
    // 函数实现
    return new String("Hello from custom plugin!");
}

// 插件类
class MyCustomPlugin : public BuiltinPlugin {
public:
    PluginInfo getPluginInfo() const override {
        return PluginInfo{
            "my_custom_plugin",
            "1.0.0",
            "我的自定义插件",
            {"my_custom_function"}
        };
    }
    
    void registerFunctions(ScopeManager& scopeManager) override {
        scopeManager.defineIdentifier("my_custom_function", 
            new BuiltinFunction("my_custom_function", my_custom_function));
    }
    
    map<string, BuiltinFunctionPtr> getFunctionMap() const override {
        return {{"my_custom_function", my_custom_function}};
    }
};

// 导出插件
EXPORT_PLUGIN(MyCustomPlugin)
```

### 2. 创建CMakeLists.txt

```cmake
add_library(my_custom_plugin SHARED
    my_custom_plugin.cpp
)

target_include_directories(my_custom_plugin PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/Parser
)

target_link_libraries(my_custom_plugin parser)

set_target_properties(my_custom_plugin PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
)
```

### 3. 编译插件

```bash
cd build
make my_custom_plugin
```

## 插件系统优势

1. **模块化**: 功能按模块分离，便于维护
2. **可扩展性**: 可以轻松添加新功能而不需要修改核心代码
3. **动态加载**: 可以在运行时加载和卸载插件
4. **类型安全**: 使用C++类型系统确保类型安全
5. **性能**: 插件编译为本地代码，性能优异

## 注意事项

1. **ABI兼容性**: 插件必须与主程序使用相同的编译器和C++标准
2. **内存管理**: 插件中的对象由主程序管理，注意避免内存泄漏
3. **错误处理**: 插件函数应该妥善处理错误情况
4. **线程安全**: 如果多线程使用，需要考虑线程安全问题

## 测试插件系统

运行测试程序：
```bash
cd build
./bin/test_plugin_system
```

这将测试所有已加载的插件功能。
