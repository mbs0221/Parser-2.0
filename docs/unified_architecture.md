# ScopeManager 统一架构设计

## 当前架构问题

虽然我们已经重构了函数职责，但当前的架构仍然存在一些不一致性：

1. **分散的存储容器**：变量、函数、实例分别存储在不同的容器中
2. **重复的查找逻辑**：每个类型都需要单独的作用域遍历
3. **类型检查分散**：类型信息分散在多个地方
4. **扩展性差**：添加新的标识符类型需要修改多个地方

## 统一架构解决方案

### 1. 统一标识符存储

```cpp
// 统一的标识符结构
struct Identifier {
    std::string name;           // 标识符名称
    Value* value;               // 值对象
    IdentifierType type;        // 标识符类型
    ObjectType* valueType;      // 值类型
    Scope* definedScope;        // 定义作用域
    bool isConst;               // 是否为常量
    
    Identifier(const std::string& n, Value* v, IdentifierType t, ObjectType* vt = nullptr)
        : name(n), value(v), type(t), valueType(vt), definedScope(nullptr), isConst(false) {}
};

// 标识符类型枚举
enum class IdentifierType {
    VARIABLE,       // 变量
    FUNCTION,       // 函数
    METHOD,         // 方法
    INSTANCE,       // 实例
    TYPE,           // 类型
    CONSTANT        // 常量
};

// 统一的标识符注册表
class UnifiedIdentifierRegistry {
private:
    // 统一的标识符存储：名称 -> 标识符对象
    std::map<std::string, std::unique_ptr<Identifier>> identifiers;
    
    // 类型索引：类型 -> 标识符列表
    std::map<IdentifierType, std::vector<std::string>> typeIndex;
    
    // 作用域索引：作用域 -> 标识符列表
    std::map<Scope*, std::vector<std::string>> scopeIndex;

public:
    // 统一的标识符管理接口
    void defineIdentifier(const std::string& name, Value* value, IdentifierType type, 
                         ObjectType* valueType = nullptr, Scope* scope = nullptr);
    
    Identifier* lookupIdentifier(const std::string& name) const;
    
    // 类型化查找
    std::vector<Identifier*> findIdentifiersByType(IdentifierType type) const;
    
    // 作用域查找
    std::vector<Identifier*> findIdentifiersByScope(Scope* scope) const;
    
    // 批量查找
    std::vector<Identifier*> findIdentifiers(const std::string& name) const;
    
    // 存在性检查
    bool hasIdentifier(const std::string& name) const;
    bool hasIdentifierOfType(const std::string& name, IdentifierType type) const;
    
    // 资源管理
    void cleanup();
    void removeIdentifiersByScope(Scope* scope);
};
```

### 2. 重构后的ScopeManager

```cpp
class ScopeManager {
private:
    std::vector<Scope*> scopes;
    Scope* currentScope;
    Scope* globalScope;
    
    // 统一的标识符注册表
    std::unique_ptr<UnifiedIdentifierRegistry> identifierRegistry;

public:
    // ==================== 统一查询接口 ====================
    
    // 主要查询方法 - 按优先级查找标识符
    Value* lookup(const std::string& name);
    
    // 类型化查询方法
    Value* lookupVariable(const std::string& name);
    Value* lookupFunction(const std::string& name);
    Value* lookupMethod(const std::string& name);
    Value* lookupInstance(const std::string& name);
    ObjectType* lookupType(const std::string& name);
    
    // 批量查询方法
    struct LookupResult {
        std::vector<Identifier*> allMatches;
        Identifier* bestMatch;  // 按优先级选择的最佳匹配
        
        bool hasAny() const { return !allMatches.empty(); }
        std::string toString() const;
    };
    
    LookupResult lookupAll(const std::string& name);
    
    // ==================== 统一定义接口 ====================
    
    // 统一的标识符定义
    void defineIdentifier(const std::string& name, Value* value, 
                         const std::string& category = "auto");
    
    // 便捷方法
    void defineVariable(const std::string& name, Value* value);
    void defineFunction(const std::string& name, Value* function);
    void defineMethod(const std::string& name, Value* method);
    void defineInstance(const std::string& name, Value* instance);
    void defineType(const std::string& name, ObjectType* type);
    
    // ==================== 统一检查接口 ====================
    
    bool has(const std::string& name) const;
    bool hasVariable(const std::string& name) const;
    bool hasFunction(const std::string& name) const;
    bool hasMethod(const std::string& name) const;
    bool hasInstance(const std::string& name) const;
    bool hasType(const std::string& name) const;
    
    // ==================== 作用域管理 ====================
    
    void enterScope();
    void exitScope();
    
    // 作用域信息
    Scope* getCurrentScope() const;
    Scope* getGlobalScope() const;
    size_t getScopeDepth() const;
    
    // 上下文管理
    void setThisPointer(Value* thisPtr);
    Value* getThisPointer() const;
    void clearThisPointer();
    
    void setCurrentClassContext(ClassType* classType);
    ClassType* getCurrentClassContext() const;
    void clearCurrentClassContext();
    
    // ==================== 调试接口 ====================
    
    void printCurrentScope() const;
    void printAllScopes() const;
};
```

### 3. 统一查询策略

```cpp
// 统一的查询策略
class QueryStrategy {
public:
    // 查询优先级配置
    struct PriorityConfig {
        std::vector<IdentifierType> priorityOrder;
        bool allowAmbiguousMatches;
        bool searchInParentScopes;
    };
    
    // 执行查询
    static Identifier* executeQuery(const std::string& name, 
                                  const UnifiedIdentifierRegistry& registry,
                                  const PriorityConfig& config);
    
    // 模糊查询
    static std::vector<Identifier*> fuzzyQuery(const std::string& name,
                                             const UnifiedIdentifierRegistry& registry);
    
    // 类型推断查询
    static Identifier* inferTypeQuery(const std::string& name,
                                    const UnifiedIdentifierRegistry& registry,
                                    const std::vector<ObjectType*>& contextTypes);
};

// 在ScopeManager中的使用
Value* ScopeManager::lookup(const std::string& name) {
    QueryStrategy::PriorityConfig config;
    config.priorityOrder = {
        IdentifierType::VARIABLE,    // 变量优先级最高
        IdentifierType::FUNCTION,    // 函数次之
        IdentifierType::METHOD,      // 方法再次之
        IdentifierType::INSTANCE,    // 实例
        IdentifierType::TYPE         // 类型优先级最低
    };
    config.allowAmbiguousMatches = false;
    config.searchInParentScopes = true;
    
    Identifier* result = QueryStrategy::executeQuery(name, *identifierRegistry, config);
    return result ? result->value : nullptr;
}
```

### 4. 智能类型推断

```cpp
// 智能类型推断系统
class TypeInference {
public:
    // 根据上下文推断标识符类型
    static IdentifierType inferIdentifierType(const std::string& name, 
                                           Value* value,
                                           const std::vector<ObjectType*>& contextTypes);
    
    // 根据使用模式推断类型
    static IdentifierType inferTypeFromUsage(const std::string& name,
                                           const std::vector<UsagePattern>& patterns);
    
    // 类型冲突检测
    static bool hasTypeConflict(const std::string& name,
                              IdentifierType proposedType,
                              const UnifiedIdentifierRegistry& registry);
};

// 使用示例
void ScopeManager::defineIdentifier(const std::string& name, Value* value, 
                                   const std::string& category) {
    // 自动推断类型
    std::vector<ObjectType*> contextTypes = getCurrentContextTypes();
    IdentifierType inferredType = TypeInference::inferIdentifierType(name, value, contextTypes);
    
    // 检查类型冲突
    if (TypeInference::hasTypeConflict(name, inferredType, *identifierRegistry)) {
        LOG_WARNING("Type conflict detected for identifier: " + name);
    }
    
    // 注册标识符
    identifierRegistry->defineIdentifier(name, value, inferredType, 
                                       value->getValueType(), currentScope);
}
```

## 统一架构的优势

### 1. **架构一致性**
- 所有标识符使用统一的存储和查询机制
- 消除了分散的容器和重复的逻辑
- 统一的接口设计，易于理解和使用

### 2. **性能提升**
- 减少了重复的作用域遍历
- 统一的索引结构，查询效率更高
- 批量操作支持，减少多次查询

### 3. **扩展性增强**
- 添加新的标识符类型只需要扩展枚举
- 查询策略可以灵活配置
- 支持复杂的查询需求（模糊查询、类型推断等）

### 4. **维护性改善**
- 代码结构更加清晰
- 修改查询逻辑只需要修改一个地方
- 统一的错误处理和日志记录

### 5. **功能增强**
- 支持标识符的元数据管理
- 更好的作用域追踪
- 支持复杂的类型推断和冲突检测

## 迁移策略

### 阶段1：保持兼容性
- 保留现有的接口
- 在内部使用新的统一架构
- 逐步迁移现有代码

### 阶段2：接口统一
- 引入新的统一接口
- 标记旧接口为过时
- 提供迁移指南

### 阶段3：完全迁移
- 移除旧的接口
- 优化性能
- 添加新功能

## 总结

通过统一架构，我们可以：

1. **消除重复代码** - 所有标识符使用统一的存储和查询机制
2. **提高性能** - 减少重复的作用域遍历，统一的索引结构
3. **增强扩展性** - 易于添加新的标识符类型和查询策略
4. **改善维护性** - 代码结构更清晰，修改影响范围更小
5. **增强功能** - 支持更复杂的查询需求和类型推断

这种统一架构设计符合软件工程的最佳实践，使系统更加健壮、高效和易于维护。
