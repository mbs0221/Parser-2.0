# 插件开发指南 - 简化方法

## 概述

为了简化插件开发，我们提供了多种方法来减少重复代码，让插件开发更加简洁和高效。

## 简化方法

### 1. 使用辅助方法（推荐）

`BuiltinPlugin`基类提供了两个辅助方法：

```cpp
// 注册单个函数
void defineBuiltinFunction(ScopeManager& scopeManager, const string& name, BuiltinFunctionPtr func);

// 批量注册函数
void defineBuiltinFunctions(ScopeManager& scopeManager, const map<string, BuiltinFunctionPtr>& functions);
```

#### 示例用法：

```cpp
class MyPlugin : public BuiltinPlugin {
public:
    void registerFunctions(ScopeManager& scopeManager) override {
        // 方法1：使用批量注册（推荐）
        defineBuiltinFunctions(scopeManager, getFunctionMap());
        
        // 方法2：单独注册
        defineBuiltinFunction(scopeManager, "my_func", my_function);
    }
    
    map<string, BuiltinFunctionPtr> getFunctionMap() const override {
        return {
            {"func1", function1},
            {"func2", function2},
            {"func3", function3}
        };
    }
};
```

### 2. 使用宏简化

我们还提供了宏来进一步简化：

```cpp
// 注册单个函数
REGISTER_BUILTIN_FUNCTION(scopeManager, "func_name", func_ptr);

// 批量注册函数（使用map）
REGISTER_BUILTIN_FUNCTIONS(scopeManager, 
    {"func1", function1},
    {"func2", function2},
    {"func3", function3}
);
```

### 3. 完整的插件示例

```cpp
#include "interpreter/builtin_plugin.h"
#include <iostream>

using namespace std;

// 函数实现
Value* my_function(vector<Variable*>& args) {
    // 函数实现
    return new String("Hello from my plugin!");
}

// 插件类
class MyPlugin : public BuiltinPlugin {
public:
    PluginInfo getPluginInfo() const override {
        return PluginInfo{
            "my_plugin",
            "1.0.0",
            "我的自定义插件",
            {"my_function"}
        };
    }
    
    void registerFunctions(ScopeManager& scopeManager) override {
        // 使用辅助方法批量注册
        defineBuiltinFunctions(scopeManager, getFunctionMap());
    }
    
    map<string, BuiltinFunctionPtr> getFunctionMap() const override {
        return {
            {"my_function", my_function}
        };
    }
};

// 导出插件
EXPORT_PLUGIN(MyPlugin)
```

## 优势对比

### 原始方法（冗长）：
```cpp
void registerFunctions(ScopeManager& scopeManager) override {
    scopeManager.defineIdentifier("func1", new BuiltinFunction("func1", function1));
    scopeManager.defineIdentifier("func2", new BuiltinFunction("func2", function2));
    scopeManager.defineIdentifier("func3", new BuiltinFunction("func3", function3));
    // ... 更多函数
}
```

### 简化方法（简洁）：
```cpp
void registerFunctions(ScopeManager& scopeManager) override {
    defineBuiltinFunctions(scopeManager, getFunctionMap());
}
```

## 最佳实践

1. **使用辅助方法**：优先使用`defineBuiltinFunctions`进行批量注册
2. **保持一致性**：在`getFunctionMap()`中定义所有函数映射
3. **使用宏**：对于简单的单函数注册，可以使用`REGISTER_BUILTIN_FUNCTION`宏
4. **代码复用**：将函数映射和注册逻辑分离，便于维护

## 构建和测试

```bash
# 构建简化插件
make math_plugin_simplified

# 运行测试
./bin/test_plugin_system
```

通过这些简化方法，插件开发变得更加简洁和高效！
