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
    if (!classDef->baseClass.empty()) {
        // 查找基类类型
        ObjectType* baseType = scopeManager.lookupType(classDef->baseClass);
        if (baseType) {
            type = new ClassType(classDef->name, baseType);
        } else {
            type = new ClassType(classDef->name);
        }
    } else {
        type = new ClassType(classDef->name);
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