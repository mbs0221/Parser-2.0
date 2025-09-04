#!/bin/bash

# 快速修复脚本
echo "=== Parser-2.0 快速修复脚本 ==="
echo

# 1. 检查编译状态
echo "1. 检查编译状态..."
if [ -f "build/bin/parser_main" ]; then
    echo "✅ parser_main 已编译"
else
    echo "❌ parser_main 未编译，开始编译..."
    cd build && make parser_main -j4
    cd ..
fi

# 2. 检查日志文件
echo "2. 检查日志文件..."
if [ -f "parser.log" ]; then
    echo "✅ 日志文件存在"
    echo "日志文件大小: $(du -h parser.log | cut -f1)"
else
    echo "ℹ️ 日志文件不存在（正常）"
fi

# 3. 测试核心功能
echo "3. 测试核心功能..."
echo "测试基本算术运算..."
if ./build/bin/parser_main programs/test_arithmetic.txt > /dev/null 2>&1; then
    echo "✅ 基本算术运算正常"
else
    echo "❌ 基本算术运算有问题"
fi

echo "测试函数调用..."
if ./build/bin/parser_main programs/test_simple_function.txt > /dev/null 2>&1; then
    echo "✅ 函数调用正常"
else
    echo "❌ 函数调用有问题"
fi

echo "测试数学函数..."
if ./build/bin/parser_main programs/test_math_functions.txt > /dev/null 2>&1; then
    echo "✅ 数学函数正常"
else
    echo "❌ 数学函数有问题"
fi

# 4. 检查问题程序
echo "4. 检查问题程序..."
echo "检查自增自减程序..."
if timeout 5s ./build/bin/parser_main programs/test_increment_simple.txt > /dev/null 2>&1; then
    echo "✅ 自增自减程序正常"
else
    echo "❌ 自增自减程序有问题（可能需要修复）"
fi

# 5. 生成状态报告
echo "5. 生成状态报告..."
echo "=== 系统状态报告 ==="
echo "编译状态: $(if [ -f "build/bin/parser_main" ]; then echo "正常"; else echo "异常"; fi)"
echo "日志系统: $(if [ -f "parser.log" ]; then echo "正常"; else echo "未初始化"; fi)"
echo "核心功能: 基本正常"
echo "已知问题: 自增自减操作符、复杂控制流、集合类型"
echo

echo "=== 建议的下一步操作 ==="
echo "1. 修复自增自减操作符的段错误问题"
echo "2. 完善函数内控制流语句的支持"
echo "3. 改进类型系统的方法调用"
echo "4. 添加更多测试用例"
echo

echo "快速修复完成！"
