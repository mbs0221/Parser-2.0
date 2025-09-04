#!/bin/bash

# 测试三个独立程序的功能
# 使用方法: ./test_separate_programs.sh [test_file]

TEST_FILE=${1:-"programs/test_class_definition_only.txt"}
TIMEOUT=10

echo "=========================================="
echo "测试三个独立程序的功能"
echo "测试文件: $TEST_FILE"
echo "超时时间: ${TIMEOUT}秒"
echo "=========================================="

# 测试 lexer_main
echo ""
echo "1. 测试 lexer_main (词法分析器)"
echo "----------------------------------------"
if timeout ${TIMEOUT}s ./build/bin/lexer_main "$TEST_FILE" > /dev/null 2>&1; then
    echo "✓ lexer_main 运行成功"
    echo "  词法分析结果:"
    timeout ${TIMEOUT}s ./build/bin/lexer_main "$TEST_FILE" | tail -5
else
    echo "✗ lexer_main 运行失败或超时"
fi

# 测试 parser_main
echo ""
echo "2. 测试 parser_main (语法分析器)"
echo "----------------------------------------"
if timeout ${TIMEOUT}s ./build/bin/parser_main "$TEST_FILE" > /dev/null 2>&1; then
    echo "✓ parser_main 运行成功"
    echo "  语法分析结果:"
    timeout ${TIMEOUT}s ./build/bin/parser_main "$TEST_FILE" | grep -E "(语句总数|语法分析成功)"
else
    echo "✗ parser_main 运行失败或超时"
fi

# 测试 parser_main 带 AST 输出
echo ""
echo "3. 测试 parser_main -a (带AST输出)"
echo "----------------------------------------"
if timeout ${TIMEOUT}s ./build/bin/parser_main -a "$TEST_FILE" > /dev/null 2>&1; then
    echo "✓ parser_main -a 运行成功"
    echo "  AST结构:"
    timeout ${TIMEOUT}s ./build/bin/parser_main -a "$TEST_FILE" | grep -A 10 "AST结构"
else
    echo "✗ parser_main -a 运行失败或超时"
fi

# 测试 interpreter_main
echo ""
echo "4. 测试 interpreter_main (解释器)"
echo "----------------------------------------"
if timeout ${TIMEOUT}s ./build/bin/interpreter_main "$TEST_FILE" > /dev/null 2>&1; then
    echo "✓ interpreter_main 运行成功"
    echo "  执行结果:"
    timeout ${TIMEOUT}s ./build/bin/interpreter_main "$TEST_FILE" | tail -3
else
    echo "✗ interpreter_main 运行失败或超时"
fi

echo ""
echo "=========================================="
echo "测试完成"
echo "=========================================="
