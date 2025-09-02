#include "interpreter/plugins/builtin_plugin.h"
#include "interpreter/values/value.h"
#include "interpreter/types/types.h"
#include "parser/definition.h"
#include <iostream>
#include <algorithm>

using namespace std;

// ==================== 数组操作函数 - 使用宏自动生成 ====================

// 使用宏自动生成数组函数实现
AUTO_PLUGIN_ARRAY_FUNCTION(array_push, "push")
AUTO_PLUGIN_ARRAY_FUNCTION(array_pop, "pop")
AUTO_PLUGIN_ARRAY_FUNCTION(array_sort, "sort")
AUTO_PLUGIN_FUNCTION_0(array_length, "length")

// 数组操作插件类
class ArrayPlugin : public BuiltinPlugin {
public:
    PluginInfo getPluginInfo() const override {
        return PluginInfo{
            "array",
            "1.0.0",
            "数组操作插件，通过类型系统调用数组操作方法",
            {"push", "pop", "sort", "length"}
        };
    }
    
protected:
    map<string, FunctionInfo> getFunctionInfoMap() const override {
        return {
            {"push", {array_push, {"array", "value"}, "向数组末尾添加元素"}},
            {"pop", {array_pop, {"array"}, "从数组末尾移除并返回元素"}},
            {"sort", {array_sort, {"array"}, "对数组进行排序"}},
            {"length", {array_length, {"array"}, "获取数组长度"}}
        };
    }
};

// 导出插件
EXPORT_PLUGIN(ArrayPlugin)
