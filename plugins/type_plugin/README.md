# 类型转换插件 (Type Plugin)

## 概述
类型转换插件提供了各种数据类型之间的转换功能，包括基本类型转换、自定义类型转换等。

## 功能列表
- **type_cast**: 通用类型转换函数
- **to_string**: 转换为字符串
- **to_int**: 转换为整数
- **to_double**: 转换为浮点数
- **to_bool**: 转换为布尔值
- **to_array**: 转换为数组
- **to_dict**: 转换为字典
- **is_type**: 检查类型
- **get_type**: 获取类型信息

## 使用方法
```cpp
// 基本类型转换
let str = to_string(42);
let num = to_int("123");
let dbl = to_double("3.14");
let flag = to_bool("true");

// 通用类型转换
let converted = type_cast(value, "string");
let isString = is_type(value, "string");
let typeInfo = get_type(value);

// 复杂类型转换
let arr = to_array("1,2,3,4,5");
let dict = to_dict("key1:value1,key2:value2");
```

## 构建
插件会自动构建为 `libtype_plugin.so` 并输出到 `build/plugins/` 目录。
