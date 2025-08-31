#include "interpreter/builtin_plugin.h"
#include "interpreter/value.h"
#include "parser/function.h"

#include <iostream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>

using namespace std;

// ==================== 辅助函数 ====================

// 通过类型系统将值转换为字符串
string convertToString(Value* value) {
    if (!value) return "null";
    
    // 通过类型系统调用toString方法
    if (value->getValueType()) {
        vector<Value*> toStringArgs;
        Value* toStringResult = value->getValueType()->callMethod(value, "toString", toStringArgs);
        if (toStringResult) {
            string result;
            if (String* strResult = dynamic_cast<String*>(toStringResult)) {
                result = strResult->getValue();
            } else {
                result = toStringResult->toString();
            }
            delete toStringResult;
            return result;
        }
    }
    
    // 如果类型系统没有toString方法，使用默认的toString
    return value->toString();
}

// 通过类型系统将值转换为指定类型
Value* convertToType(Value* value, const string& targetTypeName) {
    if (!value) return nullptr;
    
    // 通过类型系统调用convertTo方法
    if (value->getValueType()) {
        vector<Value*> convertArgs;
        // 创建目标类型名称的字符串值作为参数
        convertArgs.push_back(new String(targetTypeName));
        Value* convertResult = value->getValueType()->callMethod(value, "convertTo", convertArgs);
        if (convertResult) {
            return convertResult;
        }
    }
    
    // 如果类型系统没有convertTo方法，尝试直接转换
    TypeRegistry* registry = TypeRegistry::getInstance();
    if (registry) {
        ObjectType* targetType = registry->getType(targetTypeName);
        if (targetType) {
            return targetType->convertTo(targetType, value);
        }
    }
    
    return nullptr;
}

// 通过类型系统调用方法
Value* callMethodOnValue(Value* value, const string& methodName, vector<Value*>& args) {
    if (!value || !value->getValueType()) return nullptr;
    
    // 通过类型系统调用方法
    return value->getValueType()->callMethod(value, methodName, args);
}

// ==================== 基础函数实现 ====================

// 通用的push函数 - 通过类型系统调用
Value* builtin_push(vector<Value*>& args) {
    if (args.size() >= 2) {
        Value* target = args[0];
        if (target && target->getValueType()) {
            vector<Value*> pushArgs(args.begin() + 1, args.end());
            return callMethodOnValue(target, "push", pushArgs);
        }
    }
    return nullptr;
}

// 通用的pop函数 - 通过类型系统调用
Value* builtin_pop(vector<Value*>& args) {
    if (args.size() == 1) {
        Value* target = args[0];
        if (target && target->getValueType()) {
            vector<Value*> popArgs;
            return callMethodOnValue(target, "pop", popArgs);
        }
    }
    return nullptr;
}

// 通用的to_upper函数 - 通过类型系统调用
Value* builtin_to_upper(vector<Value*>& args) {
    if (args.size() == 1) {
        Value* target = args[0];
        if (target && target->getValueType()) {
            vector<Value*> upperArgs;
            Value* result = callMethodOnValue(target, "to_upper", upperArgs);
            if (!result) {
                // 尝试其他可能的方法名
                result = callMethodOnValue(target, "toUpper", upperArgs);
            }
            return result;
        }
    }
    return nullptr;
}

// 通用的to_lower函数 - 通过类型系统调用
Value* builtin_to_lower(vector<Value*>& args) {
    if (args.size() == 1) {
        Value* target = args[0];
        if (target && target->getValueType()) {
            vector<Value*> lowerArgs;
            Value* result = callMethodOnValue(target, "to_lower", lowerArgs);
            if (!result) {
                // 尝试其他可能的方法名
                result = callMethodOnValue(target, "toLower", lowerArgs);
            }
            return result;
        }
    }
    return nullptr;
}

// 通用的substring函数 - 通过类型系统调用
Value* builtin_substring(vector<Value*>& args) {
    if (args.size() >= 3) {
        Value* target = args[0];
        if (target && target->getValueType()) {
            vector<Value*> substrArgs(args.begin() + 1, args.end());
            return callMethodOnValue(target, "substring", substrArgs);
        }
    }
    return nullptr;
}

// 通用的length函数 - 通过类型系统调用
Value* builtin_length(vector<Value*>& args) {
    if (args.size() == 1) {
        Value* target = args[0];
        if (target && target->getValueType()) {
            vector<Value*> lengthArgs;
            Value* result = callMethodOnValue(target, "length", lengthArgs);
            if (!result) {
                // 尝试其他可能的方法名
                result = callMethodOnValue(target, "size", lengthArgs);
            }
            return result;
        }
    }
    return nullptr;
}

Value* builtin_print(vector<Value*>& args) {
    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i]) {
            cout << convertToString(args[i]);
            if (i < args.size() - 1) {
                cout << " ";
            }
        }
    }
    // print不换行
    return nullptr;
}

Value* builtin_println(vector<Value*>& args) {
    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i]) {
            cout << convertToString(args[i]);
            if (i < args.size() - 1) {
                cout << " ";
            }
        }
    }
    cout << endl;  // println换行
    return nullptr;
}

Value* builtin_count(vector<Value*>& args) {
    if (args.size() == 1) {
        Value* val = args[0];
        if (val && val->getValueType()) {
            // 通过类型系统调用相应的方法
            vector<Value*> countArgs;
            Value* result = val->getValueType()->callMethod(val, "length", countArgs);
            if (!result) {
                // 尝试其他可能的方法名
                result = val->getValueType()->callMethod(val, "size", countArgs);
            }
            return result;
        }
    }
    return nullptr;
}

Value* builtin_cin(vector<Value*>& args) {
    String* lastInputValue = nullptr;
    
    // 处理所有参数，为每个参数读取一个值
    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i]) {
            string input;
            cin >> input;
            // 直接修改args向量中的指针
            args[i] = new String(input);
            lastInputValue = dynamic_cast<String*>(args[i]);
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

Value* builtin_abs(vector<Value*>& args) {
    if (args.size() != 1 || !args[0]) {
        return nullptr;
    }
    
    Value* val = args[0];
    if (Integer* intVal = dynamic_cast<Integer*>(val)) {
        return new Integer(abs(intVal->getValue()));
    } else if (Double* doubleVal = dynamic_cast<Double*>(val)) {
        return new Double(fabs(doubleVal->getValue()));
    }
    return nullptr;
}

Value* builtin_max(vector<Value*>& args) {
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

Value* builtin_min(vector<Value*>& args) {
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

Value* builtin_pow(vector<Value*>& args) {
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

// ==================== 字符串函数实现 ====================

Value* builtin_length(vector<Value*>& args) {
    if (args.size() != 1 || !args[0]) return nullptr;
    
    Value* val = args[0];
    if (String* str = dynamic_cast<String*>(val)) {
        return new Integer(str->length());
    } else if (Array* arr = dynamic_cast<Array*>(val)) {
        return new Integer(arr->size());
    }
    return nullptr;
}

Value* builtin_substring(vector<Value*>& args) {
    if (args.size() < 2 || args.size() > 3 || !args[0]) return nullptr;
    
    Value* strVal = args[0];
    if (String* str = dynamic_cast<String*>(strVal)) {
        string s = str->getValue();
        
        if (args.size() == 2) {
            // substring(str, start)
            if (Integer* start = dynamic_cast<Integer*>(args[1])) {
                int startPos = start->getValue();
                if (startPos >= 0 && startPos < s.length()) {
                    return new String(s.substr(startPos));
                }
            }
        } else if (args.size() == 3) {
            // substring(str, start, length)
            if (Integer* start = dynamic_cast<Integer*>(args[1])) {
                if (Integer* length = dynamic_cast<Integer*>(args[2])) {
                    int startPos = start->getValue();
                    int len = length->getValue();
                    if (startPos >= 0 && len >= 0 && startPos + len <= s.length()) {
                        return new String(s.substr(startPos, len));
                    }
                }
            }
        }
    }
    return nullptr;
}



// ==================== 数组函数实现 ====================

Value* builtin_sort(vector<Value*>& args) {
    if (args.size() != 1 || !args[0]) return nullptr;
    
    Value* arrVal = args[0];
    if (Array* arr = dynamic_cast<Array*>(arrVal)) {
        // 简单的排序实现
        // 这里可以添加实际的排序逻辑
        return arr;
    }
    
    return nullptr;
}

// ==================== 类型转换函数实现 ====================

Value* builtin_to_string(vector<Value*>& args) {
    if (args.size() != 1 || !args[0]) return nullptr;
    
    Value* val = args[0];
    if (val) {
        return new String(val->toString());
    }
    return nullptr;
}

Value* builtin_to_int(vector<Value*>& args) {
    if (args.size() != 1 || !args[0]) return nullptr;
    
    Value* val = args[0];
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
    return nullptr;
}

Value* builtin_to_double(vector<Value*>& args) {
    if (args.size() != 1 || !args[0]) return nullptr;
    
    Value* val = args[0];
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
    return nullptr;
}

Value* builtin_cast(vector<Value*>& args) {
    if (args.size() != 2 || !args[0] || !args[1]) return nullptr;
    
    Value* val = args[0];
    Value* typeVal = args[1];
    
    if (!val || !typeVal) return nullptr;
    
    if (String* typeStr = dynamic_cast<String*>(typeVal)) {
        string type = typeStr->getValue();
        if (type == "int" || type == "integer") {
            vector<Value*> tempArgs = {args[0]};
            return builtin_to_int(tempArgs);
        } else if (type == "double" || type == "float") {
            vector<Value*> tempArgs = {args[0]};
            return builtin_to_double(tempArgs);
        } else if (type == "string") {
            vector<Value*> tempArgs = {args[0]};
            return builtin_to_string(tempArgs);
        }
    }
    return nullptr;
}

// ==================== 系统函数实现 ====================

Value* builtin_random(vector<Value*>& args) {
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

Value* builtin_exit(vector<Value*>& args) {
    int exitCode = 0;
    if (args.size() == 1 && args[0]) {
        Value* val = args[0];
        if (Integer* code = dynamic_cast<Integer*>(val)) {
            exitCode = code->getValue();
        }
    }
    exit(exitCode);
    return nullptr;
}

// 核心基础函数插件类
class CorePlugin : public BuiltinPlugin {
public:
    PluginInfo getPluginInfo() const override {
        return PluginInfo{
            "core",
            "1.0.0",
            "核心基础函数插件，包含print、println、count、cin、exit等基础功能，以及通过类型系统调用的通用方法",
            {"print", "println", "count", "cin", "exit", "push", "pop", "to_upper", "to_lower", "substring", "length"}
        };
    }
    
    void registerFunctions(ScopeManager& scopeManager) override {
        // 使用辅助方法批量注册函数
        defineBuiltinFunctions(scopeManager, getFunctionMap());
    }
    
    map<string, BuiltinFunctionPtr> getFunctionMap() const override {
        return {
            {"print", builtin_print},
            {"println", builtin_println},
            {"count", builtin_count},
            {"cin", builtin_cin},
            {"exit", builtin_exit},
            {"push", builtin_push},
            {"pop", builtin_pop},
            {"to_upper", builtin_to_upper},
            {"to_lower", builtin_to_lower},
            {"substring", builtin_substring},
            {"length", builtin_length}
        };
    }
};

// 导出插件
EXPORT_PLUGIN(CorePlugin)
