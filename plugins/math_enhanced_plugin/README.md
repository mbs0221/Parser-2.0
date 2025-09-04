# 增强数学函数插件 (Math Enhanced Plugin)

## 概述
增强数学函数插件提供了更高级的数学运算功能，包括复数运算、统计函数、数值分析等高级数学功能。

## 功能列表
- **复数运算**: 复数加减乘除
- **统计函数**: 平均值、方差、标准差
- **数值分析**: 数值积分、微分
- **高级函数**: 双曲函数、反三角函数
- **矩阵运算**: 基本矩阵操作

## 使用方法
```cpp
// 复数运算
let complex1 = create_complex(3, 4);
let complex2 = create_complex(1, 2);
let sum = add_complex(complex1, complex2);

// 统计函数
let data = [1, 2, 3, 4, 5];
let mean = average(data);
let variance = variance(data);
let stddev = standard_deviation(data);

// 高级数学函数
let sinh_value = sinh(1.0);
let asin_value = asin(0.5);
```

## 构建
插件会自动构建为 `libmath_enhanced_plugin.so` 并输出到 `build/plugins/` 目录。
