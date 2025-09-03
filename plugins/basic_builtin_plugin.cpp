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

// array构造函数 - 创建数组
Value* builtin_array(Scope* scope) {
    LOG_DEBUG("builtin_array() called");
    Array* array = new Array();
    
    // 使用新的*args机制处理可变参数
    if (scope->hasArgs()) {
        // 从*args中获取所有元素
        Array* argsArray = scope->getArgs();
        if (argsArray) {
            for (size_t i = 0; i < argsArray->getSize(); ++i) {
                Value* element = argsArray->getElement(i);
                if (element) {
                    array->addElement(element->clone());
                }
            }
            LOG_DEBUG("builtin_array: created array with " + to_string(array->getSize()) + " elements");
            return array;
        }
    } else if (scope->hasKwargs()) {
        // 从**kwargs中获取所有值作为元素
        Dict* kwargs = scope->getKwargs();
        if (kwargs) {
            // 使用Dict::getValues()简化实现
            vector<Value*> values = kwargs->getValues();
            // 克隆所有值
            for (Value*& value : values) {
                array->addElement(value->clone());
            }
            LOG_DEBUG("builtin_array: created array with " + to_string(array->getSize()) + " elements from kwargs");
            return array;
        }
    }
    
    // 如果没有可变参数，返回空数组
    LOG_DEBUG("builtin_array: created empty array");
    return array;
}

// dict构造函数 - 创建字典
Value* builtin_dict(Scope* scope) {
    LOG_DEBUG("builtin_dict() called");
    Dict* dict = new Dict();
    
    // 使用新的*args机制处理可变参数
    if (scope->hasArgs()) {
        // 从*args中获取所有元素
        Array* argsArray = scope->getArgs();
        if (argsArray) {
            // 每两个参数组成一个键值对
            for (size_t i = 0; i < argsArray->getSize(); i += 2) {
                if (i + 1 < argsArray->getSize()) {
                    Value* key = argsArray->getElement(i);
                    Value* value = argsArray->getElement(i + 1);
                    
                    if (key && value) {
                        string keyStr = key->toString();
                        LOG_DEBUG("builtin_dict: adding key-value pair: " + keyStr + " = " + value->toString());
                        dict->setEntry(keyStr, value->clone());
                    }
                }
            }
            LOG_DEBUG("builtin_dict: created dict with " + to_string(dict->getSize()) + " entries");
            return dict;
        }
    } else if (scope->hasKwargs()) {
        // 从**kwargs中获取所有键值对
        Dict* kwargs = scope->getKwargs();
        if (kwargs) {
            vector<string> keys = kwargs->getKeys();
            for (const string& key : keys) {
                Value* value = kwargs->getEntry(key);
                if (value) {
                    LOG_DEBUG("builtin_dict: adding key-value pair from kwargs: " + key + " = " + value->toString());
                    dict->setEntry(key, value->clone());
                }
            }
            LOG_DEBUG("builtin_dict: created dict with " + to_string(dict->getSize()) + " entries from kwargs");
            return dict;
        }
    }
    
    // 如果没有可变参数，返回空字典
    LOG_DEBUG("builtin_dict: created empty dict");
    return dict;
}

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
            "核心基础函数插件，包含print、println、count、cin、exit等基础功能，以及通过类型系统调用的通用方法",
            {"print", "println", "count", "cin", "exit", "push", "pop", "to_upper", "to_lower", "substring", "length", "abs", "max", "min", "pow", "sort", "to_string", "to_int", "to_double", "cast", "random"}
        };
    }

protected:
    
    // 重写注册函数方法，使用新的scope接口和C函数原型解析
    void registerFunctions(ScopeManager& scopeManager) override {
        LOG_DEBUG("CorePlugin::registerFunctions called with new scope interface and C function prototype parsing");
        
        // 使用宏简化内置函数注册
        REGISTER_BUILTIN_FUNCTION(scopeManager, "print", builtin_print, "print(...)");
        REGISTER_BUILTIN_FUNCTION(scopeManager, "println", builtin_println, "println(...)");
        REGISTER_BUILTIN_FUNCTION(scopeManager, "count", builtin_count, "count(value)");
        REGISTER_BUILTIN_FUNCTION(scopeManager, "cin", builtin_cin, "cin()");
        REGISTER_BUILTIN_FUNCTION(scopeManager, "exit", builtin_exit, "exit(code)");
        REGISTER_BUILTIN_FUNCTION(scopeManager, "push", builtin_push, "push(array, value)");
        REGISTER_BUILTIN_FUNCTION(scopeManager, "pop", builtin_pop, "pop(array)");
        REGISTER_BUILTIN_FUNCTION(scopeManager, "to_upper", builtin_to_upper, "to_upper(string)");
        REGISTER_BUILTIN_FUNCTION(scopeManager, "to_lower", builtin_to_lower, "to_lower(string)");
        REGISTER_BUILTIN_FUNCTION(scopeManager, "substring", builtin_substring, "substring(string, start, length)");
        REGISTER_BUILTIN_FUNCTION(scopeManager, "length", builtin_length, "length(value)");
        REGISTER_BUILTIN_FUNCTION(scopeManager, "abs", builtin_abs, "abs(value)");
        REGISTER_BUILTIN_FUNCTION(scopeManager, "max", builtin_max, "max(value, ...)");
        REGISTER_BUILTIN_FUNCTION(scopeManager, "min", builtin_min, "min(value, ...)");
        REGISTER_BUILTIN_FUNCTION(scopeManager, "pow", builtin_pow, "pow(base, exponent)");
        REGISTER_BUILTIN_FUNCTION(scopeManager, "sort", builtin_sort, "sort(array)");
        REGISTER_BUILTIN_FUNCTION(scopeManager, "to_string", builtin_to_string, "to_string(value)");
        REGISTER_BUILTIN_FUNCTION(scopeManager, "to_int", builtin_to_int, "to_int(value)");
        REGISTER_BUILTIN_FUNCTION(scopeManager, "to_double", builtin_to_double, "to_double(value)");
        REGISTER_BUILTIN_FUNCTION(scopeManager, "cast", builtin_cast, "cast(value, type)");
        REGISTER_BUILTIN_FUNCTION(scopeManager, "random", builtin_random, "random(max)");
        
        // 注册array构造函数到全局作用域
        REGISTER_BUILTIN_FUNCTION(scopeManager, "array", builtin_array, "array(...)");
        
        // 注册dict构造函数到全局作用域
        REGISTER_BUILTIN_FUNCTION(scopeManager, "dict", builtin_dict, "dict(...)");
        REGISTER_BUILTIN_FUNCTION(scopeManager, "Dict", builtin_dict, "Dict(...)");
        
        LOG_DEBUG("Registered " + to_string(23) + " functions using simplified macro interface");
    }
};

// 导出插件
EXPORT_PLUGIN(CorePlugin)
