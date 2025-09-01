#include "parser/expression.h"
#include "parser/definition.h"
#include "parser/inter.h"
#include "interpreter/utils/logger.h"
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

    // 提取参数名称列表
    vector<string> paramNames;
    if (!funcProto->parameters.empty()) {
        for (const auto& param : funcProto->parameters) {
            paramNames.push_back(param.first);
        }
    }

    // 创建用户函数（暂时没有函数体，在 FunctionDefinition 中设置）
    UserFunction* userFunc = new UserFunction(functionName, paramNames, nullptr);
    if (userFunc) {
        // 解释器自己设置函数执行器（减少依赖注入）
        FunctionExecutor* executor = new InterpreterFunctionExecutor(this);
        userFunc->setExecutor(executor);
        
            // 根据上下文决定函数的归属
    if (scopeManager.getCurrentClassContext()) {
        // 在类定义中，注册为类方法（使用函数签名）
        scopeManager.getCurrentClassContext()->addUserMethod(funcProto, userFunc);
        LOG_DEBUG("FunctionPrototype: function '" + functionName + "' registered as class method with signature");
    } else {
        // 不在类定义中，注册到作用域（使用函数名）
        scopeManager.defineUserFunction(functionName, userFunc);
        LOG_DEBUG("FunctionPrototype: function '" + functionName + "' registered to scope with signature");
    }
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
    }
    
    // 第二步：设置函数体
    if (userFunc) {
        userFunc->setFunctionBody(funcDef->body);
        LOG_DEBUG("FunctionDefinition: function body set for '" + functionName + "'");
    } else {
        LOG_ERROR("FunctionDefinition: failed to find or create function '" + functionName + "'");
    }

    LOG_DEBUG("FunctionDefinition: function definition processing completed");
}

// 统一的成员处理函数
void Interpreter::processMemberPrototypes(ObjectType* type, const vector<ClassMember*>& members) {
    for (const auto& member : members) {
        // 添加成员到类型
        ObjectType* memberType = scopeManager.lookupType(member->type);
        if (memberType) {
            // 转换visibility字符串为VisibilityType
            VisibilityType visibility = VIS_PUBLIC;
            if (member->visibility == "private") {
                visibility = VIS_PRIVATE;
            } else if (member->visibility == "protected") {
                visibility = VIS_PROTECTED;
            }
            
            // 只有ClassType才有addMember方法
            if (ClassType* classType = dynamic_cast<ClassType*>(type)) {
                classType->addMember(member->name, memberType, visibility);
                
                // 如果有默认值，设置初始值
                if (member->defaultValue) {
                    Value* initialValue = visit(member->defaultValue);
                    if (initialValue) {
                        classType->setMemberInitialValue(member->name, initialValue);
                        LOG_DEBUG("Set initial value for member '" + member->name + "': " + initialValue->toString());
                    }
                }
            }
        }
    }
}

// 统一的类型定义处理（结构体和类共用）
void Interpreter::processStructDefinition(StructDefinition* structDef) {
    if (!structDef) return;
    LOG_DEBUG("Processing struct definition: " + structDef->name);

    // 第一步：创建类型
    ObjectType* type = nullptr;
    
    // 现在StructDefinition继承ClassDefinition，所以可以直接访问members
    if (ClassDefinition* classDef = dynamic_cast<ClassDefinition*>(structDef)) {
        // 检查是否是真正的类定义（有方法或基类）
        if (!classDef->methods.empty() || !classDef->baseClass.empty()) {
            // 创建类类型
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
            scopeManager.defineType(classDef->name, type);
            
            if (type) {
                // 设置当前类上下文
                scopeManager.setCurrentClassContext(dynamic_cast<ClassType*>(type));
                
                // 处理类方法
                for (auto* method : classDef->methods) {
                    if (!method) continue;
                    visit(method);
                }
                
                // 清除类上下文
                scopeManager.clearCurrentClassContext();
            }
        } else {
            // 没有方法和基类，创建结构体类型
            type = new StructType(structDef->name);
            scopeManager.defineType(structDef->name, type);
        }
    } else {
        // 创建结构体类型
        type = new StructType(structDef->name);
        scopeManager.defineType(structDef->name, type);
    }
    
    if (type) {
        // 第二步：处理成员原型（包括初始值）
        processMemberPrototypes(type, structDef->members);

                string typeType = (dynamic_cast<ClassDefinition*>(structDef) &&
                          (!structDef->methods.empty() || !structDef->baseClass.empty()) ? "class" : "struct");
        LOG_DEBUG("Successfully created " + typeType + " '" + structDef->name + "' with " +
                 to_string(structDef->members.size()) + " members");
    } else {
                string typeType = (dynamic_cast<ClassDefinition*>(structDef) &&
                          (!structDef->methods.empty() || !structDef->baseClass.empty()) ? "class" : "struct");
        LOG_ERROR("Failed to create " + typeType + " '" + structDef->name + "'");
    }
}

// StructDefinition 访问：创建类型并由解释器设置成员初始值
void Interpreter::visit(StructDefinition* structDef) {
    processStructDefinition(structDef);
}

// ClassMethod 访问：创建方法函数对象并设置函数体
void Interpreter::visit(ClassMethod* method) {
    if (!method || !method->prototype) {
        LOG_ERROR("ClassMethod: Invalid method definition");
        return;
    }
    
    string methodName = method->name;
    string visibility = method->visibility;
    bool isStatic = method->isStatic;
    
    LOG_DEBUG("ClassMethod: processing method '" + methodName + "' (visibility: " + visibility + 
             ", static: " + (isStatic ? "true" : "false") + ")");
    
    // 第一步：检查方法是否已经定义（支持前向声明）
    UserFunction* methodFunc = nullptr;
    if (scopeManager.getCurrentClassContext()) {
        ClassType* currentClass = scopeManager.getCurrentClassContext();
        
        if (isStatic) {
            // 检查静态方法是否已存在
            if (currentClass->getStaticMethods().find(methodName) != currentClass->getStaticMethods().end()) {
                methodFunc = currentClass->getStaticMethods().at(methodName);
                LOG_DEBUG("ClassMethod: static method '" + methodName + "' already exists, using existing definition");
            }
        } else {
            // 检查实例方法是否已存在
            if (currentClass->getUserFunctionMethods().find(methodName) != currentClass->getUserFunctionMethods().end()) {
                methodFunc = currentClass->getUserFunctionMethods().at(methodName);
                LOG_DEBUG("ClassMethod: instance method '" + methodName + "' already exists, using existing definition");
            }
        }
    }
    
    if (!methodFunc) {
        // 如果方法未定义，先访问原型创建方法
        LOG_DEBUG("ClassMethod: method '" + methodName + "' not found, creating from prototype");
        visit(method->prototype);
        
        // 重新查找刚创建的方法
        if (scopeManager.getCurrentClassContext()) {
            ClassType* currentClass = scopeManager.getCurrentClassContext();
            if (isStatic) {
                if (currentClass->getStaticMethods().find(methodName) != currentClass->getStaticMethods().end()) {
                    methodFunc = currentClass->getStaticMethods().at(methodName);
                }
            } else {
                if (currentClass->getUserFunctionMethods().find(methodName) != currentClass->getUserFunctionMethods().end()) {
                    methodFunc = currentClass->getUserFunctionMethods().at(methodName);
                }
            }
        }
    }
    
    // 第二步：设置函数体
    if (methodFunc) {
        methodFunc->setFunctionBody(method->body);
        LOG_DEBUG("ClassMethod: function body set for " + string(isStatic ? "static " : "instance ") + 
                 "method '" + methodName + "'");
    } else {
        LOG_ERROR("ClassMethod: failed to find or create method '" + methodName + "'");
    }
    
    LOG_DEBUG("ClassMethod: method '" + methodName + "' processing completed");
}

// ClassDefinition 访问：创建类型并由解释器设置成员初始值
void Interpreter::visit(ClassDefinition* classDef) {
    processStructDefinition(classDef);
}

// 重载的processStructDefinition方法，接受ClassDefinition*
void Interpreter::processStructDefinition(ClassDefinition* classDef) {
    if (!classDef) return;
    LOG_DEBUG("Processing class definition: " + classDef->name);

    // 第一步：创建类型
    ObjectType* type = nullptr;
    
    // 检查是否是真正的类定义（有方法或基类）
    if (!classDef->methods.empty() || !classDef->baseClass.empty()) {
        // 创建类类型
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
        
        if (type) {
            // 注册类型到作用域
            scopeManager.defineType(classDef->name, type);
            
            // 设置当前类上下文
            scopeManager.setCurrentClassContext(dynamic_cast<ClassType*>(type));
            
            // 处理类方法
            for (auto* method : classDef->methods) {
                if (!method) continue;
                visit(method);
            }
            
            // 清除类上下文
            scopeManager.clearCurrentClassContext();
        }
    } else {
        // 没有方法和基类，创建结构体类型
        type = new StructType(classDef->name);
        scopeManager.defineType(classDef->name, type);
    }
    
    if (type) {
        // 第二步：处理成员原型（包括初始值）
        processMemberPrototypes(type, classDef->members);

        string typeType = (!classDef->methods.empty() || !classDef->baseClass.empty()) ? "class" : "struct";
        LOG_DEBUG("Successfully created " + typeType + " '" + classDef->name + "' with " +
                 to_string(classDef->members.size()) + " members");
    } else {
        string typeType = (!classDef->methods.empty() || !classDef->baseClass.empty()) ? "class" : "struct";
        LOG_ERROR("Failed to create " + typeType + " '" + classDef->name + "'");
    }
}

// ClassMember 访问：处理类成员
void Interpreter::visit(ClassMember* member) {
    if (!member) return;
    
    LOG_DEBUG("ClassMember: processing member '" + member->name + "'");
    
    // 获取成员类型
    ObjectType* memberType = scopeManager.lookupType(member->type);
    if (memberType) {
        // 转换visibility字符串为VisibilityType
        VisibilityType visibility = VIS_PUBLIC;
        if (member->visibility == "private") {
            visibility = VIS_PRIVATE;
        } else if (member->visibility == "protected") {
            visibility = VIS_PROTECTED;
        }
        
        // 添加到当前类上下文
        if (scopeManager.getCurrentClassContext()) {
            ClassType* currentClass = scopeManager.getCurrentClassContext();
            currentClass->addMember(member->name, memberType, visibility);
            
            // 如果有默认值，设置初始值
            if (member->defaultValue) {
                Value* initialValue = visit(member->defaultValue);
                if (initialValue) {
                    currentClass->setMemberInitialValue(member->name, initialValue);
                    LOG_DEBUG("Set initial value for member '" + member->name + "': " + initialValue->toString());
                }
            }
            
            LOG_DEBUG("ClassMember: member '" + member->name + "' added to class successfully");
        } else {
            LOG_ERROR("ClassMember: no current class context for member '" + member->name + "'");
        }
    } else {
        LOG_ERROR("ClassMember: failed to find type '" + member->type + "' for member '" + member->name + "'");
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
        
        // 在作用域中定义变量
        LOG_DEBUG("VariableDefinition: defining variable '" + varName + "' in scope");
        scopeManager.defineVariable(varName, initValue);
        // 注意：defineVariable返回void，所以不能用于if条件
        
        LOG_DEBUG("Successfully defined variable: " + varName + " = " + initValue->toString());
    }
}