# IO插件 (IO Plugin)

## 概述
IO插件提供了文件输入输出、网络通信、控制台交互等输入输出功能。

## 功能列表
- **file_read**: 读取文件内容
- **file_write**: 写入文件内容
- **file_append**: 追加文件内容
- **file_exists**: 检查文件是否存在
- **file_delete**: 删除文件
- **file_copy**: 复制文件
- **file_move**: 移动文件
- **dir_create**: 创建目录
- **dir_list**: 列出目录内容
- **dir_delete**: 删除目录
- **network_send**: 网络发送数据
- **network_receive**: 网络接收数据

## 使用方法
```cpp
// 文件操作
let content = file_read("input.txt");
file_write("output.txt", "Hello World");
file_append("log.txt", "New log entry");

// 文件系统操作
let exists = file_exists("test.txt");
file_copy("source.txt", "dest.txt");
file_delete("temp.txt");

// 目录操作
dir_create("new_folder");
let files = dir_list(".");
dir_delete("old_folder");

// 网络操作
network_send("http://example.com", "GET /");
let response = network_receive();
```

## 构建
插件会自动构建为 `libio_plugin.so` 并输出到 `build/plugins/` 目录。
