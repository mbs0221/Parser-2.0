#include "interpreter/plugins/builtin_plugin.h"
#include "parser/definition.h"

#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>

using namespace std;

// ==================== 基础数学函数 ====================

Value* math_abs(vector<Value*>& args) {
    if (args.size() != 1 || !args[0]) return nullptr;
    
    Value* val = args[0];
    if (Integer* intVal = dynamic_cast<Integer*>(val)) {
        return new Integer(abs(intVal->getValue()));
    } else if (Double* doubleVal = dynamic_cast<Double*>(val)) {
        return new Double(fabs(doubleVal->getValue()));
    }
    return nullptr;
}

Value* math_max(vector<Value*>& args) {
    if (args.empty()) return nullptr;
    
    Value* maxVal = args[0];
    if (!maxVal) return nullptr;
    
    for (size_t i = 1; i < args.size(); ++i) {
        if (!args[i]) continue;
        Value* val = args[i];
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

Value* math_min(vector<Value*>& args) {
    if (args.empty()) return nullptr;
    
    Value* minVal = args[0];
    if (!minVal) return nullptr;
    
    for (size_t i = 1; i < args.size(); ++i) {
        if (!args[i]) continue;
        Value* val = args[i];
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

Value* math_pow(vector<Value*>& args) {
    if (args.size() != 2 || !args[0] || !args[1]) return nullptr;
    
    Value* base = args[0];
    Value* exponent = args[1];
    
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

Value* math_random(vector<Value*>& args) {
    static bool initialized = false;
    if (!initialized) {
        srand(time(nullptr));
        initialized = true;
    }
    
    if (args.empty()) {
        return new Integer(rand());
    } else if (args.size() == 1) {
        Value* val = args[0];
        if (Integer* max = dynamic_cast<Integer*>(val)) {
            return new Integer(rand() % max->getValue());
        }
    }
    return nullptr;
}

// ==================== 高级数学函数 ====================

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

Value* math_tan(vector<Value*>& args) {
    if (args.size() != 1 || !args[0]) return nullptr;
    
    Value* val = args[0];
    if (Integer* intVal = dynamic_cast<Integer*>(val)) {
        return new Double(tan(intVal->getValue()));
    } else if (Double* doubleVal = dynamic_cast<Double*>(val)) {
        return new Double(tan(doubleVal->getValue()));
    }
    return nullptr;
}

Value* math_sqrt(vector<Value*>& args) {
    if (args.size() != 1 || !args[0]) return nullptr;
    
    Value* val = args[0];
    if (Integer* intVal = dynamic_cast<Integer*>(val)) {
        if (intVal->getValue() < 0) return nullptr;
        return new Double(sqrt(intVal->getValue()));
    } else if (Double* doubleVal = dynamic_cast<Double*>(val)) {
        if (doubleVal->getValue() < 0) return nullptr;
        return new Double(sqrt(doubleVal->getValue()));
    }
    return nullptr;
}

Value* math_log(vector<Value*>& args) {
    if (args.size() != 1 || !args[0]) return nullptr;
    
    Value* val = args[0];
    if (Integer* intVal = dynamic_cast<Integer*>(val)) {
        if (intVal->getValue() <= 0) return nullptr;
        return new Double(log(intVal->getValue()));
    } else if (Double* doubleVal = dynamic_cast<Double*>(val)) {
        if (doubleVal->getValue() <= 0) return nullptr;
        return new Double(log(doubleVal->getValue()));
    }
    return nullptr;
}

Value* math_floor(vector<Value*>& args) {
    if (args.size() != 1 || !args[0]) return nullptr;
    
    Value* val = args[0];
    if (Integer* intVal = dynamic_cast<Integer*>(val)) {
        return new Integer(intVal->getValue());
    } else if (Double* doubleVal = dynamic_cast<Double*>(val)) {
        return new Integer((int)floor(doubleVal->getValue()));
    }
    return nullptr;
}

Value* math_ceil(vector<Value*>& args) {
    if (args.size() != 1 || !args[0]) return nullptr;
    
    Value* val = args[0];
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
    
    std::vector<FunctionInfo> getFunctions() override {
        std::vector<FunctionInfo> functions;
        
        // 添加基础数学函数
        functions.emplace_back("abs", "abs(value)", 
            std::vector<std::string>{"value"},
            [](Scope* scope) -> Value* {
                if (Double* value = scope->getArgument<Double>("value")) {
                    vector<Value*> args = {value};
                    return math_abs(args);
                }
                return nullptr;
            });
            
        functions.emplace_back("max", "max(value, ...)", 
            std::vector<std::string>{"value"},
            [](Scope* scope) -> Value* {
                vector<Value*> args = scope->getAllArguments();
                return math_max(args);
            });
            
        functions.emplace_back("min", "min(value, ...)", 
            std::vector<std::string>{"value"},
            [](Scope* scope) -> Value* {
                vector<Value*> args = scope->getAllArguments();
                return math_min(args);
            });
            
        functions.emplace_back("pow", "pow(base, exponent)", 
            std::vector<std::string>{"base", "exponent"},
            [](Scope* scope) -> Value* {
                Double* base = scope->getArgument<Double>("base");
                Double* exponent = scope->getArgument<Double>("exponent");
                if (base && exponent) {
                    vector<Value*> args = {base, exponent};
                    return math_pow(args);
                }
                return nullptr;
            });
            
        functions.emplace_back("random", "random()", 
            std::vector<std::string>{},
            [](Scope* scope) -> Value* {
                vector<Value*> args = {};
                return math_random(args);
            });
            
        // 添加高级数学函数
        functions.emplace_back("sin", "sin(value)", 
            std::vector<std::string>{"value"},
            [](Scope* scope) -> Value* {
                if (Double* value = scope->getArgument<Double>("value")) {
                    vector<Value*> args = {value};
                    return math_sin(args);
                }
                return nullptr;
            });
            
        functions.emplace_back("cos", "cos(value)", 
            std::vector<std::string>{"value"},
            [](Scope* scope) -> Value* {
                if (Double* value = scope->getArgument<Double>("value")) {
                    vector<Value*> args = {value};
                    return math_cos(args);
                }
                return nullptr;
            });
            
        functions.emplace_back("tan", "tan(value)", 
            std::vector<std::string>{"value"},
            [](Scope* scope) -> Value* {
                if (Double* value = scope->getArgument<Double>("value")) {
                    vector<Value*> args = {value};
                    return math_tan(args);
                }
                return nullptr;
            });
            
        functions.emplace_back("sqrt", "sqrt(value)", 
            std::vector<std::string>{"value"},
            [](Scope* scope) -> Value* {
                if (Double* value = scope->getArgument<Double>("value")) {
                    vector<Value*> args = {value};
                    return math_sqrt(args);
                }
                return nullptr;
            });
            
        functions.emplace_back("log", "log(value)", 
            std::vector<std::string>{"value"},
            [](Scope* scope) -> Value* {
                if (Double* value = scope->getArgument<Double>("value")) {
                    vector<Value*> args = {value};
                    return math_log(args);
                }
                return nullptr;
            });
            
        functions.emplace_back("floor", "floor(value)", 
            std::vector<std::string>{"value"},
            [](Scope* scope) -> Value* {
                if (Double* value = scope->getArgument<Double>("value")) {
                    vector<Value*> args = {value};
                    return math_floor(args);
                }
                return nullptr;
            });
            
        functions.emplace_back("ceil", "ceil(value)", 
            std::vector<std::string>{"value"},
            [](Scope* scope) -> Value* {
                if (Double* value = scope->getArgument<Double>("value")) {
                    vector<Value*> args = {value};
                    return math_ceil(args);
                }
                return nullptr;
            });
            
        return functions;
    }
};

// 导出插件
EXPORT_PLUGIN(MathPluginEnhanced)
