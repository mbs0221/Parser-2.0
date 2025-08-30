#include "interpreter/builtin_plugin.h" 
#include "interpreter/value.h"
#include "parser/function.h"
#include "interpreter/scope.h"
#include "interpreter/interpreter.h"
#include <iostream>
#include <cmath>
#include <vector>

using namespace std;

// 数学函数实现
Value* math_sin(vector<Value*>& args) {
    if (args.size() != 1 || !args[0]) return nullptr;
    
    Value* val = args[0];
    if (Integer* intVal = dynamic_cast<Integer*>(val)) {
        return new Double(sin(intVal->getValue()));
    } else if (Double* doubleVal = dynamic_cast<Double*>(val)) {
        return new Double(sin(doubleVal->getValue()));
    }
    return nullptr;
}

Value* math_cos(vector<Value*>& args) {
    if (args.size() != 1 || !args[0]) return nullptr;
    
    Value* val = args[0];
    if (Integer* intVal = dynamic_cast<Integer*>(val)) {
        return new Double(cos(intVal->getValue()));
    } else if (Double* doubleVal = dynamic_cast<Double*>(val)) {
        return new Double(cos(doubleVal->getValue()));
    }
    return nullptr;
}

Value* math_tan(vector<Variable*>& args) {
    if (args.size() != 1 || !args[0]) return nullptr;
    
    Value* val = args[0]->getValue();
    if (Integer* intVal = dynamic_cast<Integer*>(val)) {
        return new Double(tan(intVal->getValue()));
    } else if (Double* doubleVal = dynamic_cast<Double*>(val)) {
        return new Double(tan(doubleVal->getValue()));
    }
    return nullptr;
}

Value* math_sqrt(vector<Variable*>& args) {
    if (args.size() != 1 || !args[0]) return nullptr;
    
    Value* val = args[0]->getValue();
    if (Integer* intVal = dynamic_cast<Integer*>(val)) {
        if (intVal->getValue() < 0) return nullptr;
        return new Double(sqrt(intVal->getValue()));
    } else if (Double* doubleVal = dynamic_cast<Double*>(val)) {
        if (doubleVal->getValue() < 0) return nullptr;
        return new Double(sqrt(doubleVal->getValue()));
    }
    return nullptr;
}

Value* math_log(vector<Variable*>& args) {
    if (args.size() != 1 || !args[0]) return nullptr;
    
    Value* val = args[0]->getValue();
    if (Integer* intVal = dynamic_cast<Integer*>(val)) {
        if (intVal->getValue() <= 0) return nullptr;
        return new Double(log(intVal->getValue()));
    } else if (Double* doubleVal = dynamic_cast<Double*>(val)) {
        if (doubleVal->getValue() <= 0) return nullptr;
        return new Double(log(doubleVal->getValue()));
    }
    return nullptr;
}

Value* math_floor(vector<Variable*>& args) {
    if (args.size() != 1 || !args[0]) return nullptr;
    
    Value* val = args[0]->getValue();
    if (Integer* intVal = dynamic_cast<Integer*>(val)) {
        return new Integer(intVal->getValue());
    } else if (Double* doubleVal = dynamic_cast<Double*>(val)) {
        return new Integer((int)floor(doubleVal->getValue()));
    }
    return nullptr;
}

Value* math_ceil(vector<Variable*>& args) {
    if (args.size() != 1 || !args[0]) return nullptr;
    
    Value* val = args[0]->getValue();
    if (Integer* intVal = dynamic_cast<Integer*>(val)) {
        return new Integer(intVal->getValue());
    } else if (Double* doubleVal = dynamic_cast<Double*>(val)) {
        return new Integer((int)ceil(doubleVal->getValue()));
    }
    return nullptr;
}

// 数学函数插件类
class MathPlugin : public BuiltinPlugin {
public:
    PluginInfo getPluginInfo() const override {
        return PluginInfo{
            "math_plugin",
            "1.0.0",
            "数学函数插件，包含三角函数、对数函数等高级数学运算",
            {"sin", "cos", "tan", "sqrt", "log", "floor", "ceil"}
        };
    }
    
    void registerFunctions(ScopeManager& scopeManager) override {
        // 使用辅助方法批量注册函数
        defineBuiltinFunctions(scopeManager, getFunctionMap());
    }
    
    map<string, BuiltinFunctionPtr> getFunctionMap() const override {
        return {
            {"sin", math_sin},
            {"cos", math_cos},
            {"tan", math_tan},
            {"sqrt", math_sqrt},
            {"log", math_log},
            {"floor", math_floor},
            {"ceil", math_ceil}
        };
    }
};

// 导出插件
EXPORT_PLUGIN(MathPlugin)
