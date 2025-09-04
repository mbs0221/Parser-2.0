# Parser-2.0 使用指南

## 🚀 快速开始

### 编译项目
```bash
cd /home/bsmei/mbs0221/Parser-2.0
mkdir -p build && cd build
cmake .. && make -j4
```

### 运行程序
```bash
# 基本用法
./build/bin/parser_main programs/test_basic.txt

# 使用调试模式
./build/bin/parser_main -v programs/test_math_functions.txt

# 查看帮助
./build/bin/parser_main --help
```

## 📋 支持的功能

### ✅ 完全支持的功能
- **基本语法**: 变量声明、赋值、表达式
- **数学运算**: 加减乘除、比较运算、逻辑运算
- **函数定义**: 无参数函数定义和调用
- **内置函数**: print、println、数学函数等
- **类型系统**: int、double、string、bool 等基本类型
- **条件语句**: if-else 语句
- **日志系统**: 自动轮转的日志功能

### ⚠️ 部分支持的功能
- **控制流**: while、for 循环（需要在函数内使用）
- **复杂函数**: 带参数的函数定义
- **类型方法**: 内置类型的方法调用
- **集合类型**: 数组和字典的基本操作

### ❌ 暂不支持的功能
- **自增自减**: `++` 和 `--` 操作符（会导致段错误）
- **复杂控制流**: 在全局作用域使用控制流语句
- **高级集合操作**: 复杂的数组和字典操作

## 📁 测试程序说明

### 推荐测试程序
```bash
# 基础功能测试
./build/bin/parser_main programs/test_basic.txt
./build/bin/parser_main programs/test_arithmetic.txt
./build/bin/parser_main programs/test_print.txt

# 数学运算测试
./build/bin/parser_main programs/test_math.txt
./build/bin/parser_main programs/test_math_functions.txt

# 函数测试
./build/bin/parser_main programs/test_simple_function.txt
./build/bin/parser_main programs/test_basic_function.txt

# 变量和表达式测试
./build/bin/parser_main programs/test_simple_var.txt
./build/bin/parser_main programs/test_simple_expr.txt
./build/bin/parser_main programs/test_simple_plus.txt
```

### 避免使用的程序
```bash
# 这些程序可能会导致段错误或解析失败
# programs/test_increment_simple.txt  # 段错误
# programs/test_control_flow_fixed.txt  # 解析失败
# programs/test_collections.txt  # 解析失败
```

## 🔧 命令行选项

```bash
./build/bin/parser_main [OPTIONS] <file>

选项:
  -l, --log-level LEVEL    设置日志级别 (DEBUG, INFO, WARN, ERROR, FATAL)
  -v, --verbose           启用详细输出 (等同于 --log-level DEBUG)
  -q, --quiet             启用静默输出 (等同于 --log-level ERROR)
  -p, --plugins PLUGINS   加载指定插件 (逗号分隔，默认: basic)
  -n, --no-plugins        禁用插件加载
  -h, --help              显示帮助信息
```

## 📊 批量测试

### 运行所有测试
```bash
./test_all_programs.sh
```

### 快速状态检查
```bash
./quick_fixes.sh
```

## 📝 语法示例

### 基本语法
```javascript
// 变量声明和赋值
let x = 10;
let y = 20;
let result = x + y;

// 打印输出
println("结果: ", result);
```

### 函数定义
```javascript
// 无参数函数
function greet() {
    println("Hello, World!");
}

// 调用函数
greet();
```

### 条件语句
```javascript
let age = 18;
if (age >= 18) {
    println("成年人");
} else {
    println("未成年人");
}
```

### 数学运算
```javascript
let a = 10;
let b = 5;
println("加法: ", a + b);
println("减法: ", a - b);
println("乘法: ", a * b);
println("除法: ", a / b);
```

## 🐛 常见问题

### 1. 段错误
**问题**: 运行某些程序时出现段错误
**原因**: 通常是自增自减操作符或复杂语法导致
**解决**: 避免使用 `++` 和 `--` 操作符

### 2. 解析失败
**问题**: 程序无法解析
**原因**: 语法不符合解析器要求
**解决**: 确保控制流语句在函数内使用

### 3. 方法未找到
**问题**: 调用不存在的方法
**原因**: 类型系统方法不完整
**解决**: 使用支持的内置函数

## 📈 性能指标

- **编译时间**: 约 30-60 秒
- **运行速度**: 基本程序 < 1 秒
- **内存使用**: 约 10-50MB
- **测试通过率**: 76% (16/21)

## 🔮 未来计划

1. **修复自增自减操作符**
2. **完善控制流语句支持**
3. **改进类型系统**
4. **添加更多内置函数**
5. **支持模块导入**

## 📞 技术支持

如果遇到问题，请：
1. 检查程序语法是否符合要求
2. 查看日志文件 `parser.log`
3. 使用调试模式运行程序
4. 参考测试程序示例

---

**注意**: 这是一个开发中的解析器项目，某些功能可能不完整或存在已知问题。建议使用经过测试的程序进行验证。
