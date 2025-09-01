#include "interpreter/scope/scope.h"
#include "interpreter/values/value.h"
#include "interpreter/types/types.h"
#include <iostream>
#include <memory>

using namespace std;

// ==================== Scope实现 ====================
// Scope的实现现在在头文件中，因为使用了unique_ptr

// 类型管理方法已移除 - 类型管理由TypeRegistry统一负责
// createTypeFromString函数已移动到TypeRegistry中

// Scope构造函数
Scope::Scope() : thisPointer(nullptr), currentClassContext(nullptr) {
    // 创建类型管理器和对象管理器
    typeRegistry = std::make_unique<TypeRegistry>();
    objectRegistry = std::make_unique<ObjectRegistry>();
}

// Scope析构函数
Scope::~Scope() {
    cleanup();
}

// 清理作用域中的资源
void Scope::cleanup() {
    // unique_ptr会自动管理内存，这里只需要清理其他资源
    thisPointer = nullptr;
    currentClassContext = nullptr;
}

// 打印作用域内容（用于调试）
void Scope::print() const {
    cout << "=== Scope Content ===" << endl;
    
    if (typeRegistry) {
        cout << "Type Registry: " << typeRegistry->getTypeCount() << " types" << endl;
    }
    
    if (objectRegistry) {
        cout << "Object Registry: " << endl;
        // 这里可以添加更多对象信息的打印
    }
    
    if (thisPointer) {
        cout << "This Pointer: " << thisPointer->toString() << endl;
    }
    
    if (currentClassContext) {
        cout << "Current Class: " << currentClassContext->getTypeName() << endl;
    }
}

// 获取类型管理器
TypeRegistry* Scope::getTypeRegistry() const {
    return typeRegistry.get();
}

// 获取对象管理器
ObjectRegistry* Scope::getObjectRegistry() const {
    return objectRegistry.get();
}




