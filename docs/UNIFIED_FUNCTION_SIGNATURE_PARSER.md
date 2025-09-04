# 统一函数签名解析器实现

## 概述

本次重构统一了 `parseCFormatParams` 函数的使用，解决了内置函数注册和插件函数注册中的重复实现问题，并确保了 lexer、parser 和 interpreter 都使用同一个日志对象。

## 主要改进

### 1. 创建统一的函数签名解析器

**新增文件：**
- `include/common/function_signature_parser.h` - 统一的函数签名解析器头文件
- `src/common/function_signature_parser.cpp` - 统一的函数签名解析器实现

**核心功能：**
- `FunctionSignatureParser::parseCFormatParams()` - 解析C风格函数原型
- `FunctionSignatureParseResult` - 解析结果结构体
- 支持函数名、参数列表、默认值、可变参数等完整解析

### 2. 更新 BuiltinFunction 类

**修改文件：**
- `src/interpreter/values/function.cpp` - 更新 `BuiltinFunction::parseCFormatParams()` 方法
- `include/interpreter/values/value.h` - 更新方法声明注释

**改进内容：**
- 移除重复的解析逻辑
- 使用统一的 `FunctionSignatureParser` 进行解析
- 保持向后兼容性

### 3. 更新插件系统

**修改文件：**
- `include/interpreter/plugins/builtin_plugin.h` - 添加头文件包含和更新注释
- `src/common/CMakeLists.txt` - 添加新的源文件

**改进内容：**
- `REGISTER_BUILTIN_FUNCTION` 宏现在使用统一的函数签名解析器
- 所有插件函数注册都通过统一的解析器处理

### 4. 统一日志系统

**修改文件：**
- `src/lexer/lexer.cpp` - 替换 `printf` 和 `cout` 为统一的日志宏
- `src/parser/parser.cpp` - 替换 `printf` 和 `cout` 为统一的日志宏
- `src/lexer/CMakeLists.txt` - 添加 common 库链接
- `src/parser/CMakeLists.txt` - 添加 common 库链接

**改进内容：**
- lexer 现在使用 `LOG_DEBUG`、`LOG_ERROR` 等统一日志宏
- parser 现在使用 `LOG_DEBUG`、`LOG_ERROR` 等统一日志宏
- interpreter 已经在使用统一的日志系统
- 所有组件都使用同一个 `Logger` 单例对象

## 技术细节

### 函数签名解析器设计

```cpp
struct FunctionSignatureParseResult {
    std::string functionName;                    // 函数名
    std::vector<Parameter> parameters;           // 参数列表
    std::vector<std::string> parameterNames;     // 参数名称列表（兼容性）
    std::map<std::string, std::string> paramDefaults;  // 参数默认值映射
    bool varArgsSupport;                         // 是否支持可变参数
    std::string varArgsName;                     // 可变参数名称
};
```

### 支持的函数原型格式

- `functionName()` - 无参数函数
- `functionName(param1)` - 单参数函数
- `functionName(param1, param2)` - 多参数函数
- `functionName(...)` - 纯可变参数函数
- `functionName(param1, ...)` - 混合参数函数
- `functionName(param1=default)` - 带默认值参数

### 日志系统统一

所有组件现在都使用相同的日志宏：
- `LOG_DEBUG(msg)` - 调试信息
- `LOG_INFO(msg)` - 一般信息
- `LOG_WARN(msg)` - 警告信息
- `LOG_ERROR(msg)` - 错误信息
- `LOG_FATAL(msg)` - 致命错误

## 构建配置更新

### CMakeLists.txt 更新

1. **common 库** - 添加 `function_signature_parser.cpp`
2. **lexer 库** - 链接 common 库和 log4cpp
3. **parser 库** - 链接 common 库和 log4cpp
4. **interpreter 库** - 已经正确配置

## 向后兼容性

- 所有现有的 `BuiltinFunction` 构造函数保持不变
- 插件注册宏 `REGISTER_BUILTIN_FUNCTION` 接口不变
- 日志输出格式保持一致，只是输出方式统一

## 测试建议

1. **功能测试** - 验证所有内置函数和插件函数正常注册
2. **日志测试** - 验证 lexer、parser、interpreter 的日志输出
3. **兼容性测试** - 验证现有代码无需修改即可工作

## 总结

通过这次重构，我们实现了：

1. ✅ **消除重复代码** - 统一的函数签名解析器
2. ✅ **提高代码质量** - 单一职责原则，易于维护
3. ✅ **统一日志系统** - 所有组件使用同一个日志对象
4. ✅ **保持兼容性** - 现有代码无需修改
5. ✅ **改善可维护性** - 集中的解析逻辑，便于扩展

这个统一的设计为未来的功能扩展和代码维护提供了良好的基础。
