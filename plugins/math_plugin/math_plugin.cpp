#include "interpreter/plugins/builtin_plugin.h"
#include "interpreter/values/value.h"
#include "interpreter/types/types.h"
#include "parser/definition.h"
#include <iostream>
#include <cmath>
#include <vector>

using namespace std;

// ==================== 数学函数实现 ====================

// 手动实现数学函数
Value* math_sin(vector<Value*>& args) {
    if (args.size() != 1 || !args[0]) return nullptr;
    
    Double* value = dynamic_cast<Double*>(args[0]);
    if (!value) return nullptr;
    
    return new Double(sin(value->getValue()));
}

Value* math_cos(vector<Value*>& args) {
    if (args.size() != 1 || !args[0]) return nullptr;
    
    Double* value = dynamic_cast<Double*>(args[0]);
    if (!value) return nullptr;
    
    return new Double(cos(value->getValue()));
}

Value* math_tan(vector<Value*>& args) {
    if (args.size() != 1 || !args[0]) return nullptr;
    
    Double* value = dynamic_cast<Double*>(args[0]);
    if (!value) return nullptr;
    
    return new Double(tan(value->getValue()));
}

Value* math_sqrt(vector<Value*>& args) {
    if (args.size() != 1 || !args[0]) return nullptr;
    
    Double* value = dynamic_cast<Double*>(args[0]);
    if (!value) return nullptr;
    
    return new Double(sqrt(value->getValue()));
}

Value* math_log(vector<Value*>& args) {
    if (args.size() != 1 || !args[0]) return nullptr;
    
    Double* value = dynamic_cast<Double*>(args[0]);
    if (!value) return nullptr;
    
    return new Double(log(value->getValue()));
}

Value* math_floor(vector<Value*>& args) {
    if (args.size() != 1 || !args[0]) return nullptr;
    
    Double* value = dynamic_cast<Double*>(args[0]);
    if (!value) return nullptr;
    
    return new Double(floor(value->getValue()));
}

Value* math_ceil(vector<Value*>& args) {
    if (args.size() != 1 || !args[0]) return nullptr;
    
    Double* value = dynamic_cast<Double*>(args[0]);
    if (!value) return nullptr;
    
    return new Double(ceil(value->getValue()));
}

Value* math_round(vector<Value*>& args) {
    if (args.size() != 1 || !args[0]) return nullptr;
    
    Double* value = dynamic_cast<Double*>(args[0]);
    if (!value) return nullptr;
    
    return new Double(round(value->getValue()));
}

Value* math_abs(vector<Value*>& args) {
    if (args.size() != 1 || !args[0]) return nullptr;
    
    Double* value = dynamic_cast<Double*>(args[0]);
    if (!value) return nullptr;
    
    return new Double(fabs(value->getValue()));
}

// ==================== 数学函数插件类 ====================
class MathPlugin : public BuiltinPlugin {
public:
    PluginInfo getPluginInfo() const override {
        return PluginInfo{
            "math_plugin",
            "1.0.0",
            "基础数学函数插件，提供基本的数学运算功能",
            {"sin", "cos", "tan", "sqrt", "log", "floor", "ceil", "round", "abs"}
        };
    }
    
    std::vector<FunctionInfo> getFunctions() override {
        std::vector<FunctionInfo> functions;
        
        // 添加所有数学函数
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
            
        functions.emplace_back("round", "round(value)", 
            std::vector<std::string>{"value"},
            [](Scope* scope) -> Value* {
                if (Double* value = scope->getArgument<Double>("value")) {
                    vector<Value*> args = {value};
                    return math_round(args);
                }
                return nullptr;
            });
            
        functions.emplace_back("abs", "abs(value)", 
            std::vector<std::string>{"value"},
            [](Scope* scope) -> Value* {
                if (Double* value = scope->getArgument<Double>("value")) {
                    vector<Value*> args = {value};
                    return math_abs(args);
                }
                return nullptr;
            });
            
        return functions;
    }
};

// 导出插件
EXPORT_PLUGIN(MathPlugin)
