# 动态C库加载系统

## 概述

动态C库加载系统允许解释器在运行时直接加载C库中的符号，并将其封装为内置函数。这种方式避免了代码生成，提供了更灵活和高效的C库集成方案。

## 核心特性

- **动态符号加载**: 使用`dlopen`/`dlsym`直接加载C库中的函数符号
- **类型安全转换**: 自动处理C类型与解释器类型之间的转换
- **配置驱动**: 通过JSON配置文件定义要加载的函数
- **插件集成**: 完全兼容现有的插件系统
- **错误处理**: 完善的错误处理和日志记录

## 系统架构

```
┌─────────────────────────────────────────────────────────────┐
│                    DynamicLibraryPlugin                     │
│  ┌─────────────────────────────────────────────────────────┐│
│  │              DynamicLibraryLoader                       ││
│  │  ┌─────────────────┐  ┌─────────────────────────────────┐││
│  │  │   Symbol Loader │  │      Function Wrapper          │││
│  │  │  (dlopen/dlsym) │  │   (Type Conversion)            │││
│  │  └─────────────────┘  └─────────────────────────────────┘││
│  └─────────────────────────────────────────────────────────┘│
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
                    ┌─────────────────┐
                    │  ScopeManager   │
                    │  (Function      │
                    │   Registration) │
                    └─────────────────┘
```

## 使用方法

### 1. 配置文件方式

```cpp
#include "interpreter/plugins/dynamic_library_plugin.h"

// 从配置文件加载
auto plugin = std::make_unique<DynamicLibraryPlugin>("configs/math_library.json");

if (plugin->isValid()) {
    // 注册函数到作用域管理器
    plugin->getLoader()->registerFunctions(scopeManager);
}
```

### 2. 直接指定库路径

```cpp
// 直接指定库路径和名称
auto plugin = std::make_unique<DynamicLibraryPlugin>("/usr/lib/libm.so", "math");

if (plugin->isValid()) {
    plugin->getLoader()->registerFunctions(scopeManager);
}
```

### 3. 配置文件格式

```json
{
  "library_name": "math",
  "library_path": "/usr/lib/x86_64-linux-gnu/libm.so",
  "description": "标准C数学库的动态加载插件",
  "header_files": ["math.h"],
  "functions": [
    {
      "name": "sin",
      "c_signature": "double sin(double x)",
      "description": "计算正弦值",
      "parameters": [
        {
          "name": "x",
          "type": "double",
          "is_pointer": false,
          "is_const": false,
          "description": "角度值（弧度）"
        }
      ],
      "return_type": "double",
      "is_void_return": false
    }
  ],
  "type_mappings": {
    "double": "Double",
    "int": "Integer",
    "char*": "String",
    "void": "null"
  }
}
```

## 支持的类型映射

| C类型 | 解释器类型 | 说明 |
|-------|------------|------|
| `double` | `Double` | 双精度浮点数 |
| `int` | `Integer` | 整数 |
| `size_t` | `Integer` | 大小类型 |
| `char*` | `String` | 字符串指针 |
| `const char*` | `String` | 常量字符串指针 |
| `void` | `null` | 空类型 |

## 函数包装器

系统自动为每个C函数创建包装器，处理以下任务：

1. **参数提取**: 从`Scope`中提取参数并转换为C类型
2. **函数调用**: 调用实际的C函数
3. **返回值转换**: 将C函数返回值转换为解释器类型
4. **错误处理**: 捕获异常并记录日志

## 示例

### 数学库示例

```cpp
// 加载数学库
auto math_plugin = std::make_unique<DynamicLibraryPlugin>("configs/math_library.json");

// 测试sin函数
auto scope = std::make_unique<Scope>();
scope->setArgument("x", new Double(1.0));

auto sin_func = scopeManager.getFunction("sin");
auto result = sin_func->call(scope.get());
std::cout << "sin(1.0) = " << dynamic_cast<Double*>(result)->getValue() << std::endl;
```

### 字符串库示例

```cpp
// 加载字符串库
auto string_plugin = std::make_unique<DynamicLibraryPlugin>("configs/string_library.json");

// 测试strlen函数
auto scope = std::make_unique<Scope>();
scope->setArgument("str", new String("Hello, World!"));

auto strlen_func = scopeManager.getFunction("strlen");
auto result = strlen_func->call(scope.get());
std::cout << "strlen = " << dynamic_cast<Integer*>(result)->getValue() << std::endl;
```

## 优势

1. **无需代码生成**: 直接加载C库符号，避免生成中间代码
2. **运行时灵活性**: 可以在运行时动态加载不同的库
3. **类型安全**: 自动处理类型转换，减少错误
4. **配置驱动**: 通过配置文件轻松管理要加载的函数
5. **插件兼容**: 完全兼容现有的插件系统
6. **性能优化**: 直接调用C函数，性能接近原生调用

## 限制

1. **类型支持**: 目前主要支持基本类型，复杂类型需要扩展
2. **函数签名**: 需要预先知道函数签名
3. **平台依赖**: 依赖`dlopen`/`dlsym`，主要支持Unix-like系统
4. **错误处理**: C库函数的错误处理需要额外考虑

## 扩展性

系统设计为可扩展的，可以轻松添加：

- 新的类型映射
- 更复杂的函数签名支持
- 自定义错误处理策略
- 函数调用缓存机制
- 多线程安全支持

## 集成到现有系统

动态库加载系统完全兼容现有的插件管理器和作用域系统：

```cpp
// 在PluginManager中使用
PluginManager manager;
auto dynamic_plugin = std::make_unique<DynamicLibraryPlugin>("configs/math_library.json");
if (dynamic_plugin->isValid()) {
    dynamic_plugin->getLoader()->registerFunctions(scopeManager);
}
```

这为解释器提供了强大的C库集成能力，使得可以轻松利用现有的C库生态系统。
