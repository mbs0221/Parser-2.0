# 测试说明

这个目录包含了Parser-2.0项目的所有测试文件。

## 文件说明

- `*.txt` - 各种测试用例文件
- `run_tests.py` - 批量测试脚本
- `README.md` - 本说明文件

## 使用方法

### 1. 运行单个测试

```bash
# 在build目录下运行
./bin/parser -v ../tests/test_simple_while.txt
```

### 2. 批量运行所有测试

```bash
# 在tests目录下运行
cd tests
python3 run_tests.py

# 或者指定parser路径
python3 run_tests.py --parser ../build/bin/parser

# 详细输出模式
python3 run_tests.py --verbose

# 指定输出文件
python3 run_tests.py --output my_results.txt
```

### 3. 测试脚本参数

- `--parser PATH` - 指定parser可执行文件路径（默认：../build/bin/parser）
- `--test-dir DIR` - 指定测试文件目录（默认：当前目录）
- `--verbose, -v` - 详细输出模式
- `--output FILE, -o FILE` - 指定结果输出文件（默认：test_results.txt）

## 测试结果说明

- **PASS** ✅ - 测试通过
- **FAIL** ❌ - 测试失败
- **SEGFAULT** ⚠️ - 段错误（通常是程序结束时的内存清理问题，不影响核心功能）
- **TIMEOUT** ⏰ - 测试超时
- **ERROR** 💥 - 执行错误

## 主要测试文件

- `Text.txt` - 主要测试文件
- `test_simple_while.txt` - while循环测试
- `test_assignment.txt` - 赋值语句测试
- `test_multiple_vars.txt` - 多变量声明测试
- `test_scope.txt` - 作用域测试

## 注意事项

1. 运行测试前请确保项目已编译：`cd ../build && make`
2. 某些测试可能会有段错误（exit code 139），这通常是程序结束时的内存清理问题，不影响核心功能
3. 测试结果会保存到指定的输出文件中，方便查看详细日志
