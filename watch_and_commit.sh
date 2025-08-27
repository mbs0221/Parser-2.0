#!/bin/bash

# 文件监控和自动提交脚本
# 监控代码文件变化，自动运行测试并提交

set -e

echo "=== 文件监控和自动提交系统启动 ==="
echo "监控目录: $(pwd)"
echo "按 Ctrl+C 停止监控"

# 检查是否安装了inotify-tools
if ! command -v inotifywait &> /dev/null; then
    echo "错误: 需要安装 inotify-tools"
    echo "Ubuntu/Debian: sudo apt-get install inotify-tools"
    echo "CentOS/RHEL: sudo yum install inotify-tools"
    exit 1
fi

# 监控的文件类型
WATCH_PATTERNS="*.cpp *.h *.txt"

# 上次提交时间，避免频繁提交
LAST_COMMIT_TIME=0
MIN_COMMIT_INTERVAL=30  # 最小提交间隔（秒）

# 自动提交函数
auto_commit() {
    local current_time=$(date +%s)
    local time_diff=$((current_time - LAST_COMMIT_TIME))
    
    if [ $time_diff -lt $MIN_COMMIT_INTERVAL ]; then
        echo "距离上次提交时间太短 (${time_diff}s < ${MIN_COMMIT_INTERVAL}s)，跳过提交"
        return
    fi
    
    echo "检测到文件变化，开始自动构建和测试..."
    
    # 切换到构建目录
    cd build
    
    # 构建项目
    echo "正在构建项目..."
    if make parser_main > /dev/null 2>&1; then
        echo "构建成功，运行测试..."
        
        # 运行测试
        if ./bin/parser_main ../test_method_simple.txt > /dev/null 2>&1; then
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
- 触发原因: 文件变化检测"

            # 添加所有更改到暂存区
            git add .
            
            # 提交更改
            git commit -m "$COMMIT_MSG" > /dev/null 2>&1
            
            LAST_COMMIT_TIME=$current_time
            
            echo "✅ 自动提交完成！"
            echo "   Commit: $(git rev-parse --short HEAD)"
            echo "   时间: $TIMESTAMP"
        else
            echo "❌ 测试失败，跳过提交"
            cd ..
        fi
    else
        echo "❌ 构建失败，跳过提交"
        cd ..
    fi
}

# 监控文件变化
echo "开始监控文件变化..."
inotifywait -m -r -e modify,create,delete,move \
    --include="\.(cpp|h|txt)$" \
    . | while read path action file; do
    echo "检测到文件变化: $path$file ($action)"
    auto_commit
done
