#!/bin/bash

# 批量测试所有程序脚本
echo "=== Parser-2.0 程序批量测试 ==="
echo "开始时间: $(date)"
echo

# 测试结果统计
total_tests=0
passed_tests=0
failed_tests=0
segfault_tests=0

# 测试函数
test_program() {
    local program=$1
    local program_name=$(basename "$program" .txt)
    
    echo "测试: $program_name"
    echo "----------------------------------------"
    
    total_tests=$((total_tests + 1))
    
    # 运行程序并捕获退出码
    if timeout 10s ./build/bin/parser_main "$program" > /tmp/test_output_$program_name.log 2>&1; then
        exit_code=$?
        if [ $exit_code -eq 0 ]; then
            echo "✅ 通过"
            passed_tests=$((passed_tests + 1))
        else
            echo "❌ 失败 (退出码: $exit_code)"
            failed_tests=$((failed_tests + 1))
        fi
    else
        exit_code=$?
        if [ $exit_code -eq 124 ]; then
            echo "⏰ 超时"
            failed_tests=$((failed_tests + 1))
        elif [ $exit_code -eq 139 ]; then
            echo "💥 段错误"
            segfault_tests=$((segfault_tests + 1))
        else
            echo "❌ 失败 (退出码: $exit_code)"
            failed_tests=$((failed_tests + 1))
        fi
    fi
    
    echo
}

# 测试所有程序
echo "开始测试所有程序..."
echo

# 测试基本程序
test_program "programs/test_basic.txt"
test_program "programs/test_arithmetic.txt"
test_program "programs/test_print.txt"
test_program "programs/test_math.txt"
test_program "programs/test_simple_function.txt"
test_program "programs/test_simple_var.txt"
test_program "programs/test_math_functions.txt"
test_program "programs/test_simple_expr.txt"
test_program "programs/test_simple_plus.txt"
test_program "programs/test_simple_assignment.txt"
test_program "programs/test_simple_let.txt"
test_program "programs/test_simple_bool.txt"
test_program "programs/test_simple_condition.txt"

# 测试可能有问题的程序
echo "测试可能有问题的程序..."
echo

test_program "programs/test_control_flow_fixed.txt"
test_program "programs/test_increment_simple.txt"
test_program "programs/test_basic_function.txt"
test_program "programs/test_basic_types_methods.txt"
test_program "programs/test_builtin_functions.txt"
test_program "programs/test_type_conversion.txt"
test_program "programs/test_functions.txt"
test_program "programs/test_collections.txt"

# 输出测试结果
echo "=== 测试结果汇总 ==="
echo "总测试数: $total_tests"
echo "通过: $passed_tests"
echo "失败: $failed_tests"
echo "段错误: $segfault_tests"
echo "成功率: $(( passed_tests * 100 / total_tests ))%"
echo
echo "结束时间: $(date)"
