#include "interpreter/plugins/builtin_plugin.h"
#include "interpreter/values/value.h"
#include "interpreter/types/types.h"
#include "parser/definition.h"
#include <iostream>
#include <algorithm>

using namespace std;

// ==================== 数组操作函数实现 ====================

// 手动实现数组操作函数
Value* array_push(vector<Value*>& args) {
    if (args.size() < 2 || !args[0] || !args[1]) return nullptr;
    
    Array* array = dynamic_cast<Array*>(args[0]);
    if (!array) return nullptr;
    
    array->addElement(args[1]);
    return array;
}

Value* array_pop(vector<Value*>& args) {
    if (args.size() != 1 || !args[0]) return nullptr;
    
    Array* array = dynamic_cast<Array*>(args[0]);
    if (!array || array->getSize() == 0) return nullptr;
    
    size_t lastIndex = array->getSize() - 1;
    Value* lastElement = array->getElement(lastIndex);
    array->removeElement(lastIndex);
    return lastElement;
}

Value* array_sort(vector<Value*>& args) {
    if (args.size() != 1 || !args[0]) return nullptr;
    
    Array* array = dynamic_cast<Array*>(args[0]);
    if (!array) return nullptr;
    
    // 简单的冒泡排序实现
    for (size_t i = 0; i < array->getSize(); i++) {
        for (size_t j = 0; j < array->getSize() - i - 1; j++) {
            Value* current = array->getElement(j);
            Value* next = array->getElement(j + 1);
            if (current && next && current->toString() > next->toString()) {
                array->setElement(j, next);
                array->setElement(j + 1, current);
            }
        }
    }
    return array;
}

Value* array_length(vector<Value*>& args) {
    if (args.size() != 1 || !args[0]) return nullptr;
    
    Array* array = dynamic_cast<Array*>(args[0]);
    if (!array) return nullptr;
    
    return new Integer(array->getSize());
}

// 数组操作插件类
class ArrayPlugin : public BuiltinPlugin {
public:
    PluginInfo getPluginInfo() const override {
        return PluginInfo{
            "array",
            "1.0.0",
            "数组操作插件，通过类型系统调用数组操作方法",
            {"push", "pop", "sort", "length"}
        };
    }
    
    std::vector<FunctionInfo> getFunctions() override {
        std::vector<FunctionInfo> functions;
        
        // 添加数组操作函数
        functions.emplace_back("push", "push(array, value)", 
            std::vector<std::string>{"array", "value"},
            [](Scope* scope) -> Value* {
                Array* array = scope->getArgument<Array>("array");
                Value* value = scope->getArgument<Value>("value");
                if (array && value) {
                    vector<Value*> args = {array, value};
                    return array_push(args);
                }
                return nullptr;
            });
            
        functions.emplace_back("pop", "pop(array)", 
            std::vector<std::string>{"array"},
            [](Scope* scope) -> Value* {
                Array* array = scope->getArgument<Array>("array");
                if (array) {
                    vector<Value*> args = {array};
                    return array_pop(args);
                }
                return nullptr;
            });
            
        functions.emplace_back("sort", "sort(array)", 
            std::vector<std::string>{"array"},
            [](Scope* scope) -> Value* {
                Array* array = scope->getArgument<Array>("array");
                if (array) {
                    vector<Value*> args = {array};
                    return array_sort(args);
                }
                return nullptr;
            });
            
        functions.emplace_back("length", "length(array)", 
            std::vector<std::string>{"array"},
            [](Scope* scope) -> Value* {
                Array* array = scope->getArgument<Array>("array");
                if (array) {
                    vector<Value*> args = {array};
                    return array_length(args);
                }
                return nullptr;
            });
            
        return functions;
    }
};

// 导出插件
EXPORT_PLUGIN(ArrayPlugin)
