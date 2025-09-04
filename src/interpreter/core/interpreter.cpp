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
#include "common/logger.h"
#include "common/string_utils.h"

// 4. 解析器头文件（放在最后，避免命名空间冲突）
#include "parser/expression.h"
#include "parser/definition.h"
#include "parser/inter.h"
#include "parser/parser.h"

using namespace std;

// 解释器构造函数
Interpreter::Interpreter() {
    // 初始化组件
    typeRegistry = TypeRegistry::getGlobalInstance();
    
    // 使用简化构造函数 - 只需要TypeRegistry
    objectFactory = new ObjectFactory(typeRegistry);
    
    // 初始化计算器实例
    calculator = new Calculator(this);
    
    // 初始化解析器实例
    parser = new Parser();
    
    // 创建初始作用域
    scopeManager.enterScope();
    
    // 初始化模块系统
    initializeModuleSystem();
    
    // 注册所有内置插件
    pluginManager.registerAllPlugins(scopeManager);
    
    LOG_DEBUG("Interpreter initialized successfully");
}

// 通用的实例方法调用辅助函数
Value* Interpreter::callMethodOnInstance(InstanceMethodReference* methodRef, const std::vector<Value*>& args) {
    if (!methodRef) return nullptr;
    
    Value* instance = methodRef->getTargetInstance();
    if (!instance) {
        LOG_DEBUG("Instance is null");
        return nullptr;
    }
    LOG_DEBUG("Got instance: " + instance->toString());
    
    // 1. 进入新作用域
    scopeManager.enterScope();
    
    try {
        // 2. 通过实例方法调用执行方法
        InstanceMethodCall methodCall(scopeManager.getCurrentScope(), instance, methodRef, args);

        // 3. 执行方法
        Value* result = methodCall.execute();
        
        // 4. 退出作用域
        scopeManager.exitScope();
        
        return result;
    } catch (...) {
        // 确保在异常情况下也退出作用域
        scopeManager.exitScope();
        throw;
    }
}

// 通用的静态类方法调用辅助函数
Value* Interpreter::callMethodOnClass(StaticMethodReference* methodRef, const std::vector<Value*>& args) {
    if (!methodRef) return nullptr;
    
    ObjectType* classType = methodRef->getTargetType();
    if (!classType || !classType->supportsMethods()) return nullptr;
    
    // 1. 进入新作用域
    scopeManager.enterScope();
    
    try {
        // 2. 通过静态方法调用执行方法
        StaticMethodCall methodCall(scopeManager.getCurrentScope(), classType, methodRef, args);

        // 3. 执行方法
        Value* result = methodCall.execute();
        
        // 4. 退出作用域
        scopeManager.exitScope();
        
        return result;
    } catch (...) {
        // 确保在异常情况下也退出作用域
        scopeManager.exitScope();
        throw;
    }
}

// 通用的普通函数调用辅助函数
Value* Interpreter::callFunction(Function* func, const std::vector<Value*>& args) {
    if (!func) return nullptr;
    
    if (!func->isCallable()) return nullptr;
    
    // 1. 进入新作用域，避免包含脚本执行过程中的变量
    scopeManager.enterScope();
    
    try {
        // 2. 使用 BasicFunctionCall 执行函数
        BasicFunctionCall functionCall(scopeManager.getCurrentScope(), func, args);
        Value* result = functionCall.execute();
        
        // 3. 退出作用域
        scopeManager.exitScope();
        
        return result;
    } catch (...) {
        // 确保在异常情况下也退出作用域
        scopeManager.exitScope();
        throw;
    }
}

// 解释器构造函数（可选择是否加载插件）
Interpreter::Interpreter(bool loadPlugins) : Interpreter() {
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
    
    if (parser) {
        delete parser;
        parser = nullptr;
    }
    
    if (moduleSystem) {
        delete moduleSystem;
        moduleSystem = nullptr;
    }
    
    // pluginManager是对象，会自动析构，不需要delete
    
    LOG_DEBUG("Interpreter destroyed");
}

// 初始化模块系统
void Interpreter::initializeModuleSystem() {
    moduleSystem = new ModuleSystem::ModuleSystem();
    moduleSystem->initialize(&scopeManager);
    moduleSystem->setDefaultSearchPaths();
    
    LOG_DEBUG("Module system initialized");
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

// ==================== 解析和执行方法 ====================

// 解析和执行代码文件
Value* Interpreter::parseAndExecute(const std::string& filename) {
    if (!parser) {
        reportError("Parser not initialized");
        return nullptr;
    }
    
    try {
        // 使用解析器解析文件
        Program* program = parser->parse(filename);
        if (!program) {
            reportError("Failed to parse file: " + filename);
            return nullptr;
        }
        
        // 执行解析后的程序
        visit(program);
        
        // 清理解析后的程序
        delete program;
        
    } catch (const ReturnException& e) {
        return e.getValue<Integer*>();
    } catch (const std::exception& e) {
        reportError("Exception during parsing/execution: " + std::string(e.what()));
        return new Integer(-1);
    }

    return new Integer(0);
}
