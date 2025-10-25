#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "parser/inter.h"
#include "parser/definition.h"
#include "parser/expression.h"
#include "parser/statement.h"
#include "parser/ast_visitor.h"
#include "parser/advanced_expressions.h"
#include "interpreter/core/control_flow.h"
#include "interpreter/core/function_call.h"
#include "interpreter/core/error_handler.h"
#include "interpreter/core/memory_manager.h"
#include "interpreter/core/async_runtime.h"
#include "interpreter/core/pattern_matching.h"
#include "interpreter/core/concurrency.h"
#include "interpreter/types/type_inference.h"
#include "interpreter/scope/scope.h"
#include "interpreter/plugins/builtin_plugin.h"
#include "interpreter/types/types.h"
#include "interpreter/values/calculate.h"
#include "interpreter/values/value.h"
#include "interpreter/modules/module_system.h"

#include <string>
#include <list>
#include <map>
#include <vector>
#include <functional>
#include <iostream>
#include <sstream>
#include <typeinfo>

using namespace std;

// 前向声明
class Parser;

// 解释器类 - 负责AST的求值和作用域管理
class Interpreter : public ASTVisitor<Value*> {
public:
    // 作用域管理器
    ScopeManager scopeManager;
    
    // 插件管理器
    PluginManager pluginManager;
    
    // 模块系统
    ModuleSystem::ModuleSystem* moduleSystem;
    
    // 类型注册表
    TypeRegistry* typeRegistry;

    // 对象工厂
    ObjectFactory* objectFactory;
    
    // 计算器实例
    Calculator* calculator;
    
    // 解析器实例
    Parser* parser;

    // 调用栈（用于调试和错误报告）
    std::vector<std::string> callStack;

    // 错误处理器
    InterpreterCore::ErrorHandler* errorHandler;
    
    // 异步运行时
    InterpreterCore::AsyncRuntime* asyncRuntime;
    
    // 模式匹配器
    InterpreterCore::PatternMatcher* patternMatcher;
    
    // 类型推断管理器
    InterpreterTypes::TypeInferenceManager* typeInferenceManager;
    
    // 并发管理器
    InterpreterCore::ConcurrencyManager* concurrencyManager;

public:
    // 通用的实例方法调用辅助函数
    Value* callMethodOnInstance(InstanceMethodReference* methodRef, const std::vector<Value*>& args);
    
    // 通用的静态类方法调用辅助函数
    Value* callMethodOnClass(StaticMethodReference* methodRef, const std::vector<Value*>& args);
    
    // 通用的普通函数调用辅助函数
    Value* callFunction(Function* func, const std::vector<Value*>& args);
    
    // 解析和执行代码文件
    Value* parseAndExecute(const std::string& filename);
    
    // 模块系统相关方法
    void initializeModuleSystem();
    
    // 自动导入标准库
    void autoImportStandardLibrary();
    ModuleSystem::ModuleSystem* getModuleSystem() { return moduleSystem; }
    
    // 异步编程相关方法
    void initializeAsyncRuntime();
    InterpreterCore::AsyncRuntime* getAsyncRuntime() { return asyncRuntime; }
    std::shared_ptr<InterpreterCore::Promise> createPromise();
    std::shared_ptr<InterpreterCore::AsyncFunction> createAsyncFunction(std::function<Value*()> func);
    
    // 模式匹配相关方法
    void initializePatternMatcher();
    InterpreterCore::PatternMatcher* getPatternMatcher() { return patternMatcher; }
    InterpreterCore::MatchResult matchPattern(InterpreterCore::Pattern* pattern, Value* value);
    Value* evaluateMatchExpression(MatchExpression* expr, Value* input);
    
    // 类型推断相关方法
    void initializeTypeInference();
    InterpreterTypes::TypeInferenceManager* getTypeInferenceManager() { return typeInferenceManager; }
    InterpreterTypes::TypeInferenceResult inferExpressionType(Expression* expr);
    InterpreterTypes::TypeInferenceResult inferStatementType(Statement* stmt);
    InterpreterTypes::TypeInferenceResult inferFunctionType(FunctionDefinition* func);
    bool checkExpressionType(Expression* expr, Type* expectedType);
    bool checkStatementType(Statement* stmt);
    bool checkFunctionType(FunctionDefinition* func);
    Value* convertValueType(Value* value, Type* targetType);
    
    // 并发支持相关方法
    void initializeConcurrency();
    InterpreterCore::ConcurrencyManager* getConcurrencyManager() { return concurrencyManager; }
    std::shared_ptr<InterpreterCore::Coroutine> createCoroutine(std::function<Value*()> func, const std::string& name = "");
    std::shared_ptr<InterpreterCore::ThreadPoolTask> submitTask(std::function<Value*()> func, const std::string& name = "");
    std::vector<Value*> parallelExecute(const std::vector<std::function<Value*()>>& functions);

public:
    // 构造函数
    Interpreter();
    Interpreter(bool loadPlugins); // 新增构造函数，可以选择是否加载插件
    // 析构函数
    ~Interpreter();
    
    // ASTVisitor接口实现 - 表达式访问方法
    Value* visit(Expression* expr) override;
    Value* visit(ConstantExpression<int>* expr) override;
    Value* visit(ConstantExpression<double>* expr) override;
    Value* visit(ConstantExpression<bool>* expr) override;
    Value* visit(ConstantExpression<char>* expr) override;
    Value* visit(ConstantExpression<std::string>* expr) override;
    Value* visit(VariableExpression* expr) override;
    Value* visit(UnaryExpression* expr) override;
    Value* visit(BinaryExpression* expr) override;
    // IncDecExpression已合并到UnaryExpression中
    // CastExpression的访问方法
    Value* visit(CastExpression* expr) override;
    Value* visit(AccessExpression* expr) override;
    
    // AccessExpression 子类的具体访问方法
    Value* visit(MemberAccessExpression* expr);
    Value* visit(MethodReferenceExpression* expr);
    Value* visit(IndexAccessExpression* expr);
    
    Value* visit(CallExpression* expr) override;
    Value* visit(TernaryExpression* expr) override;
    
    // 高级表达式类型访问方法
    Value* visit(ArrayLiteralExpression* expr);
    Value* visit(DictLiteralExpression* expr);
    Value* visit(SetLiteralExpression* expr);
    Value* visit(RangeExpression* expr);
    Value* visit(SliceExpression* expr);
    Value* visit(NullCoalescingExpression* expr);
    Value* visit(MatchExpression* expr);
    Value* visit(AsyncExpression* expr);
    Value* visit(AwaitExpression* expr);
    Value* visit(TypeCheckExpression* expr);
    Value* visit(TypeCastExpression* expr);
    Value* visit(CompoundAssignExpression* expr);
    
    // ASTVisitor接口实现 - 语句访问方法
    void visit(Statement* stmt) override;
    void visit(ImportStatement* stmt) override;
    void visit(ExpressionStatement* stmt) override;
    void visit(IfStatement* stmt) override;
    void visit(WhileStatement* stmt) override;
    void visit(ForStatement* stmt) override;
    void visit(DoWhileStatement* stmt) override;
    void visit(BlockStatement* stmt) override;
    void visit(BreakStatement* stmt) override;
    void visit(ContinueStatement* stmt) override;
    void visit(ReturnStatement* stmt) override;
    void visit(TryStatement* stmt) override;
    void visit(SwitchStatement* stmt) override;

    void visit(VariableDefinition* stmt) override;
    void visit(FunctionPrototype* stmt) override;
    void visit(FunctionDefinition* stmt) override;
    void visit(StructDefinition* stmt) override;
    void visit(ClassDefinition* stmt) override;
    void visit(InterfaceDefinition* stmt) override;
    void visit(ModuleDefinition* stmt) override;
    void visit(VisibilityStatement* stmt) override;
    

    // ASTVisitor接口实现 - 程序访问方法
    void visit(Program* program) override;
    
    // ==================== AccessExpression辅助方法 ====================
    
    // 提取成员名称
    string extractMemberName(Value* key);
    
    // 插件管理
    void loadPlugin(const std::string& pluginPath);
    void unloadPlugin(const std::string& pluginName);
    std::vector<std::string> getLoadedPlugins() const;
    void loadDefaultPlugins();
    void loadDefaultPlugins(const std::string& pluginDir);
    
    // 类型系统相关方法
    std::string getValueTypeName(Value* value);
    
    // 对象工厂访问方法
    ObjectFactory* getObjectFactory() const { return objectFactory; }
    
    // 类型工厂访问方法
    
    // 辅助方法声明
    void processMemberPrototypes(ObjectType* type, const std::vector<ClassMember*>& members);
    
    // 自增自减操作处理
    Value* handleIncrementDecrement(UnaryExpression* unary);
    
    // 作用域管理辅助函数 - 处理有返回值的函数
    template<typename Func>
    auto withScope(Func func) -> decltype(func()) {
        scopeManager.enterScope();
        try {
            auto result = func();
            scopeManager.exitScope();
            return result;
        } catch (...) {
            scopeManager.exitScope();
            throw;
        }
    }
    
    // 作用域管理辅助函数 - 处理无返回值的函数
    template<typename Func>
    void withScopeVoid(Func func) {
        scopeManager.enterScope();
        try {
            func();
            scopeManager.exitScope();
        } catch (...) {
            scopeManager.exitScope();
            throw;
        }
    }
    
private:
    
    // 辅助函数
    std::string extractPluginName(const std::string& filePath);

    // 通用计算方法 - 通过类型系统调用运算符方法
    Value* calculate_binary(Value* left, Value* right, int op);
    Value* calculate_unary(Value* operand, int op);
    
    // 错误处理相关方法
    InterpreterCore::ErrorHandler* getErrorHandler() const { return errorHandler; }
    void setErrorHandler(InterpreterCore::ErrorHandler* handler) { errorHandler = handler; }
    
    // 便捷错误报告方法
    void reportError(const std::string& message, 
                    const std::string& errorCode = "",
                    const InterpreterCore::ErrorLocation& location = InterpreterCore::ErrorLocation());
    void reportWarning(const std::string& message,
                      const std::string& errorCode = "",
                      const InterpreterCore::ErrorLocation& location = InterpreterCore::ErrorLocation());
    void reportTypeError(const std::string& expected, 
                        const std::string& actual,
                        const InterpreterCore::ErrorLocation& location = InterpreterCore::ErrorLocation());
    void reportUndefinedIdentifier(const std::string& identifier,
                                  const InterpreterCore::ErrorLocation& location = InterpreterCore::ErrorLocation());
    void reportFunctionCallError(const std::string& functionName,
                                const std::string& reason,
                                const InterpreterCore::ErrorLocation& location = InterpreterCore::ErrorLocation());
    
    // 获取错误统计
    int getErrorCount() const;
    int getWarningCount() const;
    bool hasErrors() const;
    std::string generateErrorReport() const;
    
    // 内存管理相关方法
    InterpreterCore::MemoryManager& getMemoryManager() const;
    std::string generateMemoryReport() const;
    bool checkForMemoryLeaks() const;
    void enableMemoryLeakDetection(bool enable);
    void cleanupMemory();
};

#endif // INTERPRETER_H
