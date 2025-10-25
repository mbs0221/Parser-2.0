#!/bin/bash

# LibFuzzer运行脚本
# 用于运行lexer、parser和interpreter的模糊测试

set -e

# 配置
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
FUZZ_DIR="$PROJECT_ROOT/fuzz"
BUILD_DIR="$PROJECT_ROOT/build"
CORPUS_DIR="$FUZZ_DIR/corpus"
OUTPUT_DIR="$FUZZ_DIR/outputs"
TIMESTAMP=$(date +"%Y%m%d_%H%M%S")

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
    
    # 检查clang
    if ! command -v clang++ &> /dev/null; then
        log_error "clang++ 未安装，LibFuzzer需要clang++"
        exit 1
    fi
    
    # 检查cmake
    if ! command -v cmake &> /dev/null; then
        log_error "cmake 未安装"
        exit 1
    fi
    
    log_success "依赖检查完成"
}

# 构建fuzzing目标
build_fuzzers() {
    log_info "构建fuzzing目标..."
    
    cd "$PROJECT_ROOT"
    
    # 创建fuzz构建目录
    mkdir -p "$BUILD_DIR/fuzz"
    cd "$BUILD_DIR/fuzz"
    
    # 配置cmake
    cmake -DCMAKE_BUILD_TYPE=Debug \
          -DCMAKE_CXX_COMPILER=clang++ \
          -DCMAKE_CXX_FLAGS="-fsanitize=fuzzer,address,undefined" \
          "$FUZZ_DIR"
    
    # 构建
    make -j$(nproc)
    
    if [ $? -eq 0 ]; then
        log_success "Fuzzing目标构建完成"
    else
        log_error "Fuzzing目标构建失败"
        exit 1
    fi
}

# 运行lexer fuzzing
run_lexer_fuzzing() {
    log_info "开始lexer fuzzing..."
    
    local runs=${1:-1000}
    local corpus_dir="$CORPUS_DIR/lexer"
    local output_dir="$OUTPUT_DIR/lexer_$TIMESTAMP"
    
    mkdir -p "$output_dir"
    
    # 运行lexer fuzzer
    "$BUILD_DIR/fuzz/fuzz/lexer_fuzzer" \
        -runs="$runs" \
        -max_len=1000 \
        -timeout=10 \
        -artifact_prefix="$output_dir/" \
        "$corpus_dir"
    
    log_success "Lexer fuzzing完成，结果保存在: $output_dir"
}

# 运行parser fuzzing
run_parser_fuzzing() {
    log_info "开始parser fuzzing..."
    
    local runs=${1:-1000}
    local corpus_dir="$CORPUS_DIR/parser"
    local output_dir="$OUTPUT_DIR/parser_$TIMESTAMP"
    
    mkdir -p "$output_dir"
    
    # 运行parser fuzzer
    "$BUILD_DIR/fuzz/fuzz/parser_fuzzer" \
        -runs="$runs" \
        -max_len=2000 \
        -timeout=15 \
        -artifact_prefix="$output_dir/" \
        "$corpus_dir"
    
    log_success "Parser fuzzing完成，结果保存在: $output_dir"
}

# 运行interpreter fuzzing
run_interpreter_fuzzing() {
    log_info "开始interpreter fuzzing..."
    
    local runs=${1:-1000}
    local corpus_dir="$CORPUS_DIR/interpreter"
    local output_dir="$OUTPUT_DIR/interpreter_$TIMESTAMP"
    
    mkdir -p "$output_dir"
    
    # 运行interpreter fuzzer
    "$BUILD_DIR/fuzz/fuzz/interpreter_fuzzer" \
        -runs="$runs" \
        -max_len=5000 \
        -timeout=30 \
        -artifact_prefix="$output_dir/" \
        "$corpus_dir"
    
    log_success "Interpreter fuzzing完成，结果保存在: $output_dir"
}

# 运行所有fuzzing
run_all_fuzzing() {
    log_info "开始运行所有fuzzing测试..."
    
    local runs=${1:-1000}
    
    run_lexer_fuzzing "$runs"
    run_parser_fuzzing "$runs"
    run_interpreter_fuzzing "$runs"
    
    log_success "所有fuzzing测试完成"
}

# 分析fuzzing结果
analyze_results() {
    log_info "分析fuzzing结果..."
    
    local output_dir="$OUTPUT_DIR"
    
    if [ ! -d "$output_dir" ]; then
        log_warning "没有找到输出目录: $output_dir"
        return
    fi
    
    # 统计崩溃数量
    local crash_count=0
    for dir in "$output_dir"/*; do
        if [ -d "$dir" ]; then
            local crashes=$(find "$dir" -name "crash-*" | wc -l)
            crash_count=$((crash_count + crashes))
        fi
    done
    
    echo "Fuzzing结果摘要:"
    echo "=================="
    echo "总崩溃数: $crash_count"
    echo "输出目录: $output_dir"
    
    if [ $crash_count -gt 0 ]; then
        echo ""
        echo "发现的崩溃:"
        find "$output_dir" -name "crash-*" -exec echo "  {}" \;
    fi
}

# 清理函数
cleanup() {
    log_info "清理临时文件..."
    # 这里可以添加清理逻辑
}

# 主函数
main() {
    log_info "开始LibFuzzer测试流程..."
    echo "项目根目录: $PROJECT_ROOT"
    echo "时间戳: $TIMESTAMP"
    echo ""
    
    # 设置错误处理
    trap cleanup EXIT
    
    # 执行步骤
    check_dependencies
    build_fuzzers
    
    # 根据参数选择运行模式
    case "${1:-all}" in
        "lexer")
            run_lexer_fuzzing "${2:-1000}"
            ;;
        "parser")
            run_parser_fuzzing "${2:-1000}"
            ;;
        "interpreter")
            run_interpreter_fuzzing "${2:-1000}"
            ;;
        "all")
            run_all_fuzzing "${2:-1000}"
            ;;
        *)
            log_error "未知模式: $1"
            show_help
            exit 1
            ;;
    esac
    
    analyze_results
    
    log_success "LibFuzzer测试完成！"
}

# 帮助信息
show_help() {
    echo "LibFuzzer运行脚本"
    echo ""
    echo "用法: $0 [模式] [运行次数]"
    echo ""
    echo "模式:"
    echo "  lexer       只运行lexer fuzzing"
    echo "  parser      只运行parser fuzzing"
    echo "  interpreter 只运行interpreter fuzzing"
    echo "  all         运行所有fuzzing (默认)"
    echo ""
    echo "运行次数: 每个fuzzer的运行次数 (默认: 1000)"
    echo ""
    echo "示例:"
    echo "  $0                   运行所有fuzzing，每个1000次"
    echo "  $0 lexer 5000       运行lexer fuzzing，5000次"
    echo "  $0 parser 2000      运行parser fuzzing，2000次"
    echo "  $0 interpreter 1000 运行interpreter fuzzing，1000次"
}

# 解析命令行参数
if [[ "$1" == "-h" || "$1" == "--help" ]]; then
    show_help
    exit 0
fi

# 运行主函数
main "$@"
