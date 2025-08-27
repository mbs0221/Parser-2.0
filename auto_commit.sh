#!/bin/bash

# 自动化CI/CD脚本
# 当代码修改后能成功运行测试时自动git commit

set -e  # 遇到错误时退出

echo "=== 自动化CI/CD开始 ==="

# 检查是否有未提交的更改
if git diff --quiet && git diff --cached --quiet; then
    echo "没有检测到代码更改，跳过自动提交"
    exit 0
fi

echo "检测到代码更改，开始构建和测试..."

# 切换到构建目录
cd build

# 清理并重新构建
echo "正在构建项目..."
make clean
make parser_main

# 检查构建是否成功
if [ $? -ne 0 ]; then
    echo "构建失败，跳过自动提交"
    exit 1
fi

echo "构建成功，开始运行测试..."

# 运行测试
echo "运行测试: ./bin/parser_main ../test_method_simple.txt"
./bin/parser_main ../test_method_simple.txt

# 检查测试是否成功
if [ $? -ne 0 ]; then
    echo "测试失败，跳过自动提交"
    exit 1
fi

echo "测试通过！准备自动提交..."

# 回到项目根目录
cd ..

# 生成commit message
TIMESTAMP=$(date '+%Y-%m-%d %H:%M:%S')
COMMIT_MSG="Auto commit: 代码修改通过测试 - $TIMESTAMP

- 构建状态: 成功
- 测试状态: 通过
- 测试文件: test_method_simple.txt
- 自动提交时间: $TIMESTAMP"

# 添加所有更改到暂存区
git add .

# 提交更改
echo "提交更改..."
git commit -m "$COMMIT_MSG"

echo "=== 自动提交完成 ==="
echo "Commit Hash: $(git rev-parse HEAD)"
echo "Commit Message: $COMMIT_MSG"
