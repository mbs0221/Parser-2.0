#!/bin/bash

# 测试运行脚本
echo "=== Parser项目测试套件 ==="

# 检查build目录
if [ ! -d "build" ]; then
    echo "创建build目录..."
    mkdir -p build
fi

# 进入build目录
cd build

# 创建必要的目录
mkdir -p bin lib

# 配置CMake
echo "配置CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Debug

# 编译
echo "编译项目..."
make -j$(nproc)

# 运行测试
echo "运行测试..."

# 运行lexer测试
if [ -f "bin/test_lexer_comprehensive" ]; then
    echo "运行Lexer综合测试..."
    ./bin/test_lexer_comprehensive
    echo ""
else
    echo "警告: Lexer综合测试未找到"
fi

# 运行parser测试
if [ -f "bin/test_parser_comprehensive" ]; then
    echo "运行Parser综合测试..."
    ./bin/test_parser_comprehensive
    echo ""
else
    echo "警告: Parser综合测试未找到"
fi

# 运行interpreter测试
if [ -f "bin/test_interpreter_comprehensive" ]; then
    echo "运行Interpreter综合测试..."
    ./bin/test_interpreter_comprehensive
    echo ""
else
    echo "警告: Interpreter综合测试未找到"
fi

# 运行optimizer测试
if [ -f "bin/test_optimizer_comprehensive" ]; then
    echo "运行Optimizer综合测试..."
    ./bin/test_optimizer_comprehensive
    echo ""
else
    echo "警告: Optimizer综合测试未找到"
fi

echo "=== 测试完成 ==="
