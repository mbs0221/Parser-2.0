#include "interpreter/builtin.h"
#include "lexer/value.h"
#include "parser/function.h"
#include <iostream>
#include <sstream>
#include <typeinfo>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>

using namespace std;

Value* builtin_print(vector<Variable*>& args) {
    for (auto arg : args) {
        if (arg) {
            Value* value = arg->getValue();
            if (value) {
                cout << value->toString();
            }
        }
    }
    return nullptr;
};

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
};

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

// ==================== 数学函数 ====================

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
            }
        } else if (Double* double1 = dynamic_cast<Double*>(maxVal)) {
            if (Double* double2 = dynamic_cast<Double*>(val)) {
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
            }
        } else if (Double* double1 = dynamic_cast<Double*>(minVal)) {
            if (Double* double2 = dynamic_cast<Double*>(val)) {
                if (double2->getValue() < double1->getValue()) {
                    minVal = double2;
                }
            }
        }
    }
    
    return minVal;
}

Value* builtin_pow(vector<Variable*>& args) {
    if (args.size() != 2 || !args[0] || !args[1]) {
        return nullptr;
    }
    
    Value* base = args[0]->getValue();
    Value* exponent = args[1]->getValue();
    
    if (Integer* baseInt = dynamic_cast<Integer*>(base)) {
        if (Integer* expInt = dynamic_cast<Integer*>(exponent)) {
            return new Integer((int)pow(baseInt->getValue(), expInt->getValue()));
        }
    } else if (Double* baseDouble = dynamic_cast<Double*>(base)) {
        if (Double* expDouble = dynamic_cast<Double*>(exponent)) {
            return new Double(pow(baseDouble->getValue(), expDouble->getValue()));
        }
    }
    
    return nullptr;
}

// ==================== 字符串函数 ====================

Value* builtin_length(vector<Variable*>& args) {
    if (args.size() != 1 || !args[0]) {
        return nullptr;
    }
    
    Value* val = args[0]->getValue();
    if (String* str = dynamic_cast<String*>(val)) {
        return new Integer(str->length());
    } else if (Array* arr = dynamic_cast<Array*>(val)) {
        return new Integer(arr->size());
    }
    
    return nullptr;
}

Value* builtin_substring(vector<Variable*>& args) {
    if (args.size() < 2 || args.size() > 3 || !args[0] || !args[1]) {
        return nullptr;
    }
    
    Value* strVal = args[0]->getValue();
    Value* startVal = args[1]->getValue();
    Value* lengthVal = args.size() == 3 ? args[2]->getValue() : nullptr;
    
    if (String* str = dynamic_cast<String*>(strVal)) {
        if (Integer* start = dynamic_cast<Integer*>(startVal)) {
            size_t startPos = start->getValue();
            size_t len = str->length();
            
            if (lengthVal) {
                if (Integer* length = dynamic_cast<Integer*>(lengthVal)) {
                    len = length->getValue();
                }
            }
            
            string result = str->substring(startPos, len);
            return new String(result);
        }
    }
    
    return nullptr;
}

Value* builtin_upper(vector<Variable*>& args) {
    if (args.size() != 1 || !args[0]) {
        return nullptr;
    }
    
    Value* val = args[0]->getValue();
    if (String* str = dynamic_cast<String*>(val)) {
        string result = str->getValue();
        transform(result.begin(), result.end(), result.begin(), ::toupper);
        return new String(result);
    }
    
    return nullptr;
}

Value* builtin_lower(vector<Variable*>& args) {
    if (args.size() != 1 || !args[0]) {
        return nullptr;
    }
    
    Value* val = args[0]->getValue();
    if (String* str = dynamic_cast<String*>(val)) {
        string result = str->getValue();
        transform(result.begin(), result.end(), result.begin(), ::tolower);
        return new String(result);
    }
    
    return nullptr;
}

// ==================== 数组函数 ====================

Value* builtin_push(vector<Variable*>& args) {
    if (args.size() < 2 || !args[0]) {
        return nullptr;
    }
    
    Value* arrVal = args[0]->getValue();
    if (Array* arr = dynamic_cast<Array*>(arrVal)) {
        for (size_t i = 1; i < args.size(); ++i) {
            if (args[i]) {
                arr->addElement(args[i]->getValue());
            }
        }
        return new Integer(arr->size());
    }
    
    return nullptr;
}

Value* builtin_pop(vector<Variable*>& args) {
    if (args.size() != 1 || !args[0]) {
        return nullptr;
    }
    
    Value* arrVal = args[0]->getValue();
    if (Array* arr = dynamic_cast<Array*>(arrVal)) {
        if (arr->size() > 0) {
            Value* lastElement = arr->getElement(arr->size() - 1);
            // 这里需要实现数组的pop操作
            return lastElement;
        }
    }
    
    return nullptr;
}

Value* builtin_sort(vector<Variable*>& args) {
    if (args.size() != 1 || !args[0]) {
        return nullptr;
    }
    
    Value* arrVal = args[0]->getValue();
    if (Array* arr = dynamic_cast<Array*>(arrVal)) {
        // 这里需要实现数组的排序操作
        // 暂时返回原数组
        return arr;
    }
    
    return nullptr;
}

// ==================== 类型转换函数 ====================

Value* builtin_to_string(vector<Variable*>& args) {
    if (args.size() != 1 || !args[0]) {
        return nullptr;
    }
    
    Value* val = args[0]->getValue();
    if (val) {
        return new String(val->toString());
    }
    
    return nullptr;
}

Value* builtin_to_int(vector<Variable*>& args) {
    if (args.size() != 1 || !args[0]) {
        return nullptr;
    }
    
    Value* val = args[0]->getValue();
    if (String* str = dynamic_cast<String*>(val)) {
        try {
            int result = stoi(str->getValue());
            return new Integer(result);
        } catch (const exception&) {
            return nullptr;
        }
    } else if (Double* dbl = dynamic_cast<Double*>(val)) {
        return new Integer((int)dbl->getValue());
    } else if (Bool* b = dynamic_cast<Bool*>(val)) {
        return new Integer(b->getValue() ? 1 : 0);
    }
    
    return nullptr;
}

Value* builtin_to_double(vector<Variable*>& args) {
    if (args.size() != 1 || !args[0]) {
        return nullptr;
    }
    
    Value* val = args[0]->getValue();
    if (String* str = dynamic_cast<String*>(val)) {
        try {
            double result = stod(str->getValue());
            return new Double(result);
        } catch (const exception&) {
            return nullptr;
        }
    } else if (Integer* integer = dynamic_cast<Integer*>(val)) {
        return new Double((double)integer->getValue());
    } else if (Bool* b = dynamic_cast<Bool*>(val)) {
        return new Double(b->getValue() ? 1.0 : 0.0);
    }
    
    return nullptr;
}

// ==================== 系统函数 ====================

Value* builtin_random(vector<Variable*>& args) {
    if (args.size() == 0) {
        // 返回 0-1 之间的随机浮点数
        return new Double((double)rand() / RAND_MAX);
    } else if (args.size() == 1 && args[0]) {
        Value* val = args[0]->getValue();
        if (Integer* max = dynamic_cast<Integer*>(val)) {
            // 返回 0 到 max-1 之间的随机整数
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