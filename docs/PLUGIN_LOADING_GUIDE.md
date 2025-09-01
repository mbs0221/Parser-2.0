# 插件自动加载功能指南

## 🎯 概述

解释器现在支持自动扫描和加载插件目录中的所有动态库，使用logger输出详细信息，并支持自定义插件目录。

## 🔧 功能特性

### 1. 自动扫描插件目录
- 使用`find`命令自动扫描指定目录下的所有`.so`文件
- 支持任意深度的子目录扫描
- 自动过滤非`.so`文件

### 2. Logger输出
- 使用`LOG_INFO`输出重要信息
- 使用`LOG_DEBUG`输出详细调试信息
- 使用`LOG_WARN`输出警告信息
- 使用`LOG_ERROR`输出错误信息

### 3. 插件信息显示
- 插件名称和版本
- 插件描述
- 插件提供的函数列表
- 加载统计信息

### 4. 自定义插件目录
- 支持将插件目录作为参数传递
- 提供重载版本使用默认目录
- 错误处理和容错机制

## 📝 API 使用

### 基本用法
```cpp
// 使用默认插件目录 (./plugins/)
interpreter.loadDefaultPlugins();

// 使用自定义插件目录
interpreter.loadDefaultPlugins("/path/to/plugins/");
```

### 完整示例
```cpp
#include "interpreter/interpreter.h"

int main() {
    Interpreter interpreter;
    
    // 加载默认插件目录
    interpreter.loadDefaultPlugins();
    
    // 加载自定义插件目录
    interpreter.loadDefaultPlugins("/usr/local/lib/my_plugins/");
    
    // 获取已加载的插件列表
    vector<string> loadedPlugins = interpreter.getLoadedPlugins();
    
    return 0;
}
```

## 🔍 输出示例

```
[INFO] 开始扫描插件目录: ./plugins/
[INFO] 找到 5 个插件文件
[DEBUG] 尝试加载插件: ./plugins/libcore_plugin.so
[INFO] 成功加载插件: core v1.0.0
[DEBUG] 插件描述: 核心基础函数插件，包含print、count、cin、exit等基础功能
[DEBUG] 插件函数: print, count, cin, exit
[INFO] 插件加载完成，共加载 5 个插件
```

## 🛠️ 实现细节

### 1. 目录扫描
```cpp
string command = "find " + pluginDir + " -name \"*.so\" 2>/dev/null";
FILE* pipe = popen(command.c_str(), "r");
```

### 2. 插件名称提取
```cpp
string extractPluginName(const string& filePath) {
    // 移除lib前缀和.so后缀
    // 例如: libmath_plugin.so -> math_plugin
}
```

### 3. 错误处理
- 目录不存在时的处理
- 插件加载失败时的处理
- 文件读取错误的处理

## 🎯 优势

1. **自动化**: 无需手动指定插件名称
2. **灵活性**: 支持任意插件目录
3. **可观测性**: 详细的logger输出
4. **容错性**: 优雅的错误处理
5. **扩展性**: 易于添加新插件

## 📊 性能考虑

- 使用`find`命令进行文件扫描，性能良好
- 只扫描`.so`文件，减少不必要的文件处理
- 批量加载插件，减少重复操作

## 🔧 配置选项

### 环境变量支持（未来扩展）
```bash
export PLUGIN_DIR="/path/to/plugins"
export PLUGIN_PATTERN="*.so"
```

### 配置文件支持（未来扩展）
```json
{
    "plugin_directories": [
        "./plugins/",
        "/usr/local/lib/plugins/",
        "/home/user/custom_plugins/"
    ],
    "auto_load": true,
    "log_level": "INFO"
}
```

## 🚀 未来改进

1. **递归扫描**: 支持子目录递归扫描
2. **插件依赖**: 支持插件间的依赖关系
3. **热重载**: 支持运行时重新加载插件
4. **插件配置**: 支持插件级别的配置
5. **性能优化**: 并行加载多个插件

## 结论

新的插件自动加载功能大大简化了插件的管理，提供了更好的用户体验和开发体验。通过logger输出，开发者可以清楚地了解插件的加载状态和详细信息。
