#!/bin/bash

# 自动化分析失败测试的脚本
# 使用valgrind和gdb来分析INTERPRETER_ERROR和TIMEOUT的测试

set -e

# 配置
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
UNIT_TESTS_DIR="$PROJECT_ROOT/unit-tests"
BUILD_DIR="$PROJECT_ROOT/build"
OUTPUT_DIR="$PROJECT_ROOT/test_analysis"
TIMEOUT=10  # 分析超时时间

# 创建输出目录
mkdir -p "$OUTPUT_DIR"

echo "=== 自动化测试错误分析 ==="
echo "项目根目录: $PROJECT_ROOT"
echo "输出目录: $OUTPUT_DIR"
echo ""

# 检查依赖
if ! command -v valgrind &> /dev/null; then
    echo "警告: valgrind 未安装，将跳过内存泄漏分析"
    VALGRIND_AVAILABLE=false
else
    VALGRIND_AVAILABLE=true
fi

if ! command -v gdb &> /dev/null; then
    echo "警告: gdb 未安装，将跳过调试分析"
    GDB_AVAILABLE=false
else
    GDB_AVAILABLE=true
fi

# 获取失败的测试列表
get_failed_tests() {
    # 直接分析一些已知的失败测试
    local failed_tests=(
        "test_advanced_assignment.txt"
        "test_ast_to_type_conversion.txt"
        "test_basic_comprehensive.txt"
        "test_basic_types_methods.txt"
        "test_class_method_call.txt"
        "test_class_simple.txt"
        "test_collections.txt"
        "test_collections_functions.txt"
        "test_complete_comprehensive.txt"
        "test_complex_condition.txt"
    )
    
    # 检查哪些测试文件实际存在
    local existing_tests=()
    for test in "${failed_tests[@]}"; do
        if [ -f "$UNIT_TESTS_DIR/$test" ]; then
            existing_tests+=("$test")
        fi
    done
    
    printf '%s\n' "${existing_tests[@]}"
}

# 使用valgrind分析内存问题
analyze_with_valgrind() {
    local test_file="$1"
    local output_file="$2"
    
    if [ "$VALGRIND_AVAILABLE" = false ]; then
        echo "跳过valgrind分析: 工具未安装"
        return
    fi
    
    echo "使用valgrind分析: $test_file"
    
    timeout $TIMEOUT valgrind \
        --tool=memcheck \
        --leak-check=full \
        --show-leak-kinds=all \
        --track-origins=yes \
        --verbose \
        --log-file="$output_file.valgrind" \
        "$BUILD_DIR/bin/interpreter_main" "$test_file" 2>/dev/null || true
    
    if [ -f "$output_file.valgrind" ]; then
        echo "valgrind报告已保存到: $output_file.valgrind"
    fi
}

# 使用gdb分析段错误
analyze_with_gdb() {
    local test_file="$1"
    local output_file="$2"
    
    if [ "$GDB_AVAILABLE" = false ]; then
        echo "跳过gdb分析: 工具未安装"
        return
    fi
    
    echo "使用gdb分析: $test_file"
    
    # 创建gdb命令脚本
    local gdb_script="$output_file.gdb_script"
    cat > "$gdb_script" << 'EOF'
set confirm off
set pagination off
run
bt
info registers
info locals
quit
EOF
    
    timeout $TIMEOUT gdb \
        --batch \
        --command="$gdb_script" \
        --args "$BUILD_DIR/bin/interpreter_main" "$test_file" \
        > "$output_file.gdb" 2>&1 || true
    
    if [ -f "$output_file.gdb" ]; then
        echo "gdb报告已保存到: $output_file.gdb"
    fi
    
    rm -f "$gdb_script"
}

# 分析单个测试
analyze_test() {
    local test_file="$1"
    local test_name=$(basename "$test_file" .txt)
    local output_file="$OUTPUT_DIR/analysis_$test_name"
    
    echo ""
    echo "============================================================"
    echo "分析测试: $test_file"
    echo "============================================================"
    
    # 检查测试文件是否存在
    if [ ! -f "$test_file" ]; then
        echo "错误: 测试文件不存在: $test_file"
        return 1
    fi
    
    # 显示测试内容
    echo "测试内容:"
    echo "----------------------------------------"
    head -10 "$test_file"
    echo "----------------------------------------"
    
    # 先尝试正常运行
    echo "尝试正常运行测试..."
    timeout 5 "$BUILD_DIR/bin/interpreter_main" "$test_file" > "$output_file.normal" 2>&1 || true
    
    if [ -f "$output_file.normal" ]; then
        echo "正常输出已保存到: $output_file.normal"
        echo "正常输出内容:"
        cat "$output_file.normal"
    fi
    
    # 使用valgrind分析
    analyze_with_valgrind "$test_file" "$output_file"
    
    # 使用gdb分析
    analyze_with_gdb "$test_file" "$output_file"
    
    echo "分析完成: $test_name"
}

# 生成分析报告
generate_report() {
    local report_file="$OUTPUT_DIR/analysis_report.md"
    
    echo "生成分析报告: $report_file"
    
    cat > "$report_file" << EOF
# 测试错误分析报告

生成时间: $(date)
项目目录: $PROJECT_ROOT

## 分析摘要

EOF
    
    # 统计分析结果
    local total_analyzed=$(find "$OUTPUT_DIR" -name "analysis_*.normal" | wc -l)
    local valgrind_reports=$(find "$OUTPUT_DIR" -name "*.valgrind" | wc -l)
    local gdb_reports=$(find "$OUTPUT_DIR" -name "*.gdb" | wc -l)
    
    cat >> "$report_file" << EOF
- 总分析测试数: $total_analyzed
- valgrind报告数: $valgrind_reports
- gdb报告数: $gdb_reports

## 详细分析结果

EOF
    
    # 为每个测试生成报告
    for normal_file in "$OUTPUT_DIR"/analysis_*.normal; do
        if [ -f "$normal_file" ]; then
            local test_name=$(basename "$normal_file" .normal | sed 's/analysis_//')
            local base_name="$OUTPUT_DIR/analysis_$test_name"
            
            echo "### $test_name" >> "$report_file"
            echo "" >> "$report_file"
            
            # 添加正常输出
            echo "**正常输出:**" >> "$report_file"
            echo '```' >> "$report_file"
            cat "$normal_file" >> "$report_file"
            echo '```' >> "$report_file"
            echo "" >> "$report_file"
            
            # 添加valgrind结果摘要
            if [ -f "$base_name.valgrind" ]; then
                echo "**valgrind摘要:**" >> "$report_file"
                echo '```' >> "$report_file"
                grep -E "(ERROR SUMMARY|LEAK SUMMARY|definitely lost|indirectly lost)" "$base_name.valgrind" >> "$report_file" 2>/dev/null || echo "无内存问题" >> "$report_file"
                echo '```' >> "$report_file"
                echo "" >> "$report_file"
            fi
            
            # 添加gdb结果摘要
            if [ -f "$base_name.gdb" ]; then
                echo "**gdb摘要:**" >> "$report_file"
                echo '```' >> "$report_file"
                grep -A 5 -B 5 "Program received signal" "$base_name.gdb" >> "$report_file" 2>/dev/null || echo "无段错误" >> "$report_file"
                echo '```' >> "$report_file"
                echo "" >> "$report_file"
            fi
            
            echo "---" >> "$report_file"
            echo "" >> "$report_file"
        fi
    done
    
    echo "分析报告已生成: $report_file"
}

# 主函数
main() {
    echo "开始分析失败的测试..."
    
    # 确保插件已编译
    if [ ! -f "$BUILD_DIR/plugins/libcore_plugin.so" ]; then
        echo "编译插件..."
        cd "$BUILD_DIR"
        make plugins -j$(nproc) > /dev/null 2>&1
    fi
    
    # 获取失败的测试列表
    echo "获取失败的测试列表..."
    failed_tests=($(get_failed_tests))
    
    if [ ${#failed_tests[@]} -eq 0 ]; then
        echo "没有找到失败的测试"
        exit 0
    fi
    
    echo "找到 ${#failed_tests[@]} 个失败的测试"
    
    # 分析每个失败的测试
    for test_file in "${failed_tests[@]}"; do
        if [ -f "$UNIT_TESTS_DIR/$test_file" ]; then
            analyze_test "$UNIT_TESTS_DIR/$test_file"
        else
            echo "警告: 测试文件不存在: $UNIT_TESTS_DIR/$test_file"
        fi
    done
    
    # 生成分析报告
    generate_report
    
    echo ""
    echo "=== 分析完成 ==="
    echo "分析结果保存在: $OUTPUT_DIR"
    echo "查看报告: $OUTPUT_DIR/analysis_report.md"
}

# 运行主函数
main "$@"
