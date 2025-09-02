# 方法引用架构重新设计

## 设计目标

重新设计方法引用的架构，实现"父类执行call，子类查找对应匹配函数"的设计模式。

## 架构设计

### 1. 基类 MethodReference

**职责：**
- 提供统一的函数调用接口 `call(Scope* scope)`
- 负责从Scope中获取参数并计算函数签名 `createSignatureFromArgs(args)`
- 调用子类的 `findBestMatch` 方法查找匹配函数
- 管理函数缓存机制

**关键方法：**
```cpp
// 父类实现统一的调用逻辑
Value* call(Scope* scope) {
    if (cachedFunction) {
        return cachedFunction->call(scope);
    }
    
    // 父类负责从scope计算函数签名，然后调用子类的findBestMatch方法查找函数
    FunctionSignature callSignature = createSignatureFromArgs(scope);
    Function* foundFunction = findBestMatch(callSignature);
    
    // 如果找到了函数，直接设置缓存并调用
    if (foundFunction) {
        cachedFunction = foundFunction;
        return foundFunction->call(scope);
    }
    
    return nullptr;
}

// 抽象方法：子类必须实现，负责根据函数签名查找最佳匹配的方法
virtual Function* findBestMatch(const FunctionSignature& callSignature) const = 0;

// 创建函数签名的辅助方法，支持可变参数
FunctionSignature createSignatureFromArgs(const Scope* scope) const;

// 实现Function基类的纯虚函数
FunctionSignature getSignature() const override;

// 获取基于实际调用参数的准确签名（在call时使用）
FunctionSignature getActualSignature(const Scope* scope) const;
```

### 可变参数支持

`FunctionSignature` 类现在完全支持可变参数函数：

1. **可变参数检测**：`hasVarArgs` 和 `varArgsType` 成员变量
2. **参数匹配**：`matches()` 方法正确处理可变参数的数量和类型检查
3. **匹配分数计算**：`calculateMatchScore()` 方法为可变参数函数提供合适的评分
4. **专用方法**：
   - `canAcceptVarArgs()` - 专门检查可变参数匹配
   - `getFixedParameterCount()` - 获取固定参数数量
   - `getVarArgsStartIndex()` - 获取可变参数开始位置
   - `isVarArgsParameter(index)` - 检查指定位置是否为可变参数
```

### 2. 子类 InstanceMethodReference

**职责：**
- 专门处理实例方法的查找
- 实现 `findBestMatch` 方法，根据函数签名查找最佳匹配的实例方法
- **不需要知道scope的存在，只负责根据签名查找函数**

**关键方法：**
```cpp
Function* findBestMatch(const FunctionSignature& callSignature) const override {
    if (!targetType || !targetType->supportsMethods()) {
        return nullptr;
    }
    
    // 直接根据调用签名查找匹配的实例方法
    IMethodSupport* methodSupport = dynamic_cast<IMethodSupport*>(targetType);
    if (methodSupport) {
        return methodSupport->findUserMethod(methodName, callSignature);
    }
    
    return nullptr;
}
```

### 3. 子类 StaticMethodReference

**职责：**
- 专门处理静态方法的查找
- 实现 `findBestMatch` 方法，根据函数签名查找最佳匹配的静态方法
- **不需要知道scope的存在，只负责根据签名查找函数**

## 设计优势

### 1. 职责分离
- **父类**：负责统一的调用流程、参数获取、函数签名计算、缓存管理
- **子类**：专注于**方法查找逻辑**，不需要知道scope的存在，只负责根据签名查找函数

### 2. 代码复用
- 参数获取和函数签名计算逻辑只在父类中实现一次
- 缓存机制由父类统一管理，直接操作成员变量
- **可变参数支持逻辑完全集成在FunctionSignature中**
- **函数签名生成逻辑复用**：`getActualSignature` 复用 `createSignatureFromArgs` 的逻辑
- 子类可以专注于自己的核心职责（**方法查找**）

### 3. 扩展性
- 新增方法引用类型只需要继承MethodReference并实现findBestMatch
- 父类的调用逻辑对所有子类都适用
- **完全实现Function接口**：实现了所有必需的虚函数，可以正常实例化

### 4. 维护性
- 修改调用逻辑、参数处理、函数签名计算只需要修改父类
- 修改**方法查找逻辑**只需要修改对应的子类
- 降低了代码耦合度，子类完全不知道scope的存在
- **代码更加简洁，子类只负责查找，无需管理签名缓存**

## 使用示例

```cpp
// 创建实例方法引用
MethodReference* methodRef = new InstanceMethodReference(type, instance, "methodName");

// 调用方法 - 父类负责获取参数，子类负责查找匹配函数
Value* result = methodRef->call(scope);

// 创建静态方法引用
MethodReference* staticRef = new StaticMethodReference(type, "staticMethodName");

// 调用静态方法
Value* result = staticRef->call(scope);
```

## 总结

这种设计模式实现了：
1. **父类执行call**：统一的调用流程、参数获取和函数签名计算
2. **子类查找对应匹配函数**：专门的方法查找逻辑，基于函数签名匹配
3. **清晰的职责分离**：每个类都有明确的职责，子类不需要知道scope
4. **良好的扩展性**：易于添加新的方法引用类型
5. **代码复用**：避免重复的参数获取、函数签名计算和缓存逻辑
6. **低耦合**：子类完全不知道scope的存在，只处理函数签名匹配
7. **简洁性**：子类无需管理签名缓存，直接根据签名查找函数
8. **可变参数支持**：FunctionSignature完全支持可变参数函数的匹配和评分
9. **接口完整性**：完全实现Function接口，可以正常实例化和使用
