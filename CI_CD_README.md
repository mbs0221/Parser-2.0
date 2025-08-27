# 自动化CI/CD系统使用说明

本项目提供了三个自动化CI/CD脚本，用于在代码修改后自动运行测试并提交。

## 脚本说明

### 1. `quick_commit.sh` - 快速提交脚本（推荐）
- **用途**: 手动触发，运行测试并自动提交
- **特点**: 简单易用，适合手动控制
- **使用方式**: `./quick_commit.sh`

### 2. `auto_commit.sh` - 自动提交脚本
- **用途**: 完整的构建和测试流程
- **特点**: 包含清理构建，更彻底
- **使用方式**: `./auto_commit.sh`

### 3. `watch_and_commit.sh` - 文件监控脚本
- **用途**: 监控文件变化，自动触发提交
- **特点**: 完全自动化，需要安装inotify-tools
- **使用方式**: `./watch_and_commit.sh`

## 安装依赖

### 文件监控脚本依赖
```bash
# Ubuntu/Debian
sudo apt-get install inotify-tools

# CentOS/RHEL
sudo yum install inotify-tools
```

## 使用方法

### 方法1: 手动触发（推荐）
```bash
# 在项目根目录执行
./quick_commit.sh
```

### 方法2: 文件监控（完全自动化）
```bash
# 在项目根目录执行
./watch_and_commit.sh
```

### 方法3: 完整构建
```bash
# 在项目根目录执行
./auto_commit.sh
```

## 工作流程

1. **检查更改**: 检测是否有未提交的代码更改
2. **构建项目**: 在build目录中执行`make parser_main`
3. **运行测试**: 执行`./bin/parser_main ../test_method_simple.txt`
4. **自动提交**: 如果测试通过，自动git commit并生成commit message

## Commit Message格式

自动生成的commit message包含以下信息：
- 构建状态
- 测试状态
- 测试文件
- 自动提交时间
- 触发方式

示例：
```
Auto commit: 代码修改通过测试 - 2024-01-15 14:30:25

- 构建状态: 成功
- 测试状态: 通过
- 测试文件: test_method_simple.txt
- 自动提交时间: 2024-01-15 14:30:25
- 触发方式: 手动触发
```

## 注意事项

1. **测试文件**: 当前使用`test_method_simple.txt`作为测试文件
2. **构建目录**: 脚本假设build目录存在且包含Makefile
3. **Git配置**: 确保git已正确配置用户信息
4. **权限**: 确保脚本有执行权限

## 故障排除

### 构建失败
- 检查build目录是否存在
- 检查Makefile是否正确
- 检查依赖是否安装完整

### 测试失败
- 检查`test_method_simple.txt`文件是否存在
- 检查测试文件内容是否正确
- 查看测试输出获取错误信息

### 提交失败
- 检查git配置是否正确
- 检查是否有足够的权限
- 检查网络连接（如果使用远程仓库）

## 自定义配置

### 修改测试文件
编辑脚本中的测试文件路径：
```bash
./bin/parser_main ../your_test_file.txt
```

### 修改提交间隔
在`watch_and_commit.sh`中修改：
```bash
MIN_COMMIT_INTERVAL=30  # 最小提交间隔（秒）
```

### 修改监控文件类型
在`watch_and_commit.sh`中修改：
```bash
--include="\.(cpp|h|txt|md)$"  # 添加更多文件类型
```
