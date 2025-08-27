# Parser项目测试套件

本目录包含Parser项目的单元测试，使用Google Test框架。

## 测试结构

### 模块化测试目录
- `lexer/` - 词法分析器相关测试
- `parser/` - 语法分析器相关测试
- `interpreter/` - 解释器相关测试
- `optimizer/` - 优化器相关测试

### 测试框架
使用Google Test框架进行单元测试，每个模块都有独立的测试套件。

### 测试文件分类

#### Lexer测试
- `test_lexer_comprehensive.cpp` - 综合测试
- `test_lexer.cpp` - 基本测试
- `test_value_variables.cpp` - 值变量测试
- `test_string_operations.cpp` - 字符串操作测试
- `test_operator_design.cpp` - 运算符设计测试
- `test_operator_type.cpp` - 运算符类型测试
- `test_merged_target_type.cpp` - 合并目标类型测试

#### Parser测试
- `test_parser_comprehensive.cpp` - 综合测试
- `test_parser.cpp` - 基本测试
- `test_parser_constant.cpp` - 常量测试
- `test_parser_helpers.cpp` - 辅助函数测试
- `test_parser_simple.cpp` - 简单测试
- `test_constant_expression.cpp` - 常量表达式测试
- `test_expression_parsing.cpp` - 表达式解析测试
- `test_leaf_expression.cpp` - 叶子表达式测试
- `test_simplified_expressions.cpp` - 简化表达式测试
- `test_simplified_binary.cpp` - 简化二元表达式测试
- `test_cast_expression.cpp` - 强制转换表达式测试
- `test_assignment_binary.cpp` - 赋值二元表达式测试
- `test_ast_only.cpp` - AST结构测试

#### Interpreter测试
- `test_interpreter_comprehensive.cpp` - 综合测试
- `test_interpreter.cpp` - 基本测试
- `test_interpreter_execution.cpp` - 执行测试
- `test_interpreter_simple.cpp` - 简单测试
- `test_simple_interpreter.cpp` - 简单解释器测试
- `test_builtin_function.cpp` - 内置函数测试
- `test_builtin_functions_as_identifiers.cpp` - 内置函数作为标识符测试
- `test_builtin_refactor.cpp` - 内置函数重构测试
- `test_visitor.cpp` - 访问者模式测试
- `test_value_visitor.cpp` - 值访问者测试

#### Optimizer测试
- `test_optimizer_comprehensive.cpp` - 综合测试
- `test_optimizer.cpp` - 基本测试
- `test_operator_precedence.cpp` - 运算符优先级测试
- `test_updated_parser.cpp` - 更新解析器测试

### 原有测试文件
以下是从根目录移动过来的原有测试文件：
- `test_*.cpp` - 各种功能测试文件

## 运行测试

### 方法1：使用测试脚本
```bash
# 在项目根目录运行
./run_tests.sh
```

### 方法2：手动编译和运行
```bash
# 创建并进入build目录
mkdir -p build && cd build

# 配置CMake
cmake .. -DCMAKE_BUILD_TYPE=Debug

# 编译
make -j$(nproc)

# 运行特定测试
./test_lexer
./test_parser
./test_interpreter
```

### 方法3：使用CMake测试
```bash
cd build
make test
# 或者直接运行
ctest
```

## 测试覆盖范围

### Lexer测试
- 基本类型定义（int, double, bool, string）
- 运算符定义和优先级
- 值类型创建和操作
- 布尔常量
- 类型比较

### Parser测试
- 常量表达式
- 变量表达式
- 二元表达式
- 表达式语句
- 变量声明
- 程序结构

### Value测试
- 整数运算
- 浮点数运算
- 布尔运算
- 字符串运算
- 类型转换
- 比较运算

### Interpreter测试
- AST结构验证
- 表达式求值
- 语句执行
- 访问者模式

## 添加新测试

1. 创建新的测试文件，例如 `test_new_feature.cpp`
2. 使用Google Test框架编写测试用例
3. 在 `CMakeLists.txt` 中添加新的测试文件
4. 重新编译并运行测试

## 测试框架

本项目使用Google Test框架，提供以下功能：
- 测试夹具（Test Fixtures）
- 断言宏（EXPECT_*, ASSERT_*）
- 参数化测试
- 死亡测试
- 测试发现和运行

## 注意事项

- 所有测试文件都应该包含 `#include <gtest/gtest.h>`
- 测试类应该继承自 `::testing::Test`
- 使用 `TEST_F` 宏定义测试用例
- 记得在测试中正确管理内存（delete指针）
- 测试应该独立且可重复运行
