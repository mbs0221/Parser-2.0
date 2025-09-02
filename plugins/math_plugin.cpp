#include "interpreter/plugins/builtin_plugin.h"
#include "interpreter/values/value.h"
#include "interpreter/types/types.h"
#include "parser/definition.h"
#include <iostream>
#include <cmath>
#include <vector>

using namespace std;

// ==================== 数学函数实现 - 使用宏自动生成 ====================

// 使用宏自动生成数学函数实现
AUTO_PLUGIN_MATH_FUNCTION(math_sin, "sin")
AUTO_PLUGIN_MATH_FUNCTION(math_cos, "cos")
AUTO_PLUGIN_MATH_FUNCTION(math_tan, "tan")
AUTO_PLUGIN_MATH_FUNCTION(math_sqrt, "sqrt")
AUTO_PLUGIN_MATH_FUNCTION(math_log, "log")
AUTO_PLUGIN_MATH_FUNCTION(math_floor, "floor")
AUTO_PLUGIN_MATH_FUNCTION(math_ceil, "ceil")
AUTO_PLUGIN_MATH_FUNCTION(math_round, "round")
AUTO_PLUGIN_MATH_FUNCTION(math_abs, "abs")

// ==================== 数学函数插件类 ====================
class MathPlugin : public BuiltinPlugin {
public:
    PluginInfo getPluginInfo() const override {
        return PluginInfo{
            "math_plugin",
            "1.0.0",
            "基础数学函数插件，通过类型系统调用数学运算方法",
            {"sin", "cos", "tan", "sqrt", "log", "floor", "ceil", "round", "abs"}
        };
    }
    
protected:
    map<string, FunctionInfo> getFunctionInfoMap() const override {
        return {
            {"sin", {math_sin, {"value"}, "计算正弦值"}},
            {"cos", {math_cos, {"value"}, "计算余弦值"}},
            {"tan", {math_tan, {"value"}, "计算正切值"}},
            {"sqrt", {math_sqrt, {"value"}, "计算平方根"}},
            {"log", {math_log, {"value"}, "计算自然对数"}},
            {"floor", {math_floor, {"value"}, "向下取整"}},
            {"ceil", {math_ceil, {"value"}, "向上取整"}},
            {"round", {math_round, {"value"}, "四舍五入"}},
            {"abs", {math_abs, {"value"}, "计算绝对值"}}
        };
    }
};

// 导出插件
EXPORT_PLUGIN(MathPlugin)
