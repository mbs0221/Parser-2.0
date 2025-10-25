#include "interpreter/types/types.h"
#include "interpreter/values/value.h"
#include <stdexcept>

using namespace std;

// ==================== ContainerType实现 ====================
ContainerType::ContainerType(const string& typeName, bool isPrimitive, bool isMutable, bool isReference)
    : ObjectType(typeName, isPrimitive, isMutable, isReference, true) {
    // 容器类型自动注册下标访问方法
    // 注意：只有数组类型才真正支持下标访问
    registerSubscriptAccess();
}

bool ContainerType::supportsAccess() const {
    return true;
}

bool ContainerType::supportsIteration() const {
    return true;
}

bool ContainerType::supportsSizeQuery() const {
    return true;
}

void ContainerType::registerSubscriptAccess() {
    // 为容器类型注册下标访问方法 - 使用addUserMethod
    auto subscriptMethod = [this](Scope* scope) -> Value* {
        // 从scope中获取this和index参数
        Value* instance = scope->getVariable<Value>("this");
        Value* index = scope->getVariable<Value>("index");
        
        if (instance && index) {
            // 检查实例是否为数组类型
            if (Array* array = dynamic_cast<Array*>(instance)) {
                // 数组类型：使用数字索引进行下标访问
                if (Integer* intIndex = dynamic_cast<Integer*>(index)) {
                    return array->operator[](*intIndex);
                } else {
                    throw runtime_error("Array index must be an integer");
                }
            }
            // 检查实例是否为字典类型
            else if (Dict* dict = dynamic_cast<Dict*>(instance)) {
                // 字典类型：使用键进行下标访问
                string key = index->toString();
                return dict->getEntry(key);
            }
            // 其他容器类型不支持下标访问
            throw runtime_error("Subscript access only supported for array and dict types");
        }
        return nullptr;
    };
    
    // 使用函数原型构造函数创建BuiltinFunction对象
    BuiltinFunction* subscriptFunc = new BuiltinFunction(subscriptMethod, "operator[](index:any)");
    addUserMethod(subscriptFunc, VIS_PUBLIC);
}

Value* ContainerType::createValueWithArgs(const vector<Value*>& args) {
    // 默认实现：忽略参数，创建默认值
    return createDefaultValue();
}

bool ContainerType::isCompatibleWith(ObjectType* other) const {
    // 容器类型的基本兼容性检查
    if (!other) return false;
    
    // 如果类型名称相同，则兼容
    if (getTypeName() == other->getTypeName()) return true;
    
    // 检查是否为同一类型
    if (this == other) return true;
    
    // 检查是否为父类型
    if (ObjectType* parent = getParentType()) {
        return parent->isCompatibleWith(other);
    }
    
    return false;
}