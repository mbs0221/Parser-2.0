#include "interpreter/builtin_plugin.h"
#include "lexer/value.h"
#include "parser/function.h"

#include <iostream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>

using namespace std;

// ==================== 基础函数实现 ====================

Value* builtin_print(vector<Variable*>& args) {
    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i]) {
            Value* value = args[i]->getValue();
            if (value) {
                cout << value->str();  // 使用str()方法，不添加引号
                if (i < args.size() - 1) {
                    cout << " ";
                }
            }
        }
    }
    cout << endl;
    return nullptr;
}

Value* builtin_count(vector<Variable*>& args) {
    if (args.size() == 1) {
        Variable* value = args[0];
        if (value) {
            Value* val = value->getValue();
            if (Array* array = dynamic_cast<Array*>(val)) {
                return new Integer(array->size());
            } else if (String* str = dynamic_cast<String*>(val)) {
                return new Integer(str->length());
            } else if (Dict* dict = dynamic_cast<Dict*>(val)) {
                return new Integer(dict->getEntryCount());
            }
        }
    }
    return nullptr;
}

Value* builtin_cin(vector<Variable*>& args) {
    String* lastInputValue = nullptr;
    
    // 处理所有参数，为每个参数读取一个值
    for (Variable* arg : args) {
        if (arg) {
            string input;
            cin >> input;
            String* inputValue = new String(input);
            arg->setValue(inputValue);
            lastInputValue = inputValue;
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

Value* builtin_abs(vector<Variable*>& args) {
    if (args.size() != 1 || !args[0]) {
        return nullptr;
    }
    
    Value* val = args[0]->getValue();
    if (Integer* intVal = dynamic_cast<Integer*>(val)) {
        return new Integer(abs(intVal->getValue()));
    } else if (Double* doubleVal = dynamic_cast<Double*>(val)) {
        return new Double(fabs(doubleVal->getValue()));
    }
    return nullptr;
}

Value* builtin_max(vector<Variable*>& args) {
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

Value* builtin_min(vector<Variable*>& args) {
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

Value* builtin_pow(vector<Variable*>& args) {
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

// ==================== 字符串函数实现 ====================

Value* builtin_length(vector<Variable*>& args) {
    if (args.size() != 1 || !args[0]) return nullptr;
    
    Value* val = args[0]->getValue();
    if (String* str = dynamic_cast<String*>(val)) {
        return new Integer(str->length());
    } else if (Array* arr = dynamic_cast<Array*>(val)) {
        return new Integer(arr->size());
    }
    return nullptr;
}

Value* builtin_substring(vector<Variable*>& args) {
    if (args.size() < 2 || args.size() > 3 || !args[0]) return nullptr;
    
    Value* strVal = args[0]->getValue();
    if (String* str = dynamic_cast<String*>(strVal)) {
        string s = str->getValue();
        
        if (args.size() == 2) {
            // substring(str, start)
            if (Integer* start = dynamic_cast<Integer*>(args[1]->getValue())) {
                int startPos = start->getValue();
                if (startPos >= 0 && startPos < s.length()) {
                    return new String(s.substr(startPos));
                }
            }
        } else if (args.size() == 3) {
            // substring(str, start, length)
            if (Integer* start = dynamic_cast<Integer*>(args[1]->getValue())) {
                if (Integer* length = dynamic_cast<Integer*>(args[2]->getValue())) {
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

Value* builtin_upper(vector<Variable*>& args) {
    if (args.size() != 1 || !args[0]) return nullptr;
    
    Value* val = args[0]->getValue();
    if (String* str = dynamic_cast<String*>(val)) {
        string s = str->getValue();
        transform(s.begin(), s.end(), s.begin(), ::toupper);
        return new String(s);
    }
    return nullptr;
}

Value* builtin_lower(vector<Variable*>& args) {
    if (args.size() != 1 || !args[0]) return nullptr;
    
    Value* val = args[0]->getValue();
    if (String* str = dynamic_cast<String*>(val)) {
        string s = str->getValue();
        transform(s.begin(), s.end(), s.begin(), ::tolower);
        return new String(s);
    }
    return nullptr;
}

// ==================== 数组函数实现 ====================

Value* builtin_push(vector<Variable*>& args) {
    if (args.size() < 2 || !args[0]) return nullptr;
    
    Value* arrVal = args[0]->getValue();
    if (Array* arr = dynamic_cast<Array*>(arrVal)) {
        for (size_t i = 1; i < args.size(); ++i) {
            if (args[i]) {
                // 使用Array的addElement方法
                arr->addElement(args[i]->getValue());
            }
        }
        return arr;
    }
    return nullptr;
}

Value* builtin_pop(vector<Variable*>& args) {
    if (args.size() != 1 || !args[0]) return nullptr;
    
    Value* arrVal = args[0]->getValue();
    if (Array* arr = dynamic_cast<Array*>(arrVal)) {
        if (arr->size() > 0) {
            // 获取最后一个元素并移除
            Value* last = arr->getElement(arr->size() - 1);
            // 这里需要实现移除最后一个元素的逻辑
            // 暂时返回最后一个元素
            return last;
        }
    }
    return nullptr;
}

Value* builtin_sort(vector<Variable*>& args) {
    if (args.size() != 1 || !args[0]) return nullptr;
    
    Value* arrVal = args[0]->getValue();
    if (Array* arr = dynamic_cast<Array*>(arrVal)) {
        // 简单的排序实现
        // 这里可以添加实际的排序逻辑
        return arr;
    }
    
    return nullptr;
}

// ==================== 类型转换函数实现 ====================

Value* builtin_to_string(vector<Variable*>& args) {
    if (args.size() != 1 || !args[0]) return nullptr;
    
    Value* val = args[0]->getValue();
    if (val) {
        return new String(val->str());
    }
    return nullptr;
}

Value* builtin_to_int(vector<Variable*>& args) {
    if (args.size() != 1 || !args[0]) return nullptr;
    
    Value* val = args[0]->getValue();
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

Value* builtin_to_double(vector<Variable*>& args) {
    if (args.size() != 1 || !args[0]) return nullptr;
    
    Value* val = args[0]->getValue();
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

Value* builtin_cast(vector<Variable*>& args) {
    if (args.size() != 2 || !args[0] || !args[1]) return nullptr;
    
    Value* val = args[0]->getValue();
    Value* typeVal = args[1]->getValue();
    
    if (!val || !typeVal) return nullptr;
    
    if (String* typeStr = dynamic_cast<String*>(typeVal)) {
        string type = typeStr->getValue();
        if (type == "int" || type == "integer") {
            vector<Variable*> tempArgs = {args[0]};
            return builtin_to_int(tempArgs);
        } else if (type == "double" || type == "float") {
            vector<Variable*> tempArgs = {args[0]};
            return builtin_to_double(tempArgs);
        } else if (type == "string") {
            vector<Variable*> tempArgs = {args[0]};
            return builtin_to_string(tempArgs);
        }
    }
    return nullptr;
}

// ==================== 系统函数实现 ====================

Value* builtin_random(vector<Variable*>& args) {
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

Value* builtin_exit(vector<Variable*>& args) {
    int exitCode = 0;
    if (args.size() == 1 && args[0]) {
        Value* val = args[0]->getValue();
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
            "核心基础函数插件，包含print、count、cin、exit等基础功能",
            {"print", "count", "cin", "exit"}
        };
    }
    
    void registerFunctions(ScopeManager& scopeManager) override {
        // 使用辅助方法批量注册函数
        defineBuiltinFunctions(scopeManager, getFunctionMap());
    }
    
    map<string, BuiltinFunctionPtr> getFunctionMap() const override {
        return {
            {"print", builtin_print},
            {"count", builtin_count},
            {"cin", builtin_cin},
            {"exit", builtin_exit}
        };
    }
};

// 导出插件
EXPORT_PLUGIN(CorePlugin)
