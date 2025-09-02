# ScopeManager 重构说明

## 重构目标

重构前的 ScopeManager 存在以下问题：
1. **接口重复和不一致**：有多个相似的查找方法
2. **查询逻辑分散**：查找逻辑分散在多个方法中
3. **调试信息过多**：代码中有大量的调试输出
4. **方法职责不清晰**：一些方法既做查找又做类型检查

## 新的统一查询接口

### 1. 主要查询方法

```cpp
// 统一查询方法 - 按优先级查找标识符
Value* lookup(const string& name);
```

这个方法按照以下优先级查找标识符：
1. 变量（最高优先级）
2. 函数
3. 方法
4. 实例
5. 类型名称

### 2. 类型化查询方法

```cpp
// 明确指定要查找的类型
ObjectType* lookupType(const string& name);
Value* lookupVariable(const string& name);
Value* lookupFunction(const string& name);
Value* lookupMethod(const string& name);
```

### 3. 批量查询方法

```cpp
// 一次性获取所有匹配的标识符
struct LookupResult {
    ObjectType* type = nullptr;
    Value* variable = nullptr;
    Value* function = nullptr;
    Value* method = nullptr;
    Value* instance = nullptr;
    
    bool hasAny() const;
    string toString() const;
};

LookupResult lookupAll(const string& name);
```

## 使用示例

### 基本查询

```cpp
ScopeManager scopeManager;

// 定义一些标识符
scopeManager.defineVariable("x", new Integer(42));
scopeManager.defineFunction("add", new BuiltinFunction("add"));

// 使用统一查询接口
Value* result = scopeManager.lookup("x");        // 返回变量 x
Value* func = scopeManager.lookup("add");        // 返回函数 add
Value* unknown = scopeManager.lookup("unknown"); // 返回 nullptr
```

### 类型化查询

```cpp
// 明确查找变量
Value* var = scopeManager.lookupVariable("x");
if (var) {
    cout << "Found variable: " << var->toString() << endl;
}

// 明确查找函数
Value* func = scopeManager.lookupFunction("add");
if (func) {
    cout << "Found function: " << func->toString() << endl;
}

// 明确查找类型
ObjectType* type = scopeManager.lookupType("Array");
if (type) {
    cout << "Found type: " << type->getTypeName() << endl;
}
```

### 批量查询

```cpp
// 获取所有匹配的标识符
auto result = scopeManager.lookupAll("name");

if (result.hasAny()) {
    cout << "Found identifiers: " << result.toString() << endl;
    
    if (result.variable) {
        cout << "Variable: " << result.variable->toString() << endl;
    }
    
    if (result.function) {
        cout << "Function: " << result.function->toString() << endl;
    }
    
    if (result.type) {
        cout << "Type: " << result.type->getTypeName() << endl;
    }
}
```

### 检查标识符存在性

```cpp
// 检查是否存在任何类型的标识符
if (scopeManager.has("x")) {
    cout << "Identifier 'x' exists" << endl;
}

// 检查特定类型的标识符
if (scopeManager.hasVariable("x")) {
    cout << "Variable 'x' exists" << endl;
}

if (scopeManager.hasFunction("add")) {
    cout << "Function 'add' exists" << endl;
}

if (scopeManager.hasType("Array")) {
    cout << "Type 'Array' exists" << endl;
}
```

## 定义接口

### 变量定义

```cpp
// 基本变量定义
scopeManager.defineVariable("x", new Integer(42));

// 带类型信息的变量定义
scopeManager.defineVariable("y", "int", new Integer(100));

// 带ObjectType的变量定义
ObjectType* intType = TypeRegistry::getGlobalInstance()->getType("int");
scopeManager.defineVariable("z", intType, new Integer(200));
```

### 函数定义

```cpp
// 基本函数定义
scopeManager.defineFunction("add", new BuiltinFunction("add"));

// 带参数类型的函数定义
vector<string> paramTypes = {"int", "int"};
scopeManager.defineFunction("multiply", paramTypes, new BuiltinFunction("multiply"));
```

### 类型和方法定义

```cpp
// 类型定义
scopeManager.defineType("MyClass", new ClassType("MyClass"));

// 方法定义
scopeManager.defineMethod("myMethod", new MethodValue(...));

// 实例定义
scopeManager.defineInstance("obj", new ObjectValue(...));
```

## 兼容性

为了保持向后兼容性，旧的接口仍然保留但标记为过时：

```cpp
// @deprecated 使用 lookup() 替代
Value* lookupAny(const string& name);

// @deprecated 使用 lookupAll() 替代
tuple<ObjectType*, Value*, Value*, Value*> lookupIdentifier(const string& name);

// @deprecated 使用 defineFunction() 替代
void defineCallable(const string& name, Value* callable);
```

## 优势

1. **接口统一**：所有查询操作都通过统一的接口进行
2. **逻辑清晰**：查询逻辑集中在少数几个方法中
3. **易于使用**：开发者可以根据需要选择合适的方法
4. **性能优化**：减少了重复的查询逻辑
5. **易于维护**：代码结构更加清晰，便于后续维护和扩展

## 迁移指南

### 从旧接口迁移到新接口

```cpp
// 旧代码
Value* result = scopeManager.lookupAny("name");

// 新代码
Value* result = scopeManager.lookup("name");
```

```cpp
// 旧代码
auto [type, var, func, inst] = scopeManager.lookupIdentifier("name");

// 新代码
auto result = scopeManager.lookupAll("name");
if (result.type) { /* 使用 result.type */ }
if (result.variable) { /* 使用 result.variable */ }
```

```cpp
// 旧代码
scopeManager.defineCallable("func", function);

// 新代码
scopeManager.defineFunction("func", function);
```
