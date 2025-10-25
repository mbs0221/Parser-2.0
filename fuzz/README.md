# LibFuzzer 模糊测试框架

本目录包含了使用LibFuzzer对Parser-2.0项目进行模糊测试的完整框架。

## 目录结构

```
fuzz/
├── lexer/              # Lexer模糊测试
│   └── lexer_fuzzer.cpp
├── parser/             # Parser模糊测试
│   └── parser_fuzzer.cpp
├── interpreter/        # Interpreter模糊测试
│   └── interpreter_fuzzer.cpp
├── corpus/             # 测试语料库
│   ├── lexer/          # Lexer测试用例
│   ├── parser/         # Parser测试用例
│   └── interpreter/    # Interpreter测试用例
├── outputs/            # Fuzzing输出结果
├── scripts/            # 运行脚本
│   └── run_fuzzing.sh
├── CMakeLists.txt      # 构建配置
└── README.md          # 本文件
```

## 功能特性

### 1. Lexer Fuzzing
- 测试词法分析器对各种输入的处理能力
- 验证token识别的健壮性
- 检查字符串、数字、标识符等token的解析
- 测试特殊字符和边界情况

### 2. Parser Fuzzing
- 测试语法分析器对复杂语法的处理
- 验证AST构建的正确性
- 检查表达式、语句、函数等的解析
- 测试语法错误的恢复能力

### 3. Interpreter Fuzzing
- 测试解释器对程序执行的健壮性
- 验证值计算和类型转换
- 检查内存管理和异常处理
- 测试复杂程序的控制流

## 使用方法

### 1. 构建Fuzzing目标

```bash
# 进入项目根目录
cd /path/to/Parser-2.0

# 运行构建脚本
./fuzz/scripts/run_fuzzing.sh
```

### 2. 运行特定组件的Fuzzing

```bash
# 只运行lexer fuzzing
./fuzz/scripts/run_fuzzing.sh lexer 1000

# 只运行parser fuzzing
./fuzz/scripts/run_fuzzing.sh parser 2000

# 只运行interpreter fuzzing
./fuzz/scripts/run_fuzzing.sh interpreter 1000
```

### 3. 运行所有Fuzzing

```bash
# 运行所有组件的fuzzing
./fuzz/scripts/run_fuzzing.sh all 1000
```

## 配置选项

### 运行参数
- `runs`: 每个fuzzer的运行次数
- `max_len`: 输入数据的最大长度
- `timeout`: 单个测试的超时时间
- `artifact_prefix`: 崩溃文件的保存前缀

### 内存安全选项
- AddressSanitizer: 检测内存错误
- UndefinedBehaviorSanitizer: 检测未定义行为
- LibFuzzer: 提供模糊测试引擎

## 结果分析

### 输出文件
- `crash-*`: 导致崩溃的输入
- `leak-*`: 内存泄漏的输入
- `timeout-*`: 超时的输入
- `slow-unit-*`: 执行缓慢的输入

### 分析工具
```bash
# 分析fuzzing结果
./fuzz/scripts/run_fuzzing.sh
# 脚本会自动分析结果并显示摘要
```

## 测试语料库

### Lexer语料库
- `basic_tokens.txt`: 基本token测试
- `edge_cases.txt`: 边界情况测试

### Parser语料库
- `basic_expressions.txt`: 基本表达式测试

### Interpreter语料库
- `simple_programs.txt`: 简单程序测试

## 最佳实践

### 1. 渐进式测试
- 先运行少量测试验证框架
- 逐步增加测试次数
- 监控系统资源使用

### 2. 结果分析
- 定期检查崩溃报告
- 分析崩溃模式
- 修复发现的问题

### 3. 语料库维护
- 定期更新测试用例
- 添加新的边界情况
- 保持语料库的多样性

## 故障排除

### 常见问题

1. **构建失败**
   - 检查clang++是否安装
   - 确认CMake版本 >= 3.16
   - 检查依赖库是否完整

2. **运行时崩溃**
   - 检查输入数据大小限制
   - 调整超时设置
   - 查看详细错误信息

3. **性能问题**
   - 减少运行次数
   - 调整输入长度限制
   - 使用更快的硬件

### 调试技巧

```bash
# 使用gdb调试崩溃
gdb --args ./build/fuzz/fuzz/lexer_fuzzer crash-xxx

# 使用valgrind检查内存问题
valgrind --tool=memcheck ./build/fuzz/fuzz/lexer_fuzzer crash-xxx

# 查看详细输出
./build/fuzz/fuzz/lexer_fuzzer -runs=100 -print_final_stats=1
```

## 贡献指南

1. 添加新的测试用例到corpus目录
2. 改进fuzzer的验证逻辑
3. 优化测试性能和覆盖率
4. 修复发现的问题

## 许可证

与主项目保持一致的许可证。
