# C库自动化封装工具

## 概述

这个工具可以自动将C库函数封装为解释器插件，支持自动类型转换、错误处理和函数注册。

## 特性

- 自动解析C函数签名
- 智能类型映射（C类型 ↔ 解释器类型）
- 自动生成插件代码
- 支持多种C库（数学、字符串、IO等）
- 可配置的封装规则

## 使用方法

```bash
# 生成数学库插件
./auto_wrapper --config math_config.json --output math_auto_plugin

# 生成字符串库插件  
./auto_wrapper --config string_config.json --output string_auto_plugin
```

## 配置文件格式

```json
{
  "library_name": "math",
  "header_files": ["math.h", "stdlib.h"],
  "functions": [
    {
      "name": "sin",
      "c_signature": "double sin(double x)",
      "description": "计算正弦值",
      "parameters": [
        {"name": "x", "type": "double", "description": "角度值"}
      ],
      "return_type": "double"
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

## 生成的代码结构

```
math_auto_plugin/
├── CMakeLists.txt
├── math_auto_plugin.cpp
├── math_auto_plugin.h
└── README.md
```
