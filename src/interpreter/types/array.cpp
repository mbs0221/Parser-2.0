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
static const vector<Function*> arrayStaticMethods = {
    new BuiltinFunction([](class Scope* scope) -> Value* {
        cout << "ArrayType::array() called" << endl;
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
                return array;
            }
        }
        
        // 如果没有可变参数，返回空数组
        cout << "ArrayType::array() called: nullptr" << endl;
        return array;
    }, "array(...)")
};

// 定义类方法数组
static const vector<Function*> arrayClassMethods = {
    new BuiltinFunction([](class Scope* scope) -> Value* {
        String* str = new String("[");
        if (Array* array = scope->getThis<Array>()) {
            for (int i = 0; i < array->getSize(); i++) {
                if (i > 0) {
                    str->append(",");
                }
                str->append(array->getElement(i)->toString());
            }
        }
        str->append("]");
        return str;
    }, "toString()"),
    
    new BuiltinFunction([](class Scope* scope) -> Value* {
        if (Array* array = scope->getThis<Array>()) {
            return new Bool(array->toBool());
        }
        return nullptr;
    }, "toBool()"),
    
    new BuiltinFunction([](class Scope* scope) -> Value* {
        if (Array* array = scope->getThis<Array>()) {
            return new Integer(array->getSize());
        }
        return nullptr;
    }, "size()"),
    
    new BuiltinFunction([](class Scope* scope) -> Value* {
        if (Array* array = scope->getThis<Array>()) {
            if (Value* element = scope->getArgument<Value>("element")) {
                array->addElement(scope->getArgument<Value>("element"));
                return scope->getThis<Array>();
            }
        }
        return nullptr;
    }, "add(element)"),
    
    new BuiltinFunction([](class Scope* scope) -> Value* {
        if (Array* array = scope->getThis<Array>()) {
            if (Integer* index = scope->getArgument<Integer>("index")) {
                array->removeElement(index->getValue());
                return scope->getThis<Array>();
            }
        }
        return nullptr;
    }, "remove(index)"),
    
    new BuiltinFunction([](class Scope* scope) -> Value* {
        if (Array* array = scope->getThis<Array>()) {
            if (Integer* index = scope->getArgument<Integer>("index")) {
                return array->getElement(index->getValue());
            }
        }
        return nullptr;
    }, "get(index)"),
    
    new BuiltinFunction([](class Scope* scope) -> Value* { 
        if (Array* array = scope->getThis<Array>()) {
            if (Integer* index = scope->getArgument<Integer>("index")) {
                if (Value* element = scope->getArgument<Value>("element")) {
                    array->setElement(index->getValue(), element);
                    return scope->getThis<Array>();
                }
            }
        }
        return nullptr;
    }, "set(index, element)"),
    
    new BuiltinFunction([](class Scope* scope) -> Value* {
        if (Array* array = scope->getThis<Array>()) {
            array->clear();
            return scope->getThis<Array>();
        }
        return nullptr;
    }, "clear()"),
    
    new BuiltinFunction([](class Scope* scope) -> Value* {
        if (Array* array = scope->getThis<Array>()) {
            return new Bool(array->isEmpty());
        }
        return nullptr;
    }, "isEmpty()"),
    
    new BuiltinFunction([](class Scope* scope) -> Value* {
        if (Array* array = scope->getThis<Array>()) {
            return new Bool(array->contains(scope->getArgument<Value>("element")));
        }
        return nullptr;
    }, "contains(element)"),
    
    new BuiltinFunction([](class Scope* scope) -> Value* {
        if (Array* array = scope->getThis<Array>()) {
            if (Value* element = scope->getArgument<Value>("element")) {
                // 检查数组是否包含指定元素
                const vector<Value*>& elements = array->getElements();
                for (Value* elem : elements) {
                    if (elem && elem->toString() == element->toString()) {
                        return new Bool(true);
                    }
                }
                return new Bool(false);
            }
        }
        return nullptr;
    }, "indexOf(element)"),
    
    new BuiltinFunction([](class Scope* scope) -> Value* {
        if (Array* array = scope->getThis<Array>()) {
            // 调用Array类的sort方法
            return array->sort();
        }
        return scope->getThis<Array>();
    }, "sort()"),
    
    new BuiltinFunction([](class Scope* scope) -> Value* {
        if (Array* array = scope->getThis<Array>()) {
            // 调用Array类的reverse方法
            return array->reverse();
        }
        return scope->getThis<Array>();
    }, "reverse()")
};

// ArrayType构造函数
ArrayType::ArrayType() : ClassType("array", true) {
    cout << "ArrayType::ArrayType() called" << endl;
    // 批量注册静态方法
    for (Function* method : arrayStaticMethods) {
        addStaticMethod(method);
    }
    
    // 批量注册类方法
    for (Function* method : arrayClassMethods) {
        addUserMethod(method);
    }
}