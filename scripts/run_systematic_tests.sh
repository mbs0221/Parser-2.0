#!/bin/bash

# 系统化测试执行脚本
# 用于执行所有unit-tests，分析结果，生成修复建议

set -e

# 配置
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
SCRIPTS_DIR="$PROJECT_ROOT/scripts"
UNIT_TESTS_DIR="$PROJECT_ROOT/unit-tests"
TIMEOUT=5
TIMESTAMP=$(date +"%Y%m%d_%H%M%S")

# 输出目录
OUTPUT_DIR="$PROJECT_ROOT/test_results"
REPORT_FILE="$OUTPUT_DIR/test_report_$TIMESTAMP.json"
ANALYSIS_FILE="$OUTPUT_DIR/analysis_report_$TIMESTAMP.md"
ACTION_PLAN_FILE="$OUTPUT_DIR/action_plan_$TIMESTAMP.md"

# 颜色输出
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# 日志函数
log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# 检查依赖
check_dependencies() {
    log_info "检查依赖项..."
    
    # 检查Python
    if ! command -v python3 &> /dev/null; then
        log_error "Python3 未安装"
        exit 1
    fi
    
    # 检查可执行文件
    if [ ! -f "$PROJECT_ROOT/build/bin/interpreter_main" ] && \
       [ ! -f "$PROJECT_ROOT/build/bin/parser_main" ] && \
       [ ! -f "$PROJECT_ROOT/build/bin/lexer_main" ]; then
        log_warning "未找到可执行文件，尝试构建项目..."
        cd "$PROJECT_ROOT"
        if [ -f "CMakeLists.txt" ]; then
            mkdir -p build
            cd build
            cmake ..
            make -j$(nproc)
            cd ..
        else
            log_error "未找到CMakeLists.txt文件"
            exit 1
        fi
    fi
    
    # 检查测试目录
    if [ ! -d "$UNIT_TESTS_DIR" ]; then
        log_error "测试目录不存在: $UNIT_TESTS_DIR"
        exit 1
    fi
    
    log_success "依赖检查完成"
}

# 创建输出目录
setup_output_dir() {
    log_info "设置输出目录..."
    mkdir -p "$OUTPUT_DIR"
    log_success "输出目录已创建: $OUTPUT_DIR"
}

# 运行测试
run_tests() {
    log_info "开始运行系统化测试..."
    log_info "测试目录: $UNIT_TESTS_DIR"
    log_info "超时设置: ${TIMEOUT}秒"
    
    cd "$PROJECT_ROOT"
    
    # 运行测试
    python3 "$SCRIPTS_DIR/systematic_test_runner.py" \
        --timeout "$TIMEOUT" \
        --test-dir "unit-tests" \
        --output "$REPORT_FILE"
    
    if [ $? -eq 0 ]; then
        log_success "测试执行完成"
    else
        log_error "测试执行失败"
        exit 1
    fi
}

# 分析测试结果
analyze_results() {
    log_info "分析测试结果..."
    
    if [ ! -f "$REPORT_FILE" ]; then
        log_error "测试报告文件不存在: $REPORT_FILE"
        exit 1
    fi
    
    # 生成分析报告
    python3 "$SCRIPTS_DIR/test_analyzer.py" \
        "$REPORT_FILE" \
        --output "$ANALYSIS_FILE"
    
    if [ $? -eq 0 ]; then
        log_success "分析报告已生成: $ANALYSIS_FILE"
    else
        log_error "分析报告生成失败"
        exit 1
    fi
}

# 生成修复建议
generate_fix_suggestions() {
    log_info "生成修复建议..."
    
    # 生成行动计划
    python3 "$SCRIPTS_DIR/test_fix_suggestions.py" \
        "$REPORT_FILE" \
        --output "$ACTION_PLAN_FILE"
    
    if [ $? -eq 0 ]; then
        log_success "行动计划已生成: $ACTION_PLAN_FILE"
    else
        log_error "行动计划生成失败"
        exit 1
    fi
    
    # 显示快速修复建议
    log_info "快速修复建议:"
    python3 "$SCRIPTS_DIR/test_fix_suggestions.py" \
        "$REPORT_FILE" \
        --quick-wins
}

# 显示摘要
show_summary() {
    log_info "测试摘要:"
    echo "=================================="
    
    # 显示文件位置
    echo "测试报告: $REPORT_FILE"
    echo "分析报告: $ANALYSIS_FILE"
    echo "行动计划: $ACTION_PLAN_FILE"
    echo ""
    
    # 显示测试统计
    if [ -f "$REPORT_FILE" ]; then
        echo "测试统计:"
        python3 -c "
import json
with open('$REPORT_FILE', 'r') as f:
    data = json.load(f)
    print(f'  总测试数: {data[\"total_tests\"]}')
    stats = data['statistics']
    for status, count in stats.items():
        if count > 0:
            print(f'  {status}: {count}')
"
    fi
    
    echo ""
    log_success "系统化测试完成！"
}

# 清理函数
cleanup() {
    log_info "清理临时文件..."
    # 这里可以添加清理逻辑
}

# 主函数
main() {
    log_info "开始系统化测试流程..."
    echo "项目根目录: $PROJECT_ROOT"
    echo "时间戳: $TIMESTAMP"
    echo ""
    
    # 设置错误处理
    trap cleanup EXIT
    
    # 执行步骤
    check_dependencies
    setup_output_dir
    run_tests
    analyze_results
    generate_fix_suggestions
    show_summary
    
    log_success "所有步骤完成！"
}

# 帮助信息
show_help() {
    echo "系统化测试执行脚本"
    echo ""
    echo "用法: $0 [选项]"
    echo ""
    echo "选项:"
    echo "  -t, --timeout SECONDS    设置测试超时时间 (默认: 5)"
    echo "  -o, --output DIR         设置输出目录 (默认: test_results)"
    echo "  -h, --help              显示此帮助信息"
    echo ""
    echo "示例:"
    echo "  $0                      运行默认测试"
    echo "  $0 -t 60                设置60秒超时"
    echo "  $0 -o /tmp/results      指定输出目录"
}

# 解析命令行参数
while [[ $# -gt 0 ]]; do
    case $1 in
        -t|--timeout)
            TIMEOUT="$2"
            shift 2
            ;;
        -o|--output)
            OUTPUT_DIR="$2"
            shift 2
            ;;
        -h|--help)
            show_help
            exit 0
            ;;
        *)
            log_error "未知选项: $1"
            show_help
            exit 1
            ;;
    esac
done

# 运行主函数
main
