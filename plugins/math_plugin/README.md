# 基础数学函数插件 (Math Plugin)

## 概述
基础数学函数插件提供了基本的数学运算功能，包括三角函数、对数函数、指数函数等。

## 功能列表
- **sin**: 正弦函数
- **cos**: 余弦函数
- **tan**: 正切函数
- **sqrt**: 平方根函数
- **log**: 自然对数函数
- **floor**: 向下取整函数
- **ceil**: 向上取整函数

## 使用方法
```cpp
// 三角函数
let angle = 3.14159 / 4;
let sinValue = sin(angle);
let cosValue = cos(angle);
let tanValue = tan(angle);

// 其他数学函数
let sqrtValue = sqrt(16);
let logValue = log(2.71828);
let floorValue = floor(3.7);
let ceilValue = ceil(3.2);
```

## 构建
插件会自动构建为 `libmath_plugin.so` 并输出到 `build/plugins/` 目录。
