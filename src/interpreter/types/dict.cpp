#include <algorithm>
#include <cmath>
#include <cctype>

#include "interpreter/values/value.h"
#include "interpreter/types/types.h"
#include "interpreter/utils/logger.h"
#include "interpreter/scope/scope.h"
#include "lexer/token.h"

using namespace std;

// 定义静态方法数组
static const vector<Function*> dictStaticMethods = {
    // 注册构造函数作为静态方法 - 使用新的kwargs机制
    new BuiltinFunction([](class Scope* scope) -> Value* {
        // 创建Dict对象
        Dict* dict = new Dict();
        
        // 处理kwargs参数（新的可变参数机制）
        if (scope->hasKwargs()) {
            Dict* kwargs = scope->getKwargs();
            if (kwargs) {
                // 从kwargs中获取所有键值对
                vector<string> keys = kwargs->getKeys();
                for (const string& key : keys) {
                    Value* value = kwargs->getEntry(key);
                    if (value) {
                        dict->setEntry(key, value->clone());
                        LOG_DEBUG("Dict: from kwargs - key: " + key + ", value: " + value->toString());
                    }
                }
            }
        }
        
        // 保持向后兼容：处理传统的argc/args方式
        if (scope->getArgument<Value>("argc") && scope->getArgument<Value>("args")) {
            int argc = scope->getArgument<Integer>("argc")->getValue();
            Array* args = scope->getArgument<Array>("args");
            LOG_DEBUG("Dict: processing " + to_string(argc) + " arguments using legacy va_arg style");
            
            // 每两个参数组成一个键值对
            for (int i = 0; i < argc; i += 2) {
                if (i + 1 < argc) {
                    Value* key = args->getElement(i);
                    Value* value = args->getElement(i + 1);
                    
                    if (key && value) {
                        string keyStr = key->toString();
                        LOG_DEBUG("Dict: from args - key: " + keyStr + ", value: " + value->toString());
                        dict->setEntry(keyStr, value->clone());
                    }
                }
            }
        }
        
        LOG_DEBUG("Dict: created with " + to_string(dict->getSize()) + " entries");
        return dict;
    }, "dict(...)"),
};

// 定义类方法数组
static const vector<Function*> dictClassMethods = {
    // 注册字典类型的基本方法
    new BuiltinFunction([](class Scope* scope) -> Value* {
        if (Dict* dict = scope->getThis<Dict>()) {
            return new String(dict->toString());
        }
        return nullptr;
    }, "toString()"),
    
    new BuiltinFunction([](class Scope* scope) -> Value* {
        if (Dict* dict = scope->getThis<Dict>()) {
            return new Bool(dict->toBool());
        }
        return nullptr;
    }, "toBool()"),
    
    new BuiltinFunction([](class Scope* scope) -> Value* {
        if (Dict* dict = scope->getThis<Dict>()) {
            return new Integer(dict->getSize());
        }
        return nullptr;
    }, "size()"),
    
    // 字典常用操作
    new BuiltinFunction([](class Scope* scope) -> Value* {
        if (Dict* dict = scope->getThis<Dict>()) {
            if (String* key = dynamic_cast<String*>(scope->getArgument<String>("key"))) {
                dict->setEntry(key->getValue(), scope->getArgument<Value>("value"));
                return scope->getThis<Dict>();
            }
        }
        return nullptr;
    }, "put(key, value)"),
    
    new BuiltinFunction([](class Scope* scope) -> Value* {
        if (Dict* dict = scope->getThis<Dict>()) {
            if (String* key = dynamic_cast<String*>(scope->getArgument<String>("key"))) {
                return dict->getEntry(key->getValue());
            }
        }
        return nullptr; 
    }, "get(key)"),
    
    new BuiltinFunction([](class Scope* scope) -> Value* {
        if (Dict* dict = scope->getThis<Dict>()) {
            if (String* key = dynamic_cast<String*>(scope->getArgument<String>("key"))) {
                dict->removeEntry(key->getValue());
                return scope->getThis<Dict>();
            }
        }
        return nullptr;
    }, "remove(key)"),
    
    new BuiltinFunction([](class Scope* scope) -> Value* {
        if (Dict* dict = scope->getThis<Dict>()) {
            dict->clear();
            return scope->getThis<Dict>();
        }
        return nullptr;
    }, "clear()"),
    
    new BuiltinFunction([](class Scope* scope) -> Value* {
        if (Dict* dict = scope->getThis<Dict>()) {
            return new Bool(dict->isEmpty());
        }
        return nullptr;
    }, "isEmpty()"),
    
    new BuiltinFunction([](class Scope* scope) -> Value* {
        if (Dict* dict = scope->getThis<Dict>()) {
            if (String* key = dynamic_cast<String*>(scope->getArgument<String>("key"))) {
                return new Bool(dict->hasKey(key->getValue()));
            }
        }
        return nullptr;
    }, "containsKey(key)"),
    
    new BuiltinFunction([](class Scope* scope) -> Value* {
        if (Dict* dict = scope->getThis<Dict>()) {
            if (String* value = scope->getArgument<String>("value")) {
                // 检查字典是否包含指定值
                const vector<Value*>& values = dict->getValues();
                for (Value* val : values) {
                    if (val && val->toString() == value->getValue()) {
                        return new Bool(true);
                    }
                }
                return new Bool(false);
            }
        }
        return nullptr;
    }, "containsValue(value)"),
    
    new BuiltinFunction([](class Scope* scope) -> Value* {
        if (Dict* dict = scope->getThis<Dict>()) {
            vector<string> keys = dict->getKeys();
            Array* keyValues = new Array();
            for (const string& key : keys) {
                keyValues->addElement(new String(key));
            }
            return keyValues;
        }
        return nullptr;
    }, "keys()"),
    
    new BuiltinFunction([](class Scope* scope) -> Value* {
        if (Dict* dict = scope->getThis<Dict>()) {
            vector<Value*> values = dict->getValues();
            Array* valueValues = new Array();
            for (Value* value : values) {
                valueValues->addElement(value);
            }
            return valueValues;
        }
        return nullptr;
    }, "values()"),
    
    new BuiltinFunction([](class Scope* scope) -> Value* {
        if (Dict* dict = scope->getThis<Dict>()) {
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
        return nullptr;
    }, "items()"),
};

// DictType构造函数
DictType::DictType() : ClassType("dict", true) {
    // 批量注册类方法
    for (Function* method : dictClassMethods) {
        addUserMethod(method);
    }
    
    // 批量注册静态方法
    for (Function* method : dictStaticMethods) {
        addStaticMethod(method);
    }
}