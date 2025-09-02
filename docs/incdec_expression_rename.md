# IncrementDecrementExpression 重命名为 IncDecExpression

## 重命名概述

我们将`IncrementDecrementExpression`重命名为`IncDecExpression`，使类名更加简洁和易读。

## 重命名范围

### 1. 头文件修改

#### include/parser/expression.h
```cpp
// 修改前
struct IncrementDecrementExpression : public UnaryExpression {
    // ... 成员定义 ...
    
    IncrementDecrementExpression(Expression* expr, bool prefix, int d)
        : UnaryExpression(expr, nullptr), isPrefix(prefix), delta(d) {}
    
    ~IncrementDecrementExpression() {
        // 基类UnaryExpression会处理operand的删除
    }
};

// 修改后
struct IncDecExpression : public UnaryExpression {
    // ... 成员定义 ...
    
    IncDecExpression(Expression* expr, bool prefix, int d)
        : UnaryExpression(expr, nullptr), isPrefix(prefix), delta(d) {}
    
    ~IncDecExpression() {
        // 基类UnaryExpression会处理operand的删除
    }
};
```

#### include/parser/ast_visitor.h
```cpp
// 修改前
class IncrementDecrementExpression;
// ...
virtual ReturnType visit(IncrementDecrementExpression* expr) = 0;

// 修改后
class IncDecExpression;
// ...
virtual ReturnType visit(IncDecExpression* expr) = 0;
```

#### include/interpreter/core/interpreter.h
```cpp
// 修改前
Value* visit(IncrementDecrementExpression* expr) override;

// 修改后
Value* visit(IncDecExpression* expr) override;
```

### 2. 实现文件修改

#### src/parser/expression.cpp
```cpp
// 修改前
// ==================== IncrementDecrementExpression实现 ====================
template<typename ReturnType>
ReturnType IncrementDecrementExpression::accept(ExpressionVisitor<ReturnType>* visitor) {
    return visitor->visit(this);
}

// 修改后
// ==================== IncDecExpression实现 ====================
template<typename ReturnType>
ReturnType IncDecExpression::accept(ExpressionVisitor<ReturnType>* visitor) {
    return visitor->visit(this);
}
```

#### src/interpreter/core/expression.cpp
```cpp
// 修改前
} else if (IncrementDecrementExpression* incDecExpr = dynamic_cast<IncrementDecrementExpression*>(expr)) {
    return visit(incDecExpr);
}

// 修改后
} else if (IncDecExpression* incDecExpr = dynamic_cast<IncDecExpression*>(expr)) {
    return visit(incDecExpr);
}

// 修改前
// ==================== IncrementDecrementExpression访问方法 ====================
Value* Interpreter::visit(IncrementDecrementExpression* expr) {

// 修改后
// ==================== IncDecExpression访问方法 ====================
Value* Interpreter::visit(IncDecExpression* expr) {
```

#### src/parser/parser.cpp
```cpp
// 修改前 - 前缀自增
return new IncrementDecrementExpression(operand, true, 1);

// 修改后 - 前缀自增
return new IncDecExpression(operand, true, 1);

// 修改前 - 前缀自减
return new IncrementDecrementExpression(operand, true, -1);

// 修改后 - 前缀自减
return new IncDecExpression(operand, true, -1);

// 修改前 - 后缀自增
expr = new IncrementDecrementExpression(expr, false, 1);

// 修改后 - 后缀自增
expr = new IncDecExpression(expr, false, 1);

// 修改前 - 后缀自减
expr = new IncrementDecrementExpression(expr, false, -1);

// 修改后 - 后缀自减
expr = new IncDecExpression(expr, false, -1);
```

## 重命名优势

### 1. **名称更简洁**
- `IncrementDecrementExpression` → `IncDecExpression`
- 从25个字符减少到18个字符
- 减少了28%的字符数

### 2. **更易读**
- `IncDec` 比 `IncrementDecrement` 更容易阅读
- 保持了语义的清晰性
- 符合现代编程语言的命名习惯

### 3. **保持一致性**
- 与其他表达式类名保持一致的命名风格
- 如 `UnaryExpression`、`BinaryExpression`、`TernaryExpression` 等

### 4. **减少输入错误**
- 更短的名称减少了拼写错误的可能性
- 提高了开发效率

## 功能保持不变

### 1. **继承关系**
- 仍然继承自 `UnaryExpression`
- 保持了所有原有的功能特性

### 2. **成员变量**
- `isPrefix`: 是否为前缀操作
- `delta`: 变化量（+1表示自增，-1表示自减）

### 3. **方法功能**
- `isPrefixOperation()`: 检查是否为前缀操作
- `isPostfixOperation()`: 检查是否为后缀操作
- `isIncrement()`: 检查是否为自增操作
- `isDecrement()`: 检查是否为自减操作
- `getDelta()`: 获取增量值

### 4. **优先级处理**
- 前缀操作优先级：6
- 后缀操作优先级：2

## 总结

通过这次重命名，我们成功地：

1. **简化了类名** - 从冗长的 `IncrementDecrementExpression` 简化为简洁的 `IncDecExpression`
2. **提高了可读性** - 新的名称更加直观和易读
3. **保持了功能完整性** - 所有原有功能都得到保留
4. **改善了代码质量** - 更符合现代编程语言的命名规范

这种重命名方式体现了"简洁就是美"的设计原则，使代码更加清晰和易于维护。
