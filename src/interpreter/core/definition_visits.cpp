#include "parser/expression.h"
#include "parser/definition.h"
#include "parser/inter.h"
#include "common/logger.h"
#include "interpreter/values/value.h"
#include "interpreter/types/types.h"
#include "interpreter/core/function_call.h"
#include "interpreter/core/interpreter.h"

using namespace std;

// FunctionPrototype 访问：创建用户函数并决定归属
void Interpreter::visit(FunctionPrototype* funcProto) {
    if (!funcProto) return;
    
    string functionName = funcProto->name;
    LOG_DEBUG("FunctionPrototype: processing prototype '" + functionName + "'");

    // 直接创建Parameter数组，而不是提取参数名称列表
    vector<Parameter> paramArray;
    if (!funcProto->parameters.empty()) {
        for (const auto& param : funcProto->parameters) {
            string paramName = param.first;
            string paramType = param.second ? param.second->str() : "any";
            paramArray.push_back(Parameter(paramName, paramType, "", false));
        }
    }

    // 创建用户函数（使用Parameter数组，暂时没有函数体，在 FunctionDefinition 中设置）
    UserFunction* userFunc = new UserFunction(functionName, paramArray, nullptr);
    if (userFunc) {
        // 解释器自己设置函数执行器（减少依赖注入）
        FunctionExecutor* executor = new InterpreterFunctionExecutor(this);
        userFunc->setExecutor(executor);
        
        // 创建函数签名，用于精确的函数查找和注册
        FunctionSignature funcSignature(functionName, paramArray);
        LOG_DEBUG("FunctionPrototype: created function signature: " + funcSignature.toString());
        
        // 根据上下文决定函数的归属
        // scope-manager会自动处理类方法和普通函数的注册
        scopeManager.defineFunction(functionName, userFunc);
        LOG_DEBUG("FunctionPrototype: function '" + functionName + "' registered with signature: " + funcSignature.toString());
    } else {
        LOG_ERROR("FunctionPrototype: failed to create function '" + functionName + "'");
    }

    LOG_DEBUG("FunctionPrototype: function '" + functionName + "' processing completed");
}

// FunctionDefinition 访问：设置函数体
void Interpreter::visit(FunctionDefinition* funcDef) {
    if (!funcDef || !funcDef->prototype) {
        LOG_ERROR("FunctionDefinition: Invalid function definition");
        return;
    }

    string functionName = funcDef->prototype->name;
    LOG_DEBUG("FunctionDefinition: processing function definition '" + functionName + "'");

    // 第一步：检查函数是否已经定义（支持前向声明）
    // 创建函数签名用于精确查找
    FunctionSignature funcSignature(funcDef->prototype);
    LOG_DEBUG("FunctionDefinition: looking for function with signature: " + funcSignature.toString());
    
    Value* funcValue = scopeManager.lookupFunction(functionName);
    UserFunction* userFunc = dynamic_cast<UserFunction*>(funcValue);
    
    if (!userFunc) {
        // 如果函数未定义，先访问原型创建函数
        LOG_DEBUG("FunctionDefinition: function '" + functionName + "' not found, creating from prototype");
        visit(funcDef->prototype);
        
        // 重新查找刚创建的函数
        funcValue = scopeManager.lookupFunction(functionName);
        userFunc = dynamic_cast<UserFunction*>(funcValue);
    } else {
        LOG_DEBUG("FunctionDefinition: function '" + functionName + "' already exists, using existing definition");
        // 可以在这里验证函数签名是否匹配
        if (userFunc->getSignature().isCompatibleWith(funcSignature)) {
            LOG_DEBUG("FunctionDefinition: function signature matches existing definition");
        } else {
            LOG_WARN("FunctionDefinition: function signature differs from existing definition");
        }
    }
    
    // 第二步：设置函数体（不执行，只存储）
    if (userFunc) {
        // 重要：只设置函数体，不执行
        // 函数体中的语句只有在函数被调用时才会执行
        userFunc->setFunctionBody(funcDef->body);
        LOG_DEBUG("FunctionDefinition: function body set for '" + functionName + "' (not executed)");
        
        // 可选：验证函数体的语法（但不执行）
        if (funcDef->body) {
            LOG_DEBUG("FunctionDefinition: function body syntax validated for '" + functionName + "'");
        }
    } else {
        LOG_ERROR("FunctionDefinition: failed to find or create function '" + functionName + "'");
    }

    LOG_DEBUG("FunctionDefinition: function definition processing completed");
}

// StructDefinition 访问：处理结构体定义
void Interpreter::visit(StructDefinition* structDef) {
    if (!structDef) return;
    LOG_DEBUG("Processing struct definition: " + structDef->name);

    // 创建结构体类型
    ObjectType* type = new StructType(structDef->name);
    scopeManager.defineType(structDef->name, type);
    
    // 在当前作用域中注册__struct__变量，标识当前是结构体上下文
    if (Scope* currentScope = scopeManager.getCurrentScope()) {
        currentScope->setVariable("__struct__", new String(structDef->name));
        LOG_DEBUG("Registered __struct__ = '" + structDef->name + "' in current scope");
    }
    
    // 处理结构体中的所有语句
    for (auto* stmt : structDef->statements) {
        if (!stmt) continue;
        visit(stmt);  // 使用访问者模式访问每个语句
    }
    
    LOG_DEBUG("Successfully created struct '" + structDef->name + "' with " +
             to_string(structDef->statements.size()) + " statements");
}

// ClassDefinition 访问：处理类定义
void Interpreter::visit(ClassDefinition* classDef) {
    if (!classDef) return;
    LOG_DEBUG("Processing class definition: " + classDef->name);

    // 创建类类型
    ObjectType* type = nullptr;
    if (!classDef->baseClass.empty() && classDef->baseClass != "Object") {
        // 查找基类类型
        ObjectType* baseType = scopeManager.lookupType(classDef->baseClass);
        if (baseType) {
            type = new ClassType(classDef->name, baseType);
            LOG_DEBUG("Class '" + classDef->name + "' inherits from '" + classDef->baseClass + "'");
        } else {
            type = new ClassType(classDef->name);
            LOG_WARN("Base class '" + classDef->baseClass + "' not found for class '" + classDef->name + "'");
        }
    } else {
        type = new ClassType(classDef->name);
    }
    
    // 处理实现的接口
    if (!classDef->implements.empty()) {
        ClassType* classType = dynamic_cast<ClassType*>(type);
        if (classType) {
            for (const string& interfaceName : classDef->implements) {
                // 查找接口类型
                ObjectType* interfaceType = scopeManager.lookupType(interfaceName);
                if (interfaceType) {
                    InterfaceType* interface = dynamic_cast<InterfaceType*>(interfaceType);
                    if (interface) {
                        // 获取接口要求的所有方法签名（Java风格）
                        const map<string, FunctionPrototype*>& interfaceMethods = interface->getMethodSignatures();
                        for (const auto& methodPair : interfaceMethods) {
                            const string& methodName = methodPair.first;
                            FunctionPrototype* methodProto = methodPair.second;
                            
                            // 创建函数签名
                            FunctionSignature signature(methodProto);
                            
                            // 将接口方法原型注册到类中（作为需要实现的方法）
                            // 这里可以创建一个占位符函数，或者只是记录需要实现的方法
                            LOG_DEBUG("Class '" + classDef->name + "' needs to implement method '" + methodName + "' from interface '" + interfaceName + "'");
                        }
                        LOG_DEBUG("Class '" + classDef->name + "' implements interface '" + interfaceName + "'");
                    }
                } else {
                    LOG_WARN("Interface '" + interfaceName + "' not found for class '" + classDef->name + "'");
                }
            }
        }
    }
    
    // 注册类型到作用域
    scopeManager.defineType(classDef->name, type);
    
    // 在当前作用域中注册__class__变量，标识当前是类上下文
    if (Scope* currentScope = scopeManager.getCurrentScope()) {
        currentScope->setVariable("__class__", new String(classDef->name));
        LOG_DEBUG("Registered __class__ = '" + classDef->name + "' in current scope");
    }
    
    // 处理类体中的所有语句（包括方法和成员）
    for (auto* stmt : classDef->statements) {
        if (!stmt) continue;
        visit(stmt);  // 使用访问者模式访问每个语句
    }
    
    // 验证接口实现（Java风格：在类定义完成后检查）
    if (!classDef->implements.empty()) {
        ClassType* classType = dynamic_cast<ClassType*>(type);
        if (classType) {
            for (const string& interfaceName : classDef->implements) {
                ObjectType* interfaceType = scopeManager.lookupType(interfaceName);
                if (interfaceType) {
                    InterfaceType* interface = dynamic_cast<InterfaceType*>(interfaceType);
                    if (interface) {
                        // 验证类是否实现了接口的所有方法
                        if (interface->isImplementedBy(classType)) {
                            LOG_DEBUG("Class '" + classDef->name + "' successfully implements interface '" + interfaceName + "'");
                        } else {
                            LOG_ERROR("Class '" + classDef->name + "' does not implement all methods required by interface '" + interfaceName + "'");
                        }
                    }
                }
            }
        }
    }
    
    LOG_DEBUG("Successfully created class '" + classDef->name + "' with " +
             to_string(classDef->statements.size()) + " statements");
}

// VisibilityStatement 访问：设置当前可见性
void Interpreter::visit(VisibilityStatement* visStmt) {
    if (!visStmt) return;
    
    LOG_DEBUG("VisibilityStatement: setting visibility to '" + visStmt->visibility + "'");
    
    if (Scope* currentScope = scopeManager.getCurrentScope()) {
        // 将当前可见性设置到作用域中
        currentScope->setVariable("__visibility__", new String(visStmt->visibility));
        LOG_DEBUG("VisibilityStatement: visibility '" + visStmt->visibility + "' set in current scope");
    } else {
        LOG_ERROR("VisibilityStatement: no current scope available");
    }
}

// VariableDefinition 访问：处理变量定义
void Interpreter::visit(VariableDefinition* decl) {
    if (!decl) return;
    
    LOG_DEBUG("Interpreter::visit(VariableDefinition): processing variable definition with " + 
              std::to_string(decl->getVariableCount()) + " variables");
    
    // 处理每个变量的定义
    for (size_t i = 0; i < decl->getVariableCount(); ++i) {
        std::string varName = decl->getVariableName(i);
        Expression* initExpr = decl->getVariableExpression(i);
        
        if (varName.empty()) {
            Interpreter::reportError("Variable name cannot be empty");
            continue;
        }
        
        LOG_DEBUG("Processing variable: " + varName);
        
        // 创建变量并初始化
        Value* initValue = nullptr;
        if (initExpr) {
            // 计算初始化表达式的值
            LOG_DEBUG("VariableDefinition: evaluating initialization expression for '" + varName + "'");
            initValue = visit(initExpr);
            if (!initValue) {
                Interpreter::reportError("Failed to evaluate initialization expression for variable: " + varName);
                continue;
            }
            LOG_DEBUG("VariableDefinition: initialization expression result: " + initValue->toString());
        } else {
            // 没有初始化表达式，创建默认值
            // 暂时创建一个null值，因为ObjectFactory::createDefaultValue()需要对象实例
            initValue = new Null();
            LOG_DEBUG("VariableDefinition: created default value for '" + varName + "': " + initValue->toString());
        }
        
        // scope-manager会自动处理类成员和普通变量的注册
        scopeManager.defineVariable(varName, initValue);
        LOG_DEBUG("VariableDefinition: variable '" + varName + "' defined");
        
        LOG_DEBUG("Successfully defined variable: " + varName + " = " + initValue->toString());
    }
}

// InterfaceDefinition 访问：处理接口定义
void Interpreter::visit(InterfaceDefinition* stmt) {
    if (!stmt) return;
    
    LOG_DEBUG("Processing interface definition: " + stmt->interfaceName);
    
    // 创建接口类型（Java风格）
    InterfaceType* interfaceType = new InterfaceType(stmt->interfaceName);
    
    // 处理继承的接口
    for (const string& extendsName : stmt->extends) {
        interfaceType->addExtendedInterface(extendsName);
        LOG_DEBUG("Interface '" + stmt->interfaceName + "' extends '" + extendsName + "'");
    }
    
    // 处理接口方法签名（Java风格：只有方法签名，没有实现）
    for (FunctionPrototype* method : stmt->methods) {
        interfaceType->addMethodSignature(method->name, method);
        LOG_DEBUG("Interface '" + stmt->interfaceName + "' has method signature '" + method->name + "'");
    }
    
    // 注册接口类型到作用域
    scopeManager.defineType(stmt->interfaceName, interfaceType);
    
    LOG_DEBUG("Successfully created interface '" + stmt->interfaceName + "' with " +
             to_string(stmt->methods.size()) + " method signatures");
}

// ModuleDefinition 访问：处理模块定义
void Interpreter::visit(ModuleDefinition* stmt) {
    if (!stmt) return;
    
    LOG_DEBUG("Processing module definition: " + stmt->moduleName);
    
    // 创建新的作用域
    enterScope();
    
    try {
        // 在当前作用域中注册__module__变量，标识当前是模块上下文
        if (Scope* currentScope = scopeManager.getCurrentScope()) {
            currentScope->setVariable("__module__", new String(stmt->moduleName));
            LOG_DEBUG("Registered __module__ = '" + stmt->moduleName + "' in current scope");
        }
        
        // 执行模块内容
        for (Statement* moduleStmt : stmt->statements) {
            visit(moduleStmt);
        }
        
        // 将模块内容注册到模块系统
        if (moduleSystem) {
            moduleSystem->registerModule(stmt->moduleName, currentScope);
            LOG_DEBUG("Module '" + stmt->moduleName + "' registered to module system");
        }
        
    } catch (const exception& e) {
        reportError("Module execution error: " + string(e.what()));
    }
    
    // 退出作用域
    exitScope();
    
    LOG_DEBUG("Successfully processed module '" + stmt->moduleName + "' with " +
             to_string(stmt->statements.size()) + " statements");
}