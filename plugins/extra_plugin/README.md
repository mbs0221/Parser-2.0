# 额外功能插件 (Extra Plugin)

## 概述
额外功能插件提供了各种扩展功能，包括日期时间处理、加密解密、压缩解压、系统信息等实用功能。

## 功能列表
- **date_now**: 获取当前日期时间
- **date_format**: 格式化日期时间
- **date_parse**: 解析日期时间字符串
- **encrypt**: 数据加密
- **decrypt**: 数据解密
- **compress**: 数据压缩
- **decompress**: 数据解压
- **system_info**: 获取系统信息
- **process_info**: 获取进程信息
- **memory_info**: 获取内存信息
- **disk_info**: 获取磁盘信息

## 使用方法
```cpp
// 日期时间处理
let now = date_now();
let formatted = date_format(now, "YYYY-MM-DD HH:mm:ss");
let parsed = date_parse("2023-12-25 10:30:00");

// 加密解密
let encrypted = encrypt("sensitive data", "password");
let decrypted = decrypt(encrypted, "password");

// 压缩解压
let compressed = compress("large data");
let decompressed = decompress(compressed);

// 系统信息
let sysInfo = system_info();
let procInfo = process_info();
let memInfo = memory_info();
let diskInfo = disk_info();
```

## 构建
插件会自动构建为 `libextra_plugin.so` 并输出到 `build/plugins/` 目录。
