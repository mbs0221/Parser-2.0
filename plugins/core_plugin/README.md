# 核心基础函数插件 (Core Plugin)

## 概述
核心基础函数插件提供了Parser-2.0解释器的基础功能，包括输入输出、基本数学运算、类型转换等核心功能。

## 功能列表
- **print/println**: 打印输出函数
- **count**: 计数函数
- **cin**: 输入函数
- **abs**: 绝对值函数
- **max/min**: 最大值/最小值函数
- **pow**: 幂运算函数
- **length**: 长度函数
- **substring**: 子字符串函数
- **to_upper/to_lower**: 大小写转换函数
- **push/pop**: 数组操作函数
- **sort**: 排序函数
- **to_string/to_int/to_double**: 类型转换函数
- **cast**: 类型转换函数
- **random**: 随机数函数
- **exit**: 退出函数

## 使用方法
```cpp
// 基本输出
print("Hello World");
println("Hello World with newline");

// 数学运算
let result = max(10, 20);
let power = pow(2, 3);

// 字符串操作
let upper = to_upper("hello");
let len = length("hello");

// 类型转换
let str = to_string(42);
let num = to_int("123");
```

## 构建
插件会自动构建为 `libcore_plugin.so` 并输出到 `build/plugins/` 目录。
