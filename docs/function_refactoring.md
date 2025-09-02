# ScopeManager 函数重构说明

## 重构目标

将复杂的for循环拆分到独立的函数中，确保每个函数功能明确，职责细分，提高代码的可读性和可维护性。

## 重构前的代码问题

### 1. `lookupFunction` 方法过于复杂

重构前的 `lookupFunction` 方法包含了多个嵌套的for循环和复杂的逻辑：

```cpp
Value* ScopeManager::lookupFunction(const string& name) {
    // 1. 首先尝试直接查找
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        Scope* scope = *it;
        if (scope && scope->objectRegistry) {
            Value* callable = scope->objectRegistry->lookupCallable(name);
            if (callable && callable->isCallable()) {
                return callable;
            }
        }
    }
    
    // 2. 尝试通过函数名查找
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        Scope* scope = *it;
        if (scope && scope->objectRegistry) {
            const auto& callables = scope->objectRegistry->getCallableObjects();
            for (const auto& pair : callables) {
                Value* callable = pair.second;
                if (callable && callable->isCallable()) {
                    if (Function* func = dynamic_cast<Function*>(callable)) {
                        if (func->getName() == name) {
                            return callable;
                        }
                    }
                }
            }
        }
    }
    
    // 3. 尝试在类型系统中查找同名类型（构造函数）
    ObjectType* type = TypeRegistry::getGlobalInstance()->getType(name);
    if (type) {
        if (type->hasStaticMethodName(name)) {
            return new ClassMethodValue(type, name);
        }
        if (type->hasMethodName(name)) {
            return new MethodValue(type, nullptr, name);
        }
    }
    
    return nullptr;
}
```

**问题分析：**
- 单个方法承担了太多职责
- 嵌套的for循环使代码难以理解
- 逻辑分散，难以单独测试
- 代码重复（作用域遍历逻辑）

## 重构后的代码结构

### 1. 职责细分后的私有辅助方法

```cpp
// ==================== 私有辅助方法 ====================

// 职责：在作用域中直接查找可调用对象
Value* ScopeManager::findCallableInScope(const string& name) {
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        Scope* scope = *it;
        if (scope && scope->objectRegistry) {
            Value* callable = scope->objectRegistry->lookupCallable(name);
            if (callable && callable->isCallable()) {
                LOG_DEBUG("Found function by direct lookup: " + name);
                return callable;
            }
        }
    }
    return nullptr;
}

// 职责：通过函数名在作用域中查找可调用对象
Value* ScopeManager::findCallableByName(const string& name) {
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        Scope* scope = *it;
        if (scope && scope->objectRegistry) {
            Value* callable = findCallableByNameInScope(scope, name);
            if (callable) {
                return callable;
            }
        }
    }
    return nullptr;
}

// 职责：在单个作用域中通过函数名查找可调用对象
Value* ScopeManager::findCallableByNameInScope(Scope* scope, const string& name) {
    if (!scope || !scope->objectRegistry) {
        return nullptr;
    }
    
    const auto& callables = scope->objectRegistry->getCallableObjects();
    for (const auto& pair : callables) {
        Value* callable = pair.second;
        if (callable && callable->isCallable()) {
            if (Function* func = dynamic_cast<Function*>(callable)) {
                if (func->getName() == name) {
                    LOG_DEBUG("Found function by name lookup: " + name);
                    return callable;
                }
            }
        }
    }
    return nullptr;
}

// 职责：在类型系统中查找构造函数或静态方法
Value* ScopeManager::findTypeConstructor(const string& name) {
    ObjectType* type = TypeRegistry::getGlobalInstance()->getType(name);
    if (!type) {
        return nullptr;
    }
    
    if (type->hasStaticMethodName(name)) {
        LOG_DEBUG("Found constructor: " + name);
        return new ClassMethodValue(type, name);
    }
    
    if (type->hasMethodName(name)) {
        LOG_DEBUG("Found instance method: " + name);
        return new MethodValue(type, nullptr, name);
    }
    
    return nullptr;
}

// 职责：在作用域中查找实例对象
Value* ScopeManager::findInstanceInScopes(const string& name) {
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        Scope* scope = *it;
        if (scope && scope->objectRegistry) {
            Value* instance = scope->objectRegistry->lookupInstance(name);
            if (instance) {
                return instance;
            }
        }
    }
    return nullptr;
}
```

### 2. 重构后的主要查询方法

```cpp
Value* ScopeManager::lookupFunction(const string& name) {
    LOG_DEBUG("Looking for function: " + name);
    
    // 1. 首先尝试直接查找
    Value* callable = findCallableInScope(name);
    if (callable) {
        return callable;
    }
    
    // 2. 尝试通过函数名查找
    callable = findCallableByName(name);
    if (callable) {
        return callable;
    }
    
    // 3. 尝试在类型系统中查找同名类型（构造函数）
    callable = findTypeConstructor(name);
    if (callable) {
        return callable;
    }
    
    LOG_DEBUG("Function not found: " + name);
    return nullptr;
}
```

## 重构优势

### 1. **职责单一原则**
- 每个函数只负责一个特定的查找逻辑
- 函数名称清晰表达了其功能
- 降低了函数的复杂度

### 2. **代码复用**
- `findInstanceInScopes` 被多个方法使用
- 作用域遍历逻辑被封装，避免重复
- 提高了代码的DRY原则

### 3. **易于测试**
- 每个辅助方法可以独立测试
- 测试用例更加精确和聚焦
- 便于单元测试的编写

### 4. **易于维护**
- 修改某个查找逻辑时，只需要修改对应的辅助方法
- 代码结构清晰，便于理解
- 降低了修改的风险

### 5. **易于扩展**
- 添加新的查找策略时，只需要添加新的辅助方法
- 主方法保持简洁，符合开闭原则
- 便于后续功能扩展

## 函数职责详细说明

| 函数名 | 职责 | 输入 | 输出 | 使用场景 |
|--------|------|------|------|----------|
| `findCallableInScope` | 在作用域中直接查找可调用对象 | 标识符名称 | 可调用对象或nullptr | 快速查找已注册的函数 |
| `findCallableByName` | 通过函数名在作用域中查找可调用对象 | 标识符名称 | 可调用对象或nullptr | 查找同名函数 |
| `findCallableByNameInScope` | 在单个作用域中通过函数名查找 | 作用域指针, 标识符名称 | 可调用对象或nullptr | 在特定作用域中查找函数 |
| `findTypeConstructor` | 在类型系统中查找构造函数或静态方法 | 标识符名称 | 方法值或nullptr | 查找类型构造函数 |
| `findInstanceInScopes` | 在作用域中查找实例对象 | 标识符名称 | 实例对象或nullptr | 查找对象实例 |

## 重构后的代码流程

```
lookupFunction(name)
    ├── findCallableInScope(name)     // 直接查找
    ├── findCallableByName(name)      // 按名称查找
    │   └── findCallableByNameInScope(scope, name)  // 在特定作用域中查找
    └── findTypeConstructor(name)     // 查找构造函数
```

## 总结

通过这次重构，我们实现了：

1. **职责细分**：每个函数都有明确的单一职责
2. **代码复用**：消除了重复的作用域遍历逻辑
3. **可读性提升**：主方法逻辑清晰，易于理解
4. **可维护性增强**：修改某个查找逻辑时影响范围小
5. **可测试性改善**：每个辅助方法都可以独立测试

这种重构方式符合软件工程的最佳实践，使代码更加健壮和易于维护。
