# Parser-2.0 插件系统

## 概述
Parser-2.0采用模块化的插件架构，每个插件都有独立的文件夹和构建配置，便于维护和扩展。

## 插件结构
```
plugins/
├── core_plugin/           # 核心基础函数插件
├── math_plugin/           # 基础数学函数插件
├── math_enhanced_plugin/  # 增强数学函数插件
├── string_plugin/         # 字符串处理插件
├── array_plugin/          # 数组操作插件
├── type_plugin/           # 类型转换插件
├── io_plugin/             # IO插件
├── extra_plugin/          # 额外功能插件
└── CMakeLists.txt         # 主构建配置
```

## 插件列表

### 1. 核心基础函数插件 (core_plugin)
- **功能**: 提供基础的内置函数
- **主要函数**: print, println, count, cin, abs, max, min, pow, length, substring, to_upper, to_lower, push, pop, sort, to_string, to_int, to_double, cast, random, exit
- **文件**: `basic_builtin_plugin.cpp`

### 2. 基础数学函数插件 (math_plugin)
- **功能**: 提供基础数学运算函数
- **主要函数**: sin, cos, tan, sqrt, log, floor, ceil
- **文件**: `math_plugin.cpp`

### 3. 增强数学函数插件 (math_enhanced_plugin)
- **功能**: 提供高级数学运算函数
- **主要函数**: 复数运算、统计函数、数值分析、高级函数、矩阵运算
- **文件**: `math_plugin_enhanced.cpp`

### 4. 字符串处理插件 (string_plugin)
- **功能**: 提供字符串处理函数
- **主要函数**: trim, replace, split, join, starts_with, ends_with, contains, format
- **文件**: `string_plugin.cpp`

### 5. 数组操作插件 (array_plugin)
- **功能**: 提供数组操作函数
- **主要函数**: array_create, array_push, array_pop, array_insert, array_remove, array_find, array_sort, array_reverse, array_slice, array_join
- **文件**: `array_plugin.cpp`

### 6. 类型转换插件 (type_plugin)
- **功能**: 提供类型转换函数
- **主要函数**: type_cast, to_string, to_int, to_double, to_bool, to_array, to_dict, is_type, get_type
- **文件**: `type_plugin.cpp`

### 7. IO插件 (io_plugin)
- **功能**: 提供输入输出函数
- **主要函数**: file_read, file_write, file_append, file_exists, file_delete, file_copy, file_move, dir_create, dir_list, dir_delete, network_send, network_receive
- **文件**: `io_plugin.cpp`

### 8. 额外功能插件 (extra_plugin)
- **功能**: 提供扩展功能
- **主要函数**: date_now, date_format, date_parse, encrypt, decrypt, compress, decompress, system_info, process_info, memory_info, disk_info
- **文件**: `extra_plugin.cpp`

## 构建系统

### 构建所有插件
```bash
cd build
make
```

### 构建特定插件
```bash
cd build
make core_plugin
make string_plugin
make math_plugin
```

### 插件输出
所有插件都会构建为动态库文件（.so）并输出到 `build/plugins/` 目录：
- `libcore_plugin.so`
- `libmath_plugin.so`
- `libmath_enhanced_plugin.so`
- `libstring_plugin.so`
- `libarray_plugin.so`
- `libtype_plugin.so`
- `libio_plugin.so`
- `libextra_plugin.so`

## 插件开发

### 添加新插件
1. 在 `plugins/` 目录下创建新的插件文件夹
2. 在插件文件夹中创建 `CMakeLists.txt`
3. 在主 `CMakeLists.txt` 中添加 `add_subdirectory(新插件文件夹)`
4. 在插件目标列表中添加新插件名称

### 插件接口
所有插件都需要实现 `BuiltinPlugin` 接口：
- `getPluginInfo()`: 返回插件信息
- `registerFunctions()`: 注册插件函数

### 插件导出
使用 `EXPORT_PLUGIN(插件类名)` 宏导出插件。

## 使用插件
插件会在解释器启动时自动加载，用户可以直接使用插件提供的函数，无需额外配置。

## 注意事项
- 每个插件都有独立的构建配置
- 插件之间相互独立，可以单独构建和测试
- 所有插件都使用C++17标准
- 插件支持位置无关代码（PIC）
