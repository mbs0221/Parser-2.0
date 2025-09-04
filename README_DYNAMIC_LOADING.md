# 动态C库加载系统

## 🎯 项目概述

我们成功实现了一个动态C库加载系统，可以直接加载C库中的符号并封装为解释器的内置函数。这个系统避免了代码生成，提供了更灵活和高效的C库集成方案。

## ✨ 核心特性

- **🚀 动态符号加载**: 使用`dlopen`/`dlsym`直接加载C库中的函数符号
- **🛡️ 类型安全转换**: 自动处理C类型与解释器类型之间的转换
- **📋 配置驱动**: 通过JSON配置文件定义要加载的函数
- **🔌 插件集成**: 完全兼容现有的插件系统
- **📝 错误处理**: 完善的错误处理和日志记录

## 🏗️ 系统架构

```
DynamicLibraryPlugin
├── DynamicLibraryLoader
│   ├── Symbol Loader (dlopen/dlsym)
│   └── Function Wrapper (Type Conversion)
└── ScopeManager (Function Registration)
```

## 📁 文件结构

```
include/interpreter/plugins/
├── dynamic_library_loader.h      # 动态库加载器头文件
└── dynamic_library_plugin.h      # 动态库插件头文件

src/interpreter/plugins/
├── dynamic_library_loader.cpp    # 动态库加载器实现
└── dynamic_library_plugin.cpp    # 动态库插件实现

configs/
├── math_library.json            # 数学库配置文件
└── string_library.json          # 字符串库配置文件

examples/
└── dynamic_loading_example.cpp  # 使用示例

tests/
└── dynamic_library_test.cpp     # 测试程序

docs/
└── dynamic_library_loading.md   # 详细文档
```

## 🚀 快速开始

### 1. 基本使用

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
```

### 3. 测试函数调用

```cpp
// 测试sin函数
auto scope = std::make_unique<Scope>();
scope->setArgument("x", new Double(1.0));

auto sin_func = scopeManager.getFunction("sin");
auto result = sin_func->call(scope.get());
std::cout << "sin(1.0) = " << dynamic_cast<Double*>(result)->getValue() << std::endl;
```

## 📋 配置文件格式

```json
{
  "library_name": "math",
  "library_path": "/usr/lib/x86_64-linux-gnu/libm.so",
  "description": "标准C数学库的动态加载插件",
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
  ]
}
```

## 🔄 类型映射

| C类型 | 解释器类型 | 说明 |
|-------|------------|------|
| `double` | `Double` | 双精度浮点数 |
| `int` | `Integer` | 整数 |
| `size_t` | `Integer` | 大小类型 |
| `char*` | `String` | 字符串指针 |
| `const char*` | `String` | 常量字符串指针 |
| `void` | `null` | 空类型 |

## 🎯 使用场景

### 数学库集成
```cpp
// 加载标准C数学库
auto math_plugin = std::make_unique<DynamicLibraryPlugin>("configs/math_library.json");
// 支持: sin, cos, tan, sqrt, log, floor, ceil, abs 等函数
```

### 字符串库集成
```cpp
// 加载标准C字符串库
auto string_plugin = std::make_unique<DynamicLibraryPlugin>("configs/string_library.json");
// 支持: strlen, strcmp, strncmp 等函数
```

### 自定义库集成
```cpp
// 加载自定义C库
auto custom_plugin = std::make_unique<DynamicLibraryPlugin>("/path/to/libcustom.so", "custom");
```

## 🔧 编译和测试

```bash
# 编译interpreter（包含动态加载系统）
cd build
make interpreter -j4

# 运行测试（需要先实现测试程序）
./tests/dynamic_library_test
```

## 🌟 优势

1. **无需代码生成**: 直接加载C库符号，避免生成中间代码
2. **运行时灵活性**: 可以在运行时动态加载不同的库
3. **类型安全**: 自动处理类型转换，减少错误
4. **配置驱动**: 通过配置文件轻松管理要加载的函数
5. **插件兼容**: 完全兼容现有的插件系统
6. **性能优化**: 直接调用C函数，性能接近原生调用

## 🔮 扩展性

系统设计为可扩展的，可以轻松添加：

- ✅ 新的类型映射
- ✅ 更复杂的函数签名支持
- ✅ 自定义错误处理策略
- ✅ 函数调用缓存机制
- ✅ 多线程安全支持

## 📚 相关文档

- [详细技术文档](docs/dynamic_library_loading.md)
- [配置文件示例](configs/)
- [使用示例](examples/)
- [测试程序](tests/)

## 🎉 总结

动态C库加载系统为解释器提供了强大的C库集成能力，使得可以轻松利用现有的C库生态系统。这个系统完全兼容现有的插件架构，为解释器的功能扩展提供了新的可能性。

通过这个系统，我们可以：
- 快速集成标准C库（数学、字符串、IO等）
- 利用现有的C库生态系统
- 避免重复实现常用功能
- 保持高性能的函数调用
- 提供灵活的配置管理

这为解释器的发展奠定了坚实的基础！🚀
