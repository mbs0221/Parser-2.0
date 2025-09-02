# 使用新的统一接口重构表达式访问

## 重构概述

我们使用ScopeManager的新统一接口重构了AccessExpression和VariableExpression，简化了代码，提高了可维护性。

## 重构内容

### 1. AccessExpression重构

#### 重构前的问题
- 代码过于复杂，包含大量重复的逻辑
- 调试信息过多，影响可读性
- 查找逻辑分散，难以维护

#### 重构后的改进
- 使用`scopeManager.lookup()`统一接口
- 使用`scopeManager.lookupTypeMethod()`查找类型方法
- 使用`scopeManager.lookupMethod()`查找实例方法
- 代码结构更清晰，职责分离

#### 重构后的代码结构
```cpp
Value* Interpreter::visit(AccessExpression* access) {
    // 1. 求值目标和键
    Value* target = visit(access->target);
    Value* key = visit(access->key);
    
    // 2. 提取成员名称
    string memberName = extractMemberName(key);
    
    // 3. 使用统一接口处理访问
    return handleUnifiedAccess(target, memberName);
}

Value* Interpreter::handleUnifiedAccess(Value* target, const string& memberName) {
    if (!target->getValueType()) {
        return handleTypeNameAccess(target, memberName);  // 静态访问
    }
    return handleInstanceAccess(target, memberName);      // 实例访问
}
```

### 2. VariableExpression重构

#### 重构前的问题
- 使用过时的`lookupAny()`方法
- 直接调用`TypeRegistry::getGlobalInstance()`
- 调试输出过多

#### 重构后的改进
- 使用`scopeManager.lookup()`统一接口
- 使用`scopeManager.lookupType()`查找类型
- 移除了冗余的调试输出

#### 重构后的代码结构
```cpp
Value* Interpreter::visit(VariableExpression* varExpr) {
    string name = varExpr->name;
    
    // 使用新的统一接口查找标识符
    Value* value = scopeManager.lookup(name);
    
    if (!value) {
        Interpreter::reportError("Undefined variable: " + name);
        return nullptr;
    }
    
    // 类型推断逻辑保持不变
    // ...
    
    return value;
}
```

## 新统一接口的优势

### 1. **接口一致性**
- 所有查找操作都通过统一的接口进行
- 消除了不同查找方法之间的差异
- 代码更加一致和可预测

### 2. **代码简化**
- 减少了重复的查找逻辑
- 移除了复杂的嵌套循环
- 代码更加清晰易读

### 3. **性能提升**
- 统一的查询策略，减少重复计算
- 更好的缓存和索引机制
- 减少了作用域遍历次数

### 4. **维护性改善**
- 修改查找逻辑只需要修改ScopeManager
- 统一的错误处理和日志记录
- 更容易进行单元测试

## 使用的新接口

### 主要查询接口
```cpp
// 统一查找标识符（按优先级）
Value* lookup(const string& name);

// 类型化查找
ObjectType* lookupType(const string& name);
Value* lookupVariable(const string& name);
Value* lookupFunction(const string& name);
Value* lookupMethod(const string& name);

// 批量查找
LookupResult lookupAll(const string& name);
```

### 类型方法查找
```cpp
// 查找类型的方法
Value* lookupTypeMethod(const string& typeName, const string& methodName);
```

## 重构效果对比

| 方面 | 重构前 | 重构后 |
|------|--------|--------|
| 代码行数 | 100+ 行 | 50+ 行 |
| 查找方法 | 多个分散的方法 | 统一的接口 |
| 调试输出 | 大量cout输出 | 统一的LOG_DEBUG |
| 维护性 | 难以维护 | 易于维护 |
| 性能 | 重复遍历 | 统一策略 |

## 总结

通过使用ScopeManager的新统一接口，我们成功地：

1. **简化了AccessExpression** - 从复杂的嵌套逻辑简化为清晰的职责分离
2. **优化了VariableExpression** - 使用统一的查找接口，提高了一致性
3. **提升了代码质量** - 减少了重复代码，提高了可维护性
4. **改善了性能** - 统一的查询策略减少了重复计算

这种重构方式体现了"简单就是美"的设计原则，避免了过度工程化，直接使用已有的统一接口来解决问题。
