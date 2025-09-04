# Examples 目录结构

本目录包含了Parser-2.0项目的示例代码，按照功能模块进行了分类组织。

## 目录结构

```
examples/
├── lexer/          # 词法分析器相关示例
│   └── (暂无文件)   # 目前没有专门的lexer示例文件
├── parser/         # 语法分析器相关示例
│   ├── *.cpp       # C++示例文件（类型系统、插件系统等）
│   └── *.txt       # 模块系统示例文件
└── README.md       # 本说明文件
```

## 文件分类说明

### Lexer 相关文件
- **位置**: `lexer/` 目录
- **内容**: 词法分析器相关的示例代码
- **状态**: 目前暂无专门的lexer示例文件

### Parser 相关文件
- **位置**: `parser/` 目录
- **内容**: 语法分析器相关的示例代码，包括：

#### C++ 示例文件
- `demo_dependency_injection_initial_values.cpp` - 依赖注入初始值计算演示
- `demo_struct_type_builder.cpp` - 结构体类型构建器演示
- `demo_type_factory_builders.cpp` - 类型工厂构建器演示
- `dynamic_loading_example.cpp` - 动态库加载示例
- `improved_method_access_example.cpp` - 改进的方法访问示例
- 其他C++示例文件（部分为空文件）

#### 模块系统示例文件
- 注意：模块系统相关的txt文件已移动到 `programs/` 目录
- 包括：`collections_module.txt`, `math_module.txt`, `string_module.txt`, `utils_module.txt`, `module_system_example.txt`

## 使用说明

1. **C++示例**: 这些文件展示了如何使用Parser-2.0的C++ API，包括类型系统、插件系统等功能
2. **模块系统示例**: 这些.txt文件展示了如何定义和使用模块系统
3. **路径引用**: 模块系统示例中的import路径已经更新为新的目录结构

## 注意事项

- 所有模块系统示例文件中的import路径已经更新为 `examples/parser/` 前缀
- 如果添加新的lexer相关示例，请放在 `lexer/` 目录中
- 如果添加新的parser相关示例，请放在 `parser/` 目录中
