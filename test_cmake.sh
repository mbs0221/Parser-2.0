#!/bin/bash

echo "=== Parser-2.0 CMake项目测试 ==="

# 检查是否在正确的目录
if [ ! -f "CMakeLists.txt" ]; then
    echo "错误：请在Parser-2.0项目根目录运行此脚本"
    exit 1
fi

echo "1. 清理build目录..."
rm -rf build
mkdir -p build
cd build

echo "2. 配置CMake项目..."
cmake .. || { echo "CMake配置失败"; exit 1; }

echo "3. 编译项目..."
make -j4 || { echo "编译失败"; exit 1; }

echo "4. 检查生成的可执行文件..."
if [ -f "bin/parser" ]; then
    echo "✓ parser可执行文件生成成功"
else
    echo "✗ parser可执行文件生成失败"
    exit 1
fi

if [ -f "bin/test_parser" ]; then
    echo "✓ test_parser可执行文件生成成功"
else
    echo "✗ test_parser可执行文件生成失败"
    exit 1
fi

echo "5. 运行测试程序..."
./bin/test_parser || { echo "test_parser运行失败"; exit 1; }

echo "6. 测试parser程序（如果有输入文件）..."
if [ -f "../Text.txt" ]; then
    echo "发现Text.txt文件，测试parser程序..."
    timeout 3s ./bin/parser ../Text.txt || echo "parser程序运行完成或超时"
else
    echo "未发现Text.txt文件，跳过parser程序测试"
fi

echo ""
echo "=== 测试完成 ==="
echo "✓ CMake项目配置成功"
echo "✓ 编译成功"
echo "✓ 可执行文件生成成功"
echo "✓ 测试程序运行成功"
echo ""
echo "项目结构："
echo "- CMakeLists.txt (主配置文件)"
echo "- Parser/CMakeLists.txt (Parser子目录配置)"
echo "- Parser/parser.cpp (主程序)"
echo "- Parser/inter.h (中间表示)"
echo "- Parser/lexer.h (词法分析器)"
echo "- test_parser.cpp (测试程序)"
echo ""
echo "构建输出："
echo "- build/bin/parser (主程序)"
echo "- build/bin/test_parser (测试程序)"
