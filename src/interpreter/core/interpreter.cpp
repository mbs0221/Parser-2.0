// ==================== 标准库头文件 ====================
#include <iostream>
#include <sstream>
#include <typeinfo>
#include <map>
#include <functional>
#include <cstdio>
#include <cstdlib>

// ==================== 项目头文件 ====================
// 1. 当前源文件对应的头文件
#include "interpreter/core/interpreter.h"

// 2. 项目核心头文件
#include "interpreter/values/value.h"
#include "interpreter/types/types.h"

// 3. 项目功能头文件
#include "interpreter/utils/logger.h"
#include "interpreter/utils/string_utils.h"

// 4. 解析器头文件（放在最后，避免命名空间冲突）
#include "parser/expression.h"
#include "parser/definition.h"
#include "parser/inter.h"

using namespace std;

// 解释器构造函数
Interpreter::Interpreter() {
    // 初始化组件
    typeRegistry = TypeRegistry::getGlobalInstance();
    
    // 使用简化构造函数 - 只需要TypeRegistry
    objectFactory = new ObjectFactory(typeRegistry);
    
    // 初始化计算器实例
    calculator = new Calculator(this);
    
    // 创建初始作用域
    scopeManager.enterScope();
    
    // 注册所有内置插件
    pluginManager.registerAllPlugins(scopeManager);
    
    LOG_DEBUG("Interpreter initialized successfully");
}

// 解释器构造函数（可选择是否加载插件）
Interpreter::Interpreter(bool loadPlugins) {
    // 初始化类型系统 - 使用全局注册表实例
    typeRegistry = TypeRegistry::getGlobalInstance();
    
    // 验证内置类型是否正确注册
    if (typeRegistry) {
        LOG_DEBUG("TypeRegistry initialized with " + to_string(typeRegistry->getTypeCount()) + " types");
        
        // 检查关键类型是否已注册
        ObjectType* stringType = typeRegistry->getType("string");
        ObjectType* intType = typeRegistry->getType("int");
        ObjectType* boolType = typeRegistry->getType("bool");
        
        if (stringType) {
            LOG_DEBUG("String type found: " + stringType->getTypeName());
        } else {
            LOG_DEBUG("WARNING: String type not found in registry!");
        }
        
        if (intType) {
            LOG_DEBUG("Int type found: " + intType->getTypeName());
        } else {
            LOG_DEBUG("WARNING: Int type not found in registry!");
        }
        
        if (boolType) {
            LOG_DEBUG("Bool type found: " + boolType->getTypeName());
        } else {
            LOG_DEBUG("WARNING: Bool type not found in registry!");
        }
    } else {
        LOG_DEBUG("ERROR: TypeRegistry is null!");
    }
    
    // 使用简化构造函数 - 只需要TypeRegistry
    objectFactory = new ObjectFactory(typeRegistry);
    
    // 初始化计算器实例
    calculator = new Calculator(this);
    
    // 创建初始作用域
    scopeManager.enterScope();
    
    if (loadPlugins) {
        // 自动加载插件目录中的插件
        loadDefaultPlugins();
        
        // 注册所有插件函数
        pluginManager.registerAllPlugins(scopeManager);
    }
}

// 解释器析构函数
Interpreter::~Interpreter() {
    // 清理资源
    if (objectFactory) {
        delete objectFactory;
        objectFactory = nullptr;
    }
    
    if (calculator) {
        delete calculator;
        calculator = nullptr;
    }
    
    // pluginManager是对象，会自动析构，不需要delete
    
    LOG_DEBUG("Interpreter destroyed");
}

// 程序执行
void Interpreter::visit(Program* program) {
    if (!program) return;
    
    for (Statement* stmt : program->statements) {
        visit(stmt);  // 直接调用visit方法，消除execute函数依赖
    }
}

// 静态错误处理函数
void Interpreter::reportError(const std::string& message) {
    std::cerr << "Error: " << message << std::endl;
}

void Interpreter::reportTypeError(const std::string& expected, const std::string& actual) {
    std::cerr << "Type Error: expected " << expected << ", got " << actual << std::endl;
}

// ==================== 类型系统相关方法 ====================

// 获取值的类型名称 - 直接使用运行时类型系统
std::string Interpreter::getValueTypeName(Value* value) {
    if (!value) return "unknown";
    
    // 直接使用Value的valueType指针获取类型名称
    ObjectType* valueType = value->getValueType();
    if (valueType) {
        return valueType->getTypeName();
    }
    
    // 如果valueType为空，返回unknown（这种情况不应该发生）
    return "unknown";
}
