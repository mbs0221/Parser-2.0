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
    
    // 初始化错误处理器
    errorHandler = new InterpreterCore::ErrorHandler(false); // 不停止在第一个错误
    
    // 初始化异步运行时
    asyncRuntime = &InterpreterCore::AsyncRuntime::getInstance();
    initializeAsyncRuntime();
    
    // 初始化模式匹配器
    patternMatcher = new InterpreterCore::PatternMatcher();
    initializePatternMatcher();
    
    // 初始化类型推断管理器
    typeInferenceManager = &InterpreterTypes::TypeInferenceManager::getInstance();
    initializeTypeInference();
    
    // 初始化并发管理器
    concurrencyManager = &InterpreterCore::ConcurrencyManager::getInstance();
    initializeConcurrency();
    
    // 创建初始作用域
    scopeManager.enterScope();
    
    // 初始化模块系统
    initializeModuleSystem();
    
    // 注册所有内置插件
    pluginManager.registerAllPlugins(scopeManager);
    
    // 自动导入标准库
    autoImportStandardLibrary();
    
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
    
    if (errorHandler) {
        delete errorHandler;
        errorHandler = nullptr;
    }
    
    // 类型推断管理器是单例，不需要删除
    typeInferenceManager = nullptr;
    
    // 并发管理器是单例，不需要删除
    concurrencyManager = nullptr;
    
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

void Interpreter::autoImportStandardLibrary() {
    LOG_DEBUG("Auto-importing standard library...");
    
    try {
        // 创建标准库的导入语句
        ImportStatement* stdlibImport = new ImportStatement("stdlib/core.mbs", ImportType::DEFAULT);
        
        // 执行导入语句
        visit(stdlibImport);
        
        LOG_DEBUG("Standard library imported successfully");
        
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to auto-import standard library: " + std::string(e.what()));
        // 不抛出异常，允许解释器继续运行
    }
}

// 程序执行
void Interpreter::visit(Program* program) {
    if (!program) return;
    
    for (Statement* stmt : program->statements) {
        visit(stmt);  // 直接调用visit方法，消除execute函数依赖
    }
}

// 导入语句访问方法
void Interpreter::visit(ImportStatement* stmt) {
    if (!stmt || !moduleSystem) {
        reportError("Import statement or module system is null");
        return;
    }
    
    try {
        // 创建导入信息
        ModuleSystem::ImportInfo importInfo(stmt->modulePath, stmt->type);
        
        // 设置命名空间别名
        if (!stmt->namespaceAlias.empty()) {
            importInfo.namespaceAlias = stmt->namespaceAlias;
        }
        
        // 设置导入项
        for (const auto& item : stmt->items) {
            importInfo.items.push_back(ModuleSystem::ImportItem(item.name, item.alias));
        }
        
        // 处理导入
        bool success = moduleSystem->processImport(importInfo);
        
        if (!success) {
            reportError("Failed to import module: " + stmt->modulePath);
        } else {
            LOG_DEBUG("Successfully imported module: " + stmt->modulePath);
        }
        
    } catch (const std::exception& e) {
        reportError("Error importing module '" + stmt->modulePath + "': " + e.what());
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

// ==================== 错误处理方法实现 ====================

void Interpreter::reportError(const std::string& message, 
                             const std::string& errorCode,
                             const InterpreterCore::ErrorLocation& location) {
    if (errorHandler) {
        errorHandler->reportError(message, errorCode, location);
    } else {
        std::cerr << "[ERROR] " << message << std::endl;
    }
}

void Interpreter::reportWarning(const std::string& message,
                               const std::string& errorCode,
                               const InterpreterCore::ErrorLocation& location) {
    if (errorHandler) {
        errorHandler->reportWarning(message, errorCode, location);
    } else {
        std::cerr << "[WARNING] " << message << std::endl;
    }
}

void Interpreter::reportTypeError(const std::string& expected, 
                                 const std::string& actual,
                                 const InterpreterCore::ErrorLocation& location) {
    if (errorHandler) {
        errorHandler->reportTypeError(expected, actual, location);
    } else {
        std::cerr << "[TYPE_ERROR] 期望 " << expected << "，但得到 " << actual << std::endl;
    }
}

void Interpreter::reportUndefinedIdentifier(const std::string& identifier,
                                           const InterpreterCore::ErrorLocation& location) {
    if (errorHandler) {
        errorHandler->reportUndefinedIdentifier(identifier, location);
    } else {
        std::cerr << "[UNDEFINED_IDENTIFIER] 未定义的标识符: " << identifier << std::endl;
    }
}

void Interpreter::reportFunctionCallError(const std::string& functionName,
                                         const std::string& reason,
                                         const InterpreterCore::ErrorLocation& location) {
    if (errorHandler) {
        errorHandler->reportFunctionCallError(functionName, reason, location);
    } else {
        std::cerr << "[FUNCTION_CALL_ERROR] 函数调用错误 '" << functionName << "': " << reason << std::endl;
    }
}

int Interpreter::getErrorCount() const {
    return errorHandler ? errorHandler->getErrorCount() : 0;
}

int Interpreter::getWarningCount() const {
    return errorHandler ? errorHandler->getWarningCount() : 0;
}

bool Interpreter::hasErrors() const {
    return errorHandler ? errorHandler->hasErrors() : false;
}

std::string Interpreter::generateErrorReport() const {
    return errorHandler ? errorHandler->generateReport() : "错误处理器未初始化";
}

// ==================== 内存管理方法实现 ====================

InterpreterCore::MemoryManager& Interpreter::getMemoryManager() const {
    return InterpreterCore::MemoryManager::getInstance();
}

std::string Interpreter::generateMemoryReport() const {
    return InterpreterCore::MemoryManager::getInstance().generateMemoryReport();
}

bool Interpreter::checkForMemoryLeaks() const {
    return InterpreterCore::MemoryManager::getInstance().checkForLeaks();
}

void Interpreter::enableMemoryLeakDetection(bool enable) {
    InterpreterCore::MemoryManager::getInstance().setLeakDetection(enable);
}

void Interpreter::cleanupMemory() {
    InterpreterCore::MemoryManager::getInstance().cleanupAll();
}

// ==================== 异步编程相关方法实现 ====================

void Interpreter::initializeAsyncRuntime() {
    if (!asyncRuntime->isRunning()) {
        asyncRuntime->start();
        LOG_INFO("AsyncRuntime initialized and started");
    }
}

std::shared_ptr<InterpreterCore::Promise> Interpreter::createPromise() {
    return asyncRuntime->createPromise();
}

std::shared_ptr<InterpreterCore::AsyncFunction> Interpreter::createAsyncFunction(std::function<Value*()> func) {
    return std::make_shared<InterpreterCore::AsyncFunction>(func);
}

// ==================== 模式匹配相关方法实现 ====================

void Interpreter::initializePatternMatcher() {
    if (patternMatcher) {
        LOG_INFO("PatternMatcher initialized");
    }
}

InterpreterCore::MatchResult Interpreter::matchPattern(InterpreterCore::Pattern* pattern, Value* value) {
    if (!patternMatcher || !pattern) {
        return InterpreterCore::MatchResult(false, "PatternMatcher或Pattern为空");
    }
    
    return patternMatcher->match(pattern, value);
}

Value* Interpreter::evaluateMatchExpression(MatchExpression* expr, Value* input) {
    if (!expr || !input) {
        reportError("MatchExpression或输入值为空");
        return nullptr;
    }
    
    try {
        // 这里需要根据实际的MatchExpression实现来求值
        // 简化实现：返回输入值
        LOG_DEBUG("MatchExpression求值成功");
        return input;
        
    } catch (const exception& e) {
        reportError("MatchExpression求值错误: " + string(e.what()));
        return nullptr;
    }
}

// ==================== 类型推断相关方法实现 ====================

void Interpreter::initializeTypeInference() {
    if (!typeInferenceManager) {
        reportError("类型推断管理器未初始化");
        return;
    }
    
    LOG_DEBUG("类型推断系统初始化完成");
}

InterpreterTypes::TypeInferenceResult Interpreter::inferExpressionType(Expression* expr) {
    if (!expr) {
        return InterpreterTypes::TypeInferenceResult("表达式为空");
    }
    
    if (!typeInferenceManager) {
        return InterpreterTypes::TypeInferenceResult("类型推断管理器未初始化");
    }
    
    try {
        return typeInferenceManager->infer(expr);
    } catch (const exception& e) {
        return InterpreterTypes::TypeInferenceResult("类型推断错误: " + string(e.what()));
    }
}

InterpreterTypes::TypeInferenceResult Interpreter::inferStatementType(Statement* stmt) {
    if (!stmt) {
        return InterpreterTypes::TypeInferenceResult("语句为空");
    }
    
    if (!typeInferenceManager) {
        return InterpreterTypes::TypeInferenceResult("类型推断管理器未初始化");
    }
    
    try {
        return typeInferenceManager->infer(stmt);
    } catch (const exception& e) {
        return InterpreterTypes::TypeInferenceResult("类型推断错误: " + string(e.what()));
    }
}

InterpreterTypes::TypeInferenceResult Interpreter::inferFunctionType(FunctionDefinition* func) {
    if (!func) {
        return InterpreterTypes::TypeInferenceResult("函数定义为空");
    }
    
    if (!typeInferenceManager) {
        return InterpreterTypes::TypeInferenceResult("类型推断管理器未初始化");
    }
    
    try {
        return typeInferenceManager->infer(func);
    } catch (const exception& e) {
        return InterpreterTypes::TypeInferenceResult("类型推断错误: " + string(e.what()));
    }
}

bool Interpreter::checkExpressionType(Expression* expr, Type* expectedType) {
    if (!expr) {
        reportError("表达式为空");
        return false;
    }
    
    if (!typeInferenceManager) {
        reportError("类型推断管理器未初始化");
        return false;
    }
    
    try {
        return typeInferenceManager->check(expr, expectedType);
    } catch (const exception& e) {
        reportError("类型检查错误: " + string(e.what()));
        return false;
    }
}

bool Interpreter::checkStatementType(Statement* stmt) {
    if (!stmt) {
        reportError("语句为空");
        return false;
    }
    
    if (!typeInferenceManager) {
        reportError("类型推断管理器未初始化");
        return false;
    }
    
    try {
        return typeInferenceManager->check(stmt);
    } catch (const exception& e) {
        reportError("类型检查错误: " + string(e.what()));
        return false;
    }
}

bool Interpreter::checkFunctionType(FunctionDefinition* func) {
    if (!func) {
        reportError("函数定义为空");
        return false;
    }
    
    if (!typeInferenceManager) {
        reportError("类型推断管理器未初始化");
        return false;
    }
    
    try {
        return typeInferenceManager->check(func);
    } catch (const exception& e) {
        reportError("类型检查错误: " + string(e.what()));
        return false;
    }
}

Value* Interpreter::convertValueType(Value* value, Type* targetType) {
    if (!value) {
        reportError("值为空");
        return nullptr;
    }
    
    if (!targetType) {
        reportError("目标类型为空");
        return nullptr;
    }
    
    if (!typeInferenceManager) {
        reportError("类型推断管理器未初始化");
        return nullptr;
    }
    
    try {
        return typeInferenceManager->convert(value, targetType);
    } catch (const exception& e) {
        reportError("类型转换错误: " + string(e.what()));
        return nullptr;
    }
}

// ==================== 并发支持相关方法实现 ====================

void Interpreter::initializeConcurrency() {
    if (!concurrencyManager) {
        reportError("并发管理器未初始化");
        return;
    }
    
    concurrencyManager->initialize();
    LOG_DEBUG("并发支持系统初始化完成");
}

std::shared_ptr<InterpreterCore::Coroutine> Interpreter::createCoroutine(std::function<Value*()> func, const std::string& name) {
    if (!concurrencyManager) {
        reportError("并发管理器未初始化");
        return nullptr;
    }
    
    try {
        return concurrencyManager->createCoroutine(func, name);
    } catch (const exception& e) {
        reportError("创建协程错误: " + string(e.what()));
        return nullptr;
    }
}

std::shared_ptr<InterpreterCore::ThreadPoolTask> Interpreter::submitTask(std::function<Value*()> func, const std::string& name) {
    if (!concurrencyManager) {
        reportError("并发管理器未初始化");
        return nullptr;
    }
    
    try {
        return concurrencyManager->submitTask(func, name);
    } catch (const exception& e) {
        reportError("提交任务错误: " + string(e.what()));
        return nullptr;
    }
}

std::vector<Value*> Interpreter::parallelExecute(const std::vector<std::function<Value*()>>& functions) {
    if (!concurrencyManager) {
        reportError("并发管理器未初始化");
        return {};
    }
    
    try {
        return InterpreterCore::ConcurrencyUtils::parallelExecute(functions);
    } catch (const exception& e) {
        reportError("并行执行错误: " + string(e.what()));
        return {};
    }
}

// ==================== 新增的访问方法实现 ====================


// 类型检查表达式访问方法
Value* Interpreter::visit(TypeCheckExpression* expr) {
    if (!expr) return nullptr;
    
    LOG_DEBUG("处理类型检查表达式");
    
    // 计算左操作数（值）
    Value* value = visit(expr->left);
    if (!value) {
        reportError("类型检查表达式左操作数计算失败");
        return nullptr;
    }
    
    // 获取类型名称（右边是类型名称，不需要计算）
    string typeName;
    if (VariableExpression* varType = dynamic_cast<VariableExpression*>(expr->right)) {
        typeName = varType->name;
    } else {
        reportError("类型检查表达式的类型参数必须是类型名称");
        return nullptr;
    }
    
    LOG_DEBUG("检查值类型是否为: " + typeName);
    
    // 通过类型系统获取期望的类型
    ObjectType* expectedType = nullptr;
    if (typeRegistry) {
        expectedType = typeRegistry->getType(typeName);
    }
    
    if (!expectedType) {
        LOG_DEBUG("类型系统中未找到类型: " + typeName);
        return new Bool(false);
    }
    
    // 获取值的实际类型
    ObjectType* actualType = value->getType();
    if (!actualType) {
        LOG_DEBUG("值没有类型信息");
        return new Bool(false);
    }
    
    // 执行类型检查
    bool isType = (actualType == expectedType);
    
    LOG_DEBUG("类型检查结果: " + (isType ? "true" : "false") + 
              " (实际类型: " + actualType->getName() + 
              ", 期望类型: " + expectedType->getName() + ")");
    
    return new Bool(isType);
}


