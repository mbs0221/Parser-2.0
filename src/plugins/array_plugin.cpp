#include "interpreter/builtin_plugin.h"
#include "lexer/value.h"
#include "parser/function.h"

#include <iostream>
#include <algorithm>

using namespace std;

// ==================== 数组操作函数 ====================

Value* array_push(vector<Variable*>& args) {
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

Value* array_pop(vector<Variable*>& args) {
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

Value* array_sort(vector<Variable*>& args) {
    if (args.size() != 1 || !args[0]) return nullptr;
    
    Value* arrVal = args[0]->getValue();
    if (Array* arr = dynamic_cast<Array*>(arrVal)) {
        // 简单的排序实现
        // 这里可以添加实际的排序逻辑
        return arr;
    }
    
    return nullptr;
}

Value* array_length(vector<Variable*>& args) {
    if (args.size() != 1 || !args[0]) return nullptr;
    
    Value* val = args[0]->getValue();
    if (Array* arr = dynamic_cast<Array*>(val)) {
        return new Integer(arr->size());
    }
    return nullptr;
}

// 数组操作插件类
class ArrayPlugin : public BuiltinPlugin {
public:
    PluginInfo getPluginInfo() const override {
        return PluginInfo{
            "array",
            "1.0.0",
            "数组操作插件，包含push、pop、sort、length等数组操作函数",
            {"push", "pop", "sort", "length"}
        };
    }
    
    void registerFunctions(ScopeManager& scopeManager) override {
        // 使用辅助方法批量注册函数
        defineBuiltinFunctions(scopeManager, getFunctionMap());
    }
    
    map<string, BuiltinFunctionPtr> getFunctionMap() const override {
        return {
            {"push", array_push},
            {"pop", array_pop},
            {"sort", array_sort},
            {"length", array_length}
        };
    }
};

// 导出插件
EXPORT_PLUGIN(ArrayPlugin)
