#include <algorithm>
#include <cmath>
#include <cctype>

#include "interpreter/values/value.h"
#include "interpreter/types/types.h"
#include "interpreter/utils/logger.h"
#include "lexer/token.h"

using namespace std;

// ==================== 基础内置类型构造函数实现 ====================

// BoolType构造函数
BoolType::BoolType() : PrimitiveType("bool", true) {
    // 注册构造函数作为静态方法
    registerStaticMethod("bool", [](vector<Value*>& args) -> Value* {
        if (args.size() == 0) {
            return new Bool(false);  // 默认值
        } else if (args.size() == 1) {
            if (Bool* boolVal = dynamic_cast<Bool*>(args[0])) {
                return new Bool(boolVal->getValue());
            } else if (Integer* intVal = dynamic_cast<Integer*>(args[0])) {
                return new Bool(intVal->getValue() != 0);
            } else if (Double* doubleVal = dynamic_cast<Double*>(args[0])) {
                return new Bool(doubleVal->getValue() != 0.0);
            } else if (String* strVal = dynamic_cast<String*>(args[0])) {
                string str = strVal->getValue();
                return new Bool(str == "true" || str == "1");
            }
        }
        return nullptr;
    }, {"value?"});
    
    // 注册布尔类型的方法
    registerMethod("toString", [](Value* instance, vector<Value*>& args) -> Value* {
        if (instance) {
            return new String(instance->toString());
        }
        return nullptr;
    });
    
    registerMethod("toBool", [](Value* instance, vector<Value*>& args) -> Value* {
        if (instance) {
            return new Bool(instance->toBool());
        }
        return nullptr;
    });
}

// NullType构造函数
NullType::NullType() : PrimitiveType("null", false) {
    // 注册空值类型的方法
    registerMethod("toString", [](Value* instance, vector<Value*>& args) -> Value* {
        if (instance) {
            return new String(instance->toString());
        }
        return nullptr;
    });
    
    registerMethod("toBool", [](Value* instance, vector<Value*>& args) -> Value* {
        if (instance) {
            return new Bool(instance->toBool());
        }
        return nullptr;
    });
}

// CharType构造函数
CharType::CharType() : NumericType("char", true) {
    // 注册构造函数作为静态方法
    registerStaticMethod("char", [](vector<Value*>& args) -> Value* {
        if (args.size() == 0) {
            return new Char('\0');  // 默认值
        } else if (args.size() == 1) {
            if (Char* charVal = dynamic_cast<Char*>(args[0])) {
                return new Char(charVal->getValue());
            } else if (Integer* intVal = dynamic_cast<Integer*>(args[0])) {
                return new Char((char)intVal->getValue());
            } else if (String* strVal = dynamic_cast<String*>(args[0])) {
                string str = strVal->getValue();
                return new Char(str.empty() ? '\0' : str[0]);
            }
        }
        return nullptr;
    }, {"value?"});
    
    // 注册字符类型的方法
    registerMethod("toString", [](Value* instance, vector<Value*>& args) -> Value* {
        if (instance) {
            return new String(instance->toString());
        }
        return nullptr;
    });
    
    registerMethod("toBool", [](Value* instance, vector<Value*>& args) -> Value* {
        if (instance) {
            return new Bool(instance->toBool());
        }
        return nullptr;
    });
    
    registerMethod("getValue", [](Value* instance, vector<Value*>& args) -> Value* {
        if (instance && dynamic_cast<Char*>(instance)) {
            Char* charInstance = dynamic_cast<Char*>(instance);
            return new Integer(charInstance->getValue());
        }
        return nullptr;
    });
}

// IntegerType构造函数
IntegerType::IntegerType() : NumericType("int", true) {
    // 注册构造函数作为静态方法
    registerStaticMethod("int", [](vector<Value*>& args) -> Value* {
        if (args.size() == 0) {
            return new Integer(0);  // 默认值
        } else if (args.size() == 1) {
            if (Integer* intVal = dynamic_cast<Integer*>(args[0])) {
                return new Integer(intVal->getValue());
            } else if (Double* doubleVal = dynamic_cast<Double*>(args[0])) {
                return new Integer((int)doubleVal->getValue());
            } else if (String* strVal = dynamic_cast<String*>(args[0])) {
                try {
                    return new Integer(stoi(strVal->getValue()));
                } catch (...) {
                    return new Integer(0);
                }
            }
        }
        return nullptr;
    }, {"value?"});
    
    // 注册整数类型的方法
    registerMethod("toString", [](Value* instance, vector<Value*>& args) -> Value* {
        if (instance) {
            return new String(instance->toString());
        }
        return nullptr;
    });
    
    registerMethod("toBool", [](Value* instance, vector<Value*>& args) -> Value* {
        if (instance) {
            return new Bool(instance->toBool());
        }
        return nullptr;
    });
    
    registerMethod("getValue", [](Value* instance, vector<Value*>& args) -> Value* {
        if (instance && dynamic_cast<Integer*>(instance)) {
            Integer* intInstance = dynamic_cast<Integer*>(instance);
            return new Integer(intInstance->getValue());
        }
        return nullptr;
    });
}

// DoubleType构造函数
DoubleType::DoubleType() : NumericType("double", true) {
    // 注册构造函数作为静态方法
    registerStaticMethod("double", [](vector<Value*>& args) -> Value* {
        if (args.size() == 0) {
            return new Double(0.0);  // 默认值
        } else if (args.size() == 1) {
            if (Double* doubleVal = dynamic_cast<Double*>(args[0])) {
                return new Double(doubleVal->getValue());
            } else if (Integer* intVal = dynamic_cast<Integer*>(args[0])) {
                return new Double((double)intVal->getValue());
            } else if (String* strVal = dynamic_cast<String*>(args[0])) {
                try {
                    return new Double(stod(strVal->getValue()));
                } catch (...) {
                    return new Double(0.0);
                }
            }
        }
        return nullptr;
    }, {"value?"});
    
    // 注册双精度浮点类型的方法
    registerMethod("toString", [](Value* instance, vector<Value*>& args) -> Value* {
        if (instance) {
            return new String(instance->toString());
        }
        return nullptr;
    });
    
    registerMethod("toBool", [](Value* instance, vector<Value*>& args) -> Value* {
        if (instance) {
            return new Bool(instance->toBool());
        }
        return nullptr;
    });
    
    registerMethod("getValue", [](Value* instance, vector<Value*>& args) -> Value* {
        if (instance && dynamic_cast<Double*>(instance)) {
            Double* doubleInstance = dynamic_cast<Double*>(instance);
            return new Double(doubleInstance->getValue());
        }
        return nullptr;
    });
}



// ==================== 容器类型构造函数实现 ====================

// StringType构造函数
StringType::StringType() : ContainerType("string", true) {
    // 注册字符串类型的基本方法
    registerMethod("toString", [](Value* instance, vector<Value*>& args) -> Value* {
        if (String* str = dynamic_cast<String*>(instance)) {
            return new String(str->getValue());
        }
        return nullptr;
    });
    
    registerMethod("toBool", [](Value* instance, vector<Value*>& args) -> Value* {
        return nullptr; // 具体实现将在运行时处理
    });
    
    registerMethod("length", [](Value* instance, vector<Value*>& args) -> Value* {
        if (String* str = dynamic_cast<String*>(instance)) {
            return new Integer(str->getValue().length());
        }
        return nullptr;
    });
    
    // 字符串常用操作
    registerMethod("substring", [](Value* instance, vector<Value*>& args) -> Value* {
        if (instance && args.size() >= 2) {
            if (String* str = dynamic_cast<String*>(instance)) {
                if (Integer* start = dynamic_cast<Integer*>(args[0])) {
                    if (Integer* end = dynamic_cast<Integer*>(args[1])) {
                        return str->substring(start->getValue(), end->getValue());
                    }
                }
            }
        }
        return nullptr;
    });
    
    registerMethod("indexOf", [](Value* instance, vector<Value*>& args) -> Value* {
        if (instance && args.size() >= 1) {
            if (String* str = dynamic_cast<String*>(instance)) {
                // 将参数转换为String对象
                String* substr = new String(args[0]->toString());
                int result = str->indexOf(*substr);
                delete substr;  // 清理临时对象
                return new Integer(result);
            }
        }
        return nullptr;
    });
    
    registerMethod("replace", [](Value* instance, vector<Value*>& args) -> Value* {
        if (instance && args.size() >= 2) {
            if (String* str = dynamic_cast<String*>(instance)) {
                if (String* oldStr = dynamic_cast<String*>(args[0])) {
                    if (String* newStr = dynamic_cast<String*>(args[1])) {
                        return str->replace(*oldStr, *newStr);
                    }
                }
            }
        }
        return nullptr;
    });
    
    registerMethod("split", [](Value* instance, vector<Value*>& args) -> Value* {
        if (instance && args.size() >= 1) {
            if (String* str = dynamic_cast<String*>(instance)) {
                if (String* delimiter = dynamic_cast<String*>(args[0])) {
                    vector<String*> splitResult = str->split(*delimiter);
                    // 转换为Array<Value*>格式
                    vector<Value*> result;
                    for (String* s : splitResult) {
                        result.push_back(s);
                    }
                    return new Array(result);
                }
            }
        }
        return nullptr;
    });
    
    registerMethod("trim", [](Value* instance, vector<Value*>& args) -> Value* {
        if (instance) {
            if (String* str = dynamic_cast<String*>(instance)) {
                return str->trim();
            }
        }
        return nullptr;
    });
    
    registerMethod("toUpper", [](Value* instance, vector<Value*>& args) -> Value* {
        if (instance) {
            if (String* str = dynamic_cast<String*>(instance)) {
                return str->toUpperCase();
            }
        }
        return nullptr;
    });
    
    registerMethod("toLower", [](Value* instance, vector<Value*>& args) -> Value* {
        if (instance) {
            if (String* str = dynamic_cast<String*>(instance)) {
                return str->toLowerCase();
            }
        }
        return nullptr;
    });
    
    registerMethod("startsWith", [](Value* instance, vector<Value*>& args) -> Value* {
        if (instance && args.size() >= 1) {
            if (String* str = dynamic_cast<String*>(instance)) {
                if (String* prefix = dynamic_cast<String*>(args[0])) {
                    return new Bool(str->startsWith(*prefix));
                }
            }
        }
        return nullptr;
    });
    
    registerMethod("endsWith", [](Value* instance, vector<Value*>& args) -> Value* {
        if (instance && args.size() >= 1) {
            if (String* str = dynamic_cast<String*>(instance)) {
                if (String* suffix = dynamic_cast<String*>(args[0])) {
                    return new Bool(str->endsWith(*suffix));
                }
            }
        }
        return nullptr;
    });
    
    registerMethod("contains", [](Value* instance, vector<Value*>& args) -> Value* {
        if (instance && args.size() >= 1) {
            if (String* str = dynamic_cast<String*>(instance)) {
                if (String* substr = dynamic_cast<String*>(args[0])) {
                    return new Bool(str->contains(*substr));
                }
            }
        }
        return nullptr;
    });
    
    registerMethod("isEmpty", [](Value* instance, vector<Value*>& args) -> Value* {
        if (instance) {
            if (String* str = dynamic_cast<String*>(instance)) {
                return new Bool(str->isEmpty());
            }
        }
        return nullptr;
    });
    
    registerMethod("lastIndexOf", [](Value* instance, vector<Value*>& args) -> Value* {
        if (instance && args.size() >= 1) {
            if (String* str = dynamic_cast<String*>(instance)) {
                // 将参数转换为String对象
                String* substr = new String(args[0]->toString());
                int result = str->lastIndexOf(*substr);
                delete substr;  // 清理临时对象
                return new Integer(result);
            }
        }
        return nullptr;
    });
    
    registerMethod("append", [](Value* instance, vector<Value*>& args) -> Value* {
        if (instance && args.size() >= 1) {
            if (String* str = dynamic_cast<String*>(instance)) {
                if (String* other = dynamic_cast<String*>(args[0])) {
                    str->append(*other);
                    return instance;  // 返回修改后的字符串
                } else if (Integer* intVal = dynamic_cast<Integer*>(args[0])) {
                    // 将整数转换为字符串后追加
                    str->append(intVal->toString());
                    return instance;
                } else if (Double* doubleVal = dynamic_cast<Double*>(args[0])) {
                    // 将浮点数转换为字符串后追加
                    str->append(doubleVal->toString());
                    return instance;
                } else if (Bool* boolVal = dynamic_cast<Bool*>(args[0])) {
                    // 将布尔值转换为字符串后追加
                    str->append(boolVal->toString());
                    return instance;
                } else {
                    // 其他类型转换为字符串后追加
                    str->append(args[0]->toString());
                    return instance;
                }
            }
        }
        return nullptr;
    });
    
    // ==================== 注册静态方法 ====================
    // 构造函数（创建字符串）
    registerStaticMethod("string", [](vector<Value*>& args) -> Value* {
        if (args.size() == 0) {
            return new String("");  // 空字符串
        } else if (args.size() == 1) {
            return new String(args[0]->toString());  // 从其他类型转换
        }
        return nullptr;
    }, {"value?"});
    
    // 从整数创建字符串
    registerStaticMethod("fromInt", [](vector<Value*>& args) -> Value* {
        if (args.size() != 1) return nullptr;
        if (Integer* intVal = dynamic_cast<Integer*>(args[0])) {
            return new String(to_string(intVal->getValue()));
        }
        return nullptr;
    }, {"int"});
    
    // 从浮点数创建字符串
    registerStaticMethod("fromDouble", [](vector<Value*>& args) -> Value* {
        if (args.size() != 1) return nullptr;
        if (Double* doubleVal = dynamic_cast<Double*>(args[0])) {
            return new String(to_string(doubleVal->getValue()));
        }
        return nullptr;
    });
    
    // 从布尔值创建字符串
    registerStaticMethod("fromBool", [](vector<Value*>& args) -> Value* {
        if (args.size() != 1) return nullptr;
        if (Bool* boolVal = dynamic_cast<Bool*>(args[0])) {
            return new String(boolVal->getValue() ? "true" : "false");
        }
        return nullptr;
    });
    
    // 创建重复字符串
    registerStaticMethod("repeat", [](vector<Value*>& args) -> Value* {
        if (args.size() != 2) return nullptr;
        if (String* str = dynamic_cast<String*>(args[0])) {
            if (Integer* count = dynamic_cast<Integer*>(args[1])) {
                string result;
                for (int i = 0; i < count->getValue(); i++) {
                    result += str->getValue();
                }
                return new String(result);
            }
        }
        return nullptr;
    });
}

// ArrayType构造函数
ArrayType::ArrayType() : ContainerType("array", true) {
    // 注册构造函数作为静态方法
    registerStaticMethod("array", [](vector<Value*>& args) -> Value* {
        // 创建Array对象，传入元素列表
        return new Array(args);
    }, {"..."});
    
    // 注册数组类型的基本方法
    registerMethod("toString", [](Value* instance, vector<Value*>& args) -> Value* {
        String* str = new String("[");
        if (Array* array = dynamic_cast<Array*>(instance)) {
            for (int i = 0; i < array->getSize(); i++) {
                if (i > 0) {
                    str->append(",");
                }
                str->append(array->getElement(i)->toString());
            }
        }
        str->append("]");
        return str;
    });
    
    registerMethod("toBool", [](Value* instance, vector<Value*>& args) -> Value* {
        return nullptr; // 具体实现将在运行时处理
    });
    
    registerMethod("size", [](Value* instance, vector<Value*>& args) -> Value* {
        if (Array* array = dynamic_cast<Array*>(instance)) {
            return new Integer(array->getSize());
        }
        return nullptr;
    });
    
    // 数组常用操作
    registerMethod("add", [](Value* instance, vector<Value*>& args) -> Value* {
        if (instance && args.size() >= 1) {
            if (Array* array = dynamic_cast<Array*>(instance)) {
                array->addElement(args[0]);
                return instance;
            }
        }
        return nullptr;
    });
    
    registerMethod("remove", [](Value* instance, vector<Value*>& args) -> Value* {
        if (instance && args.size() >= 1) {
            if (Array* array = dynamic_cast<Array*>(instance)) {
                if (Integer* index = dynamic_cast<Integer*>(args[0])) {
                    array->removeElement(index->getValue());
                    return instance;
                }
            }
        }
        return nullptr;
    });
    
    registerMethod("get", [](Value* instance, vector<Value*>& args) -> Value* {
        if (instance && args.size() >= 1) {
            if (Array* array = dynamic_cast<Array*>(instance)) {
                if (Integer* index = dynamic_cast<Integer*>(args[0])) {
                    return array->getElement(index->getValue());
                }
            }
        }
        return nullptr;
    });
    
    registerMethod("set", [](Value* instance, vector<Value*>& args) -> Value* { 
        if (instance && args.size() >= 2) {
            if (Array* array = dynamic_cast<Array*>(instance)) {
                if (Integer* index = dynamic_cast<Integer*>(args[0])) {
                    array->setElement(index->getValue(), args[1]);
                    return instance;
                }
            }
        }
        return nullptr;
    });
    
    registerMethod("clear", [](Value* instance, vector<Value*>& args) -> Value* {
        if (instance) {
            if (Array* array = dynamic_cast<Array*>(instance)) {
                array->clear();
                return instance;
            }
        }
        return nullptr;
    });
    
    registerMethod("isEmpty", [](Value* instance, vector<Value*>& args) -> Value* {
        if (instance) {
            if (Array* array = dynamic_cast<Array*>(instance)) {
                return new Bool(array->isEmpty());
            }
        }
        return nullptr;
    });
    
    registerMethod("contains", [](Value* instance, vector<Value*>& args) -> Value* {
        if (instance && args.size() >= 1) {
            if (Array* array = dynamic_cast<Array*>(instance)) {
                // 检查数组是否包含指定元素
                const vector<Value*>& elements = array->getElements();
                for (Value* elem : elements) {
                    if (elem && elem->toString() == args[0]->toString()) {
                        return new Bool(true);
                    }
                }
                return new Bool(false);
            }
        }
        return nullptr;
    });
    
    registerMethod("indexOf", [](Value* instance, vector<Value*>& args) -> Value* {
        if (instance && args.size() >= 1) {
            if (Array* array = dynamic_cast<Array*>(instance)) {
                // 直接调用Array类的indexOf方法
                return array->indexOf(args[0]);
            }
        }
        return nullptr;
    });
    
    registerMethod("sort", [](Value* instance, vector<Value*>& args) -> Value* {
        if (instance) {
            if (Array* array = dynamic_cast<Array*>(instance)) {
                // 调用Array类的sort方法
                return array->sort();
            }
        }
        return nullptr;
    });
    
    registerMethod("reverse", [](Value* instance, vector<Value*>& args) -> Value* {
        if (instance) {
            if (Array* array = dynamic_cast<Array*>(instance)) {
                // 调用Array类的reverse方法
                return array->reverse();
            }
        }
        return nullptr;
    });
}

// DictType构造函数
DictType::DictType() : ContainerType("dict", true) {
    // 注册构造函数作为静态方法
    registerStaticMethod("dict", [](vector<Value*>& args) -> Value* {
        // 创建Dict对象，传入键值对列表
        // args格式：[key1, value1, key2, value2, ...]
        Dict* dict = new Dict();
        
        for (size_t i = 0; i < args.size(); i += 2) {
            if (i + 1 < args.size()) {
                string key;
                if (String* strKey = dynamic_cast<String*>(args[i])) {
                    key = strKey->getValue();
                } else {
                    // 如果不是字符串，转换为字符串
                    key = args[i] ? args[i]->toString() : "null";
                }
                dict->setEntry(key, args[i + 1] ? args[i + 1]->clone() : new Null());
            }
        }
        
        return dict;
    }, {"..."});
    
    // 注册字典类型的基本方法
    registerMethod("toString", [](Value* instance, vector<Value*>& args) -> Value* {
        if (instance) {
            return new String(instance->toString());
        }
        return nullptr;
    });
    
    registerMethod("toBool", [](Value* instance, vector<Value*>& args) -> Value* {
        if (instance) {
            return new Bool(dynamic_cast<Dict*>(instance)->toBool());
        }
        return nullptr;
    });
    
    registerMethod("size", [](Value* instance, vector<Value*>& args) -> Value* {
        if (instance && dynamic_cast<Dict*>(instance)) {
            return new Integer(dynamic_cast<Dict*>(instance)->getSize());
        }
        return nullptr;
    });
    
    // 字典常用操作
    registerMethod("put", [](Value* instance, vector<Value*>& args) -> Value* {
        if (instance && dynamic_cast<Dict*>(instance) && args.size() >= 2) {
            Dict* dictInstance = dynamic_cast<Dict*>(instance);
            if (String* key = dynamic_cast<String*>(args[0])) {
                dictInstance->setEntry(key->getValue(), args[1]);
                return instance;
            }
        }
        return nullptr;
    });
    
    registerMethod("get", [](Value* instance, vector<Value*>& args) -> Value* {
        if (instance && dynamic_cast<Dict*>(instance) && args.size() >= 1) {
            Dict* dictInstance = dynamic_cast<Dict*>(instance);
            if (String* key = dynamic_cast<String*>(args[0])) {
                return dynamic_cast<Dict*>(instance)->getEntry(key->getValue());
            }
        }
        return nullptr;
    });
    
    registerMethod("remove", [](Value* instance, vector<Value*>& args) -> Value* {
        if (instance && dynamic_cast<Dict*>(instance) && args.size() >= 1) {
            Dict* dictInstance = dynamic_cast<Dict*>(instance);
            if (String* key = dynamic_cast<String*>(args[0])) {
                dictInstance->removeEntry(key->getValue());
                return instance;
            }
        }
        return nullptr;
    });
    
    registerMethod("clear", [](Value* instance, vector<Value*>& args) -> Value* {
        if (instance && dynamic_cast<Dict*>(instance)) {
            Dict* dictInstance = dynamic_cast<Dict*>(instance);
            dictInstance->clear();
            return instance;
        }
        return nullptr;
    });
    
    registerMethod("isEmpty", [](Value* instance, vector<Value*>& args) -> Value* {
        if (instance && dynamic_cast<Dict*>(instance)) {
            Dict* dictInstance = dynamic_cast<Dict*>(instance);
            return new Bool(dictInstance->isEmpty());
        }
        return nullptr;
    });
    
    registerMethod("containsKey", [](Value* instance, vector<Value*>& args) -> Value* {
        if (instance && dynamic_cast<Dict*>(instance) && args.size() >= 1) {
            Dict* dictInstance = dynamic_cast<Dict*>(instance);
            if (String* key = dynamic_cast<String*>(args[0])) {
                return new Bool(dictInstance->hasKey(key->getValue()));
            }
        }
        return nullptr;
    });
    
    registerMethod("containsValue", [](Value* instance, vector<Value*>& args) -> Value* {
        if (instance && args.size() >= 1) {
            if (Dict* dict = dynamic_cast<Dict*>(instance)) {
                // 检查字典是否包含指定值
                const vector<Value*>& values = dict->getValues();
                for (Value* val : values) {
                    if (val && val->toString() == args[0]->toString()) {
                        return new Bool(true);
                    }
                }
                return new Bool(false);
            }
        }
        return nullptr;
    });
    
    registerMethod("keys", [](Value* instance, vector<Value*>& args) -> Value* {
        if (instance && dynamic_cast<Dict*>(instance)) {
            Dict* dictInstance = dynamic_cast<Dict*>(instance);
            vector<string> keys = dictInstance->getKeys();
            vector<Value*> keyValues;
            for (const string& key : keys) {
                keyValues.push_back(new String(key));
            }
            return new Array(keyValues);
        }
        return nullptr;
    });
    
    registerMethod("values", [](Value* instance, vector<Value*>& args) -> Value* {
        if (instance && dynamic_cast<Dict*>(instance)) {
            Dict* dictInstance = dynamic_cast<Dict*>(instance);
            vector<Value*> values = dictInstance->getValues();
            return new Array(values);
        }
        return nullptr;
    });
    
    registerMethod("items", [](Value* instance, vector<Value*>& args) -> Value* {
        if (instance) {
            if (Dict* dict = dynamic_cast<Dict*>(instance)) {
                // 实现items逻辑：返回键值对数组
                const map<string, Value*>& entries = dict->getEntries();
                vector<Value*> items;
                
                for (const auto& pair : entries) {
                    // 创建键值对数组 [key, value]
                    vector<Value*> pairArray = {
                        new String(pair.first),
                        pair.second ? pair.second->clone() : new Null()
                    };
                    items.push_back(new Array(pairArray));
                }
                
                return new Array(items);
            }
        }
        return nullptr;
    });
}

// ==================== 自动注册内置类型 ====================
// 在main()之前自动注册所有内置类型
// 注意：必须放在所有类型构造函数实现之后

// 注册基本类型（继承自PrimitiveType）
REGISTER_BUILTIN_TYPE(bool, BoolType)      // BoolType -> PrimitiveType -> ObjectType
REGISTER_BUILTIN_TYPE(null, NullType)      // NullType -> PrimitiveType -> ObjectType

// 注册数值类型（继承自NumericType）
REGISTER_BUILTIN_TYPE(char, CharType)      // CharType -> NumericType -> PrimitiveType -> ObjectType
REGISTER_BUILTIN_TYPE(int, IntegerType)    // IntegerType -> NumericType -> PrimitiveType -> ObjectType
REGISTER_BUILTIN_TYPE(double, DoubleType)  // DoubleType -> NumericType -> PrimitiveType -> ObjectType

// 注册容器类型（继承自ContainerType）
REGISTER_BUILTIN_TYPE(string, StringType)  // StringType -> ContainerType -> ObjectType
REGISTER_BUILTIN_TYPE(array, ArrayType)    // ArrayType -> ContainerType -> ObjectType
REGISTER_BUILTIN_TYPE(dict, DictType)      // DictType -> ContainerType -> ObjectType

// 添加调试信息，验证注册是否成功
void debugTypeRegistration() {
    TypeRegistry* globalRegistry = TypeRegistry::getGlobalInstance();
    if (globalRegistry) {
        cout << "=== Debug: Built-in Types Registration ===" << endl;
        cout << "Global registry has " << globalRegistry->types.size() << " types:" << endl;
        for (const auto& pair : globalRegistry->types) {
            cout << "  - " << pair.first << " -> " << pair.second->getTypeName() << endl;
        }
        cout << "=========================================" << endl;
    }
}

// 在程序启动时调用调试函数
static bool debugInitialized = false;
void ensureDebugInitialized() {
    if (!debugInitialized) {
        debugTypeRegistration();
        debugInitialized = true;
    }
}





// ==================== 显式初始化函数 ====================

// 统一的动态库初始化函数
extern "C" void initialize_interpreter_types() {
    std::cout << "Initializing interpreter types..." << std::endl;
    
    // 调用所有类型的显式初始化函数
    register_bool_type_explicit();
    register_null_type_explicit();
    register_char_type_explicit();
    register_int_type_explicit();
    register_double_type_explicit();
    register_string_type_explicit();
    register_array_type_explicit();
    register_dict_type_explicit();
    
    std::cout << "Interpreter types initialization completed." << std::endl;
    
    // 显示调试信息
    debugTypeRegistration();
}



// TypeRegistry的静态初始化函数
void TypeRegistry::initializeBuiltinTypes() {
    initialize_interpreter_types();
}
