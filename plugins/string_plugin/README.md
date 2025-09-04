# 字符串处理插件 (String Plugin)

## 概述
字符串处理插件提供了丰富的字符串操作功能，包括字符串处理、格式化、搜索等高级功能。

## 功能列表
- **trim**: 去除字符串首尾空白字符
- **replace**: 替换字符串中的子串
- **split**: 按分隔符分割字符串
- **join**: 用分隔符连接字符串数组
- **starts_with**: 检查字符串是否以指定前缀开头
- **ends_with**: 检查字符串是否以指定后缀结尾
- **contains**: 检查字符串是否包含指定子串
- **format**: Python风格的字符串格式化函数

## 使用方法

### 基本字符串操作
```cpp
// 去除空白字符
let trimmed = trim("  hello world  ");

// 替换字符串
let replaced = replace("hello world", "world", "universe");

// 分割字符串
let parts = split("apple,banana,orange", ",");

// 连接字符串数组
let joined = join(parts, " | ");
```

### 字符串检查
```cpp
// 检查前缀和后缀
let hasPrefix = starts_with("hello world", "hello");
let hasSuffix = ends_with("hello world", "world");
let hasSubstr = contains("hello world", "world");
```

### Python风格格式化
```cpp
// 基本格式化
let result1 = format("Hello {}!", "World");

// 位置参数
let result2 = format("{1} 和 {0}", "World", "Hello");

// 数字格式化
let result3 = format("精度: {:.2f}", 3.14159);
let result4 = format("整数宽度: {:4d}", 42);

// 字符串宽度
let result5 = format("宽度: {:10s}", "test");
```

## 构建
插件会自动构建为 `libstring_plugin.so` 并输出到 `build/plugins/` 目录。
