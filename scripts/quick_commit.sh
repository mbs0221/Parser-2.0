#!/bin/bash

# 快速提交脚本
# 手动触发，运行测试并自动提交

set -e

echo "=== 快速提交系统 ==="

# 检查是否有未提交的更改
if git diff --quiet && git diff --cached --quiet; then
    echo "没有检测到代码更改，跳过提交"
    exit 0
fi

echo "检测到代码更改，开始构建和测试..."

# 切换到构建目录
cd build

# 构建项目
echo "正在构建项目..."
if make parser_main; then
    echo "构建成功，运行测试..."
    
    # 运行测试
    echo "运行测试: ./bin/parser_main ../test_method_simple.txt"
    if ./bin/parser_main ../test_method_simple.txt; then
        echo "测试通过！准备自动提交..."
        
        # 回到项目根目录
        cd ..
        
        # 生成commit message
        TIMESTAMP=$(date '+%Y-%m-%d %H:%M:%S')
        COMMIT_MSG="Auto commit: 代码修改通过测试 - $TIMESTAMP

- 构建状态: 成功
- 测试状态: 通过
- 测试文件: test_method_simple.txt
- 自动提交时间: $TIMESTAMP
- 触发方式: 手动触发"

        # 添加所有更改到暂存区
        git add .
        
        # 提交更改
        echo "提交更改..."
        git commit -m "$COMMIT_MSG"
        
        echo "✅ 自动提交完成！"
        echo "   Commit: $(git rev-parse --short HEAD)"
        echo "   时间: $TIMESTAMP"
    else
        echo "❌ 测试失败，跳过提交"
        cd ..
        exit 1
    fi
else
    echo "❌ 构建失败，跳过提交"
    cd ..
    exit 1
fi
