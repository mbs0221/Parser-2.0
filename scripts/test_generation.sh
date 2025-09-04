#!/bin/bash

# 自动化封装工具测试脚本

echo "=== C库自动化封装工具测试 ==="

# 检查工具是否存在
if [ ! -f "./auto_wrapper" ]; then
    echo "错误: auto_wrapper 工具不存在，请先编译"
    exit 1
fi

# 创建输出目录
mkdir -p test_output

echo "1. 生成数学库插件..."
./auto_wrapper --config examples/math_config.json --output test_output/math_auto_plugin --verbose

if [ $? -eq 0 ]; then
    echo "✓ 数学库插件生成成功"
    echo "生成的文件:"
    ls -la test_output/math_auto_plugin/
else
    echo "✗ 数学库插件生成失败"
fi

echo ""
echo "2. 生成字符串库插件..."
./auto_wrapper --config examples/string_config.json --output test_output/string_auto_plugin --verbose

if [ $? -eq 0 ]; then
    echo "✓ 字符串库插件生成成功"
    echo "生成的文件:"
    ls -la test_output/string_auto_plugin/
else
    echo "✗ 字符串库插件生成失败"
fi

echo ""
echo "3. 显示生成的代码示例..."
if [ -f "test_output/math_auto_plugin/MathAutoPlugin.h" ]; then
    echo "=== 生成的头文件示例 ==="
    head -20 test_output/math_auto_plugin/MathAutoPlugin.h
fi

echo ""
echo "=== 测试完成 ==="
echo "生成的插件位于 test_output/ 目录中"
echo "可以将这些插件集成到主项目的 plugins/ 目录中"
