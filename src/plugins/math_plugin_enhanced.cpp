#include "interpreter/builtin_plugin.h"
#include "lexer/value.h"
#include "parser/function.h"

#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>

using namespace std;

// ==================== 基础数学函数 ====================

Value* math_abs(vector<Variable*>& args) {
    if (args.size() != 1 || !args[0]) return nullptr;
    
    Value* val = args[0]->getValue();
    if (Integer* intVal = dynamic_cast<Integer*>(val)) {
        return new Integer(abs(intVal->getValue()));
    } else if (Double* doubleVal = dynamic_cast<Double*>(val)) {
        return new Double(fabs(doubleVal->getValue()));
    }
    return nullptr;
}

Value* math_max(vector<Variable*>& args) {
    if (args.empty()) return nullptr;
    
    Value* maxVal = args[0]->getValue();
    if (!maxVal) return nullptr;
    
    for (size_t i = 1; i < args.size(); ++i) {
        if (!args[i]) continue;
        Value* val = args[i]->getValue();
        if (!val) continue;
        
        if (Integer* int1 = dynamic_cast<Integer*>(maxVal)) {
            if (Integer* int2 = dynamic_cast<Integer*>(val)) {
                if (int2->getValue() > int1->getValue()) {
                    maxVal = int2;
                }
            } else if (Double* double2 = dynamic_cast<Double*>(val)) {
                if (double2->getValue() > int1->getValue()) {
                    maxVal = double2;
                }
            }
        } else if (Double* double1 = dynamic_cast<Double*>(maxVal)) {
            if (Integer* int2 = dynamic_cast<Integer*>(val)) {
                if (int2->getValue() > double1->getValue()) {
                    maxVal = int2;
                }
            } else if (Double* double2 = dynamic_cast<Double*>(val)) {
                if (double2->getValue() > double1->getValue()) {
                    maxVal = double2;
                }
            }
        }
    }
    
    return maxVal;
}

Value* math_min(vector<Variable*>& args) {
    if (args.empty()) return nullptr;
    
    Value* minVal = args[0]->getValue();
    if (!minVal) return nullptr;
    
    for (size_t i = 1; i < args.size(); ++i) {
        if (!args[i]) continue;
        Value* val = args[i]->getValue();
        if (!val) continue;
        
        if (Integer* int1 = dynamic_cast<Integer*>(minVal)) {
            if (Integer* int2 = dynamic_cast<Integer*>(val)) {
                if (int2->getValue() < int1->getValue()) {
                    minVal = int2;
                }
            } else if (Double* double2 = dynamic_cast<Double*>(val)) {
                if (double2->getValue() < int1->getValue()) {
                    minVal = double2;
                }
            }
        } else if (Double* double1 = dynamic_cast<Double*>(minVal)) {
            if (Integer* int2 = dynamic_cast<Integer*>(val)) {
                if (int2->getValue() < double1->getValue()) {
                    minVal = int2;
                }
            } else if (Double* double2 = dynamic_cast<Double*>(val)) {
                if (double2->getValue() < double1->getValue()) {
                    minVal = double2;
                }
            }
        }
    }
    
    return minVal;
}

Value* math_pow(vector<Variable*>& args) {
    if (args.size() != 2 || !args[0] || !args[1]) return nullptr;
    
    Value* base = args[0]->getValue();
    Value* exponent = args[1]->getValue();
    
    if (!base || !exponent) return nullptr;
    
    double baseVal = 0.0, expVal = 0.0;
    
    if (Integer* intBase = dynamic_cast<Integer*>(base)) {
        baseVal = intBase->getValue();
    } else if (Double* doubleBase = dynamic_cast<Double*>(base)) {
        baseVal = doubleBase->getValue();
    } else {
        return nullptr;
    }
    
    if (Integer* intExp = dynamic_cast<Integer*>(exponent)) {
        expVal = intExp->getValue();
    } else if (Double* doubleExp = dynamic_cast<Double*>(exponent)) {
        expVal = doubleExp->getValue();
    } else {
        return nullptr;
    }
    
    return new Double(pow(baseVal, expVal));
}

Value* math_random(vector<Variable*>& args) {
    static bool initialized = false;
    if (!initialized) {
        srand(time(nullptr));
        initialized = true;
    }
    
    if (args.empty()) {
        return new Integer(rand());
    } else if (args.size() == 1) {
        Value* val = args[0]->getValue();
        if (Integer* max = dynamic_cast<Integer*>(val)) {
            return new Integer(rand() % max->getValue());
        }
    }
    return nullptr;
}

// ==================== 高级数学函数 ====================

Value* math_sin(vector<Variable*>& args) {
    if (args.size() != 1 || !args[0]) return nullptr;
    
    Value* val = args[0]->getValue();
    if (Integer* intVal = dynamic_cast<Integer*>(val)) {
        return new Double(sin(intVal->getValue()));
    } else if (Double* doubleVal = dynamic_cast<Double*>(val)) {
        return new Double(sin(doubleVal->getValue()));
    }
    return nullptr;
}

Value* math_cos(vector<Variable*>& args) {
    if (args.size() != 1 || !args[0]) return nullptr;
    
    Value* val = args[0]->getValue();
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

// 增强数学函数插件类
class MathPluginEnhanced : public BuiltinPlugin {
public:
    PluginInfo getPluginInfo() const override {
        return PluginInfo{
            "math_enhanced",
            "1.0.0",
            "增强数学函数插件，包含基础数学运算和高级数学函数",
            {"abs", "max", "min", "pow", "random", "sin", "cos", "tan", "sqrt", "log", "floor", "ceil"}
        };
    }
    
    void registerFunctions(ScopeManager& scopeManager) override {
        // 使用辅助方法批量注册函数
        defineBuiltinFunctions(scopeManager, getFunctionMap());
    }
    
    map<string, BuiltinFunctionPtr> getFunctionMap() const override {
        return {
            // 基础数学函数
            {"abs", math_abs},
            {"max", math_max},
            {"min", math_min},
            {"pow", math_pow},
            {"random", math_random},
            
            // 高级数学函数
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
EXPORT_PLUGIN(MathPluginEnhanced)
