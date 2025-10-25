#include "interpreter/plugins/builtin_plugin.h"
// #include "interpreter/plugins/plugin_manager.h"
#include "interpreter/core/interpreter.h"
#include "interpreter/scope/scope.h"
#include "interpreter/values/value.h"
#include "interpreter/types/types.h"
#include "parser/definition.h"

#include <iostream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>

using namespace std;

// ==================== 基础函数实现 ====================

// 通用的push函数 - 通过类型系统调用
Value* builtin_push(Scope* scope) {
    if (Array* target = scope->getArgument<Array>("array")) {
        if (Value* value = scope->getArgument<Value>("value")) {
            vector<Value*> pushArgs = {value};
            return PluginManager::callMethodOnValue(target, "push", pushArgs);
        }
    }
    return nullptr;
}

// 通用的pop函数 - 通过类型系统调用
Value* builtin_pop(Scope* scope) {
    if (Array* target = scope->getArgument<Array>("array")) {
        vector<Value*> popArgs;
        return PluginManager::callMethodOnValue(target, "pop", popArgs);
    }
    return nullptr;
}

// 通用的to_upper函数 - 通过类型系统调用
Value* builtin_to_upper(Scope* scope) {
    if (String* target = scope->getArgument<String>("string")) {
        vector<Value*> upperArgs;
        Value* result = PluginManager::callMethodOnValue(target, "to_upper", upperArgs);
        if (!result) {
            // 尝试其他可能的方法名
            result = PluginManager::callMethodOnValue(target, "toUpper", upperArgs);
        }
        return result;
    }
    return nullptr;
}

// 通用的to_lower函数 - 通过类型系统调用
Value* builtin_to_lower(Scope* scope) {
    if (String* target = scope->getArgument<String>("string")) {
        vector<Value*> lowerArgs;
        Value* result = PluginManager::callMethodOnValue(target, "to_lower", lowerArgs);
        if (!result) {
            // 尝试其他可能的方法名
            result = PluginManager::callMethodOnValue(target, "toLower", lowerArgs);
        }
        return result;
    }
    return nullptr;
}

// 通用的substring函数 - 通过类型系统调用
Value* builtin_substring(Scope* scope) {
    if (String* target = scope->getArgument<String>("string")) {
        if (Integer* start = scope->getArgument<Integer>("start")) {
            if (Integer* length = scope->getArgument<Integer>("length")) {
                vector<Value*> substrArgs = {start, length};
                return PluginManager::callMethodOnValue(target, "substring", substrArgs);
            }
        }
    }
    return nullptr;
}

// 通用的length函数 - 通过类型系统调用
Value* builtin_length(Scope* scope) {
    if (Value* target = scope->getArgument<Value>("value")) {
        vector<Value*> lengthArgs;
        Value* result = PluginManager::callMethodOnValue(target, "length", lengthArgs);
        if (!result) {
            // 尝试其他可能的方法名
            result = PluginManager::callMethodOnValue(target, "size", lengthArgs);
        }
        return result;
    }
    return nullptr;
}

Value* builtin_print(Scope* scope) {
    // 使用新的kwargs机制处理可变参数
    if (scope->hasKwargs()) {
        // 从kwargs中获取所有值
        Dict* kwargs = scope->getKwargs();
        if (kwargs) {
            vector<string> keys = kwargs->getKeys();
            for (size_t i = 0; i < keys.size(); ++i) {
                Value* val = kwargs->getEntry(keys[i]);
                if (val) {
                    cout << val->toString();
                    if (i < keys.size() - 1) {
                        cout << " ";
                    }
                }
            }
        }
    } else {
        // 使用新的getArgs方式
        vector<Value*> args;
        if (Array* argsArray = scope->getArgs()) {
            args = argsArray->getElements();
        }
        for (size_t i = 0; i < args.size(); ++i) {
            if (args[i]) {
                cout << args[i]->toString();
                if (i < args.size() - 1) {
                    cout << " ";
                }
            }
        }
    }
    // print不换行
    return nullptr;
}

Value* builtin_println(Scope* scope) {
    // 首先处理第一个value参数
    if (Value* value = scope->getArgument<Value>("value")) {
        cout << value->toString();
    }
    
    // 然后处理可变参数
    if (scope->hasKwargs()) {
        // 从kwargs中获取所有值
        Dict* kwargs = scope->getKwargs();
        if (kwargs) {
            vector<string> keys = kwargs->getKeys();
            for (size_t i = 0; i < keys.size(); ++i) {
                Value* val = kwargs->getEntry(keys[i]);
                if (val) {
                    cout << " " << val->toString();
                }
            }
        }
    } else if (scope->hasArgs()) {
        // 使用新的getArgs方式
        Array* argsArray = scope->getArgs();
        if (argsArray) {
            vector<Value*> args = argsArray->getElements();
            for (size_t i = 0; i < args.size(); ++i) {
                if (args[i]) {
                    cout << " " << args[i]->toString();
                }
            }
        }
    }
    
    cout << endl;  // println换行
    return nullptr;
}

Value* builtin_count(Scope* scope) {
    if (Value* val = scope->getArgument<Value>("value")) {
        if (val && val->getValueType()) {
            // 通过类型系统调用相应的方法
            vector<Value*> countArgs;
            Value* result = PluginManager::callMethodOnValue(val, "length", countArgs);
            if (!result) {
                // 尝试其他可能的方法名
                result = PluginManager::callMethodOnValue(val, "size", countArgs);
            }
            return result;
        }
    }
    return nullptr;
}

Value* builtin_cin(Scope* scope) {
    String* lastInputValue = nullptr;
    
    // 获取可变参数
    vector<Value*> args;
    if (Array* argsArray = scope->getArgs()) {
        args = argsArray->getElements();
    }
    
    // 处理所有参数，为每个参数读取一个值
    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i]) {
            string input;
            cin >> input;
            // 创建新的String对象
            String* newInput = new String(input);
            // 将新值设置回作用域
            scope->setVariable("arg" + to_string(i), newInput);
            lastInputValue = newInput;
        }
    }
    
    // 如果没有参数，读取一个值并返回
    if (args.empty()) {
        string input;
        cin >> input;
        lastInputValue = new String(input);
    }
    
    // 返回最后一个输入值（用于 let input = cin() 的情况）
    return lastInputValue;
}

// ==================== 数学函数实现 ====================

Value* builtin_abs(Scope* scope) {
    if (Value* val = scope->getArgument<Value>("value")) {
        if (Integer* intVal = dynamic_cast<Integer*>(val)) {
            return new Integer(abs(intVal->getValue()));
        } else if (Double* doubleVal = dynamic_cast<Double*>(val)) {
            return new Double(fabs(doubleVal->getValue()));
        }
    }
    return nullptr;
}

Value* builtin_max(Scope* scope) {
    // 使用新的kwargs机制处理可变参数
    vector<Value*> args;
    
    if (scope->hasKwargs()) {
        // 从kwargs中获取所有值
        Dict* kwargs = scope->getKwargs();
        if (kwargs) {
            vector<string> keys = kwargs->getKeys();
            for (const string& key : keys) {
                Value* val = kwargs->getEntry(key);
                if (val) {
                    args.push_back(val);
                }
            }
        }
    } else {
        // 使用新的getArgs方式
        if (Array* argsArray = scope->getArgs()) {
            args = argsArray->getElements();
        }
    }
    
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

Value* builtin_min(Scope* scope) {
    // 获取可变参数
    vector<Value*> args;
    if (Array* argsArray = scope->getArgs()) {
        args = argsArray->getElements();
    }
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

Value* builtin_pow(Scope* scope) {
    if (Value* base = scope->getArgument<Value>("base")) {
        if (Value* exponent = scope->getArgument<Value>("exponent")) {
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
    }
    return nullptr;
}

// ==================== 字符串函数实现 ====================



// ==================== 数组函数实现 ====================

Value* builtin_sort(Scope* scope) {
    if (Array* arr = scope->getArgument<Array>("array")) {
        // 简单的排序实现
        // 这里可以添加实际的排序逻辑
        return arr;
    }
    return nullptr;
}

// ==================== 类型转换函数实现 ====================

Value* builtin_to_string(Scope* scope) {
    if (Value* val = scope->getArgument<Value>("value")) {
        if (val) {
            return new String(val->toString());
        }
    }
    return nullptr;
}

Value* builtin_to_int(Scope* scope) {
    if (Value* val = scope->getArgument<Value>("value")) {
        if (Integer* intVal = dynamic_cast<Integer*>(val)) {
            return new Integer(intVal->getValue());
        } else if (Double* doubleVal = dynamic_cast<Double*>(val)) {
            return new Integer(static_cast<int>(doubleVal->getValue()));
        } else if (String* strVal = dynamic_cast<String*>(val)) {
            try {
                return new Integer(stoi(strVal->getValue()));
            } catch (...) {
                return nullptr;
            }
        }
    }
    return nullptr;
}

Value* builtin_to_double(Scope* scope) {
    if (Value* val = scope->getArgument<Value>("value")) {
        if (Integer* intVal = dynamic_cast<Integer*>(val)) {
            return new Double(static_cast<double>(intVal->getValue()));
        } else if (Double* doubleVal = dynamic_cast<Double*>(val)) {
            return new Double(doubleVal->getValue());
        } else if (String* strVal = dynamic_cast<String*>(val)) {
            try {
                return new Double(stod(strVal->getValue()));
            } catch (...) {
                return nullptr;
            }
        }
    }
    return nullptr;
}

Value* builtin_cast(Scope* scope) {
    if (Value* val = scope->getArgument<Value>("value")) {
        if (String* typeVal = scope->getArgument<String>("type")) {
            if (val && typeVal) {
                string type = typeVal->getValue();
                if (type == "int" || type == "integer") {
                    // 创建临时作用域来调用to_int
                    Scope tempScope;
                    tempScope.setArgument("value", val);
                    return builtin_to_int(&tempScope);
                } else if (type == "double" || type == "float") {
                    // 创建临时作用域来调用to_double
                    Scope tempScope;
                    tempScope.setArgument("value", val);
                    return builtin_to_double(&tempScope);
                } else if (type == "string") {
                    // 创建临时作用域来调用to_string
                    Scope tempScope;
                    tempScope.setArgument("value", val);
                    return builtin_to_string(&tempScope);
                }
            }
        }
    }
    return nullptr;
}

// ==================== 系统函数实现 ====================

Value* builtin_random(Scope* scope) {
    static bool initialized = false;
    if (!initialized) {
        srand(time(nullptr));
        initialized = true;
    }
    
    // 获取可变参数
    vector<Value*> args;
    if (Array* argsArray = scope->getArgs()) {
        args = argsArray->getElements();
    }
    if (args.empty()) {
        return new Integer(rand());
    } else if (args.size() == 1) {
        if (Integer* max = scope->getArgument<Integer>("max")) {
            return new Integer(rand() % max->getValue());
        }
    }
    return nullptr;
}

// 注意：array和dict构造函数已移至ArrayType和DictType类中作为静态方法
// 这里不再需要重复实现

Value* builtin_exit(Scope* scope) {
    if (Integer* code = scope->getArgument<Integer>("code")) {
        int exitCode = code->getValue();
        throw ReturnException(new Integer(exitCode));
    }
    throw ReturnException(new Integer(0));
}

// 核心基础函数插件类
class CorePlugin : public BuiltinPlugin {
public:
    PluginInfo getPluginInfo() const override {
        return PluginInfo{
            "core",
            "1.0.0",
            "核心基础函数插件，包含print、println、count、cin、exit等基础功能以及基础数据结构构造函数",
            {"print", "println", "count", "cin", "exit", "array", "dict"}
        };
    }

protected:
    
    std::vector<FunctionInfo> getFunctions() override {
        std::vector<FunctionInfo> functions;
        
        // 添加核心内置函数
        functions.emplace_back("print", "print(...)", 
            std::vector<std::string>{},
            [](Scope* scope) -> Value* {
                return builtin_print(scope);
            });
            
        functions.emplace_back("println", "println(value, ...)", 
            std::vector<std::string>{"value"},
            [](Scope* scope) -> Value* {
                return builtin_println(scope);
            });
            
        functions.emplace_back("count", "count(value)", 
            std::vector<std::string>{"value"},
            [](Scope* scope) -> Value* {
                return builtin_count(scope);
            });
            
        functions.emplace_back("cin", "cin()", 
            std::vector<std::string>{},
            [](Scope* scope) -> Value* {
                return builtin_cin(scope);
            });
            
        functions.emplace_back("exit", "exit(code)", 
            std::vector<std::string>{"code"},
            [](Scope* scope) -> Value* {
                return builtin_exit(scope);
            });
            
        // 注意：array和dict构造函数应该通过类型系统调用，而不是插件函数
        // 这样才是真正的面向对象设计
            
        return functions;
    }
};

// 导出插件
EXPORT_PLUGIN(CorePlugin)
