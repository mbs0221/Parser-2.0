#include "interpreter/interpreter.h"
#include "parser/expression.h"
#include "parser/function.h"
#include "parser/inter.h"
#include "lexer/lexer.h"
#include "interpreter/logger.h"
#include "lexer/value.h"
#include "interpreter/builtin.h"
#include <iostream>
#include <sstream>
#include <typeinfo>
#include <map>
#include <functional>

using namespace std;

// 解释器构造函数
Interpreter::Interpreter() {
    // ScopeManager在构造函数中自动初始化
    
    // 注册内置函数到作用域管理器
    registerBuiltinFunctionsToScope();
}

// 解释器析构函数
Interpreter::~Interpreter() {
    // ScopeManager在析构函数中自动清理
}

// 执行语句
void Interpreter::execute(Statement* stmt) {
    if (!stmt) return;
    stmt->accept(this);
}

// 执行程序
void Interpreter::execute(Program* program) {
    if (!program) return;
    program->accept(this);
}

// 程序执行
void Interpreter::visit(Program* program) {
    if (!program) return;
    
    for (Statement* stmt : program->statements) {
        execute(stmt);
    }
}

void Interpreter::visit(FunctionPrototype* funcProto) {
    if (!funcProto) return;
    // 函数原型不需要执行，只是声明
    LOG_DEBUG("Function prototype: " + funcProto->name);
}

void Interpreter::visit(StructDefinition* structDef) {
    if (!structDef) return;
    
    // 将结构体定义注册到作用域中
    scopeManager.defineStruct(structDef->name, structDef);
    
    LOG_DEBUG("Registered struct '" + structDef->name + "' with " + to_string(structDef->members.size()) + " members");
}

void Interpreter::visit(ClassDefinition* classDef) {
    if (!classDef) return;
    
    // 将类定义注册到作用域中
    scopeManager.defineClass(classDef->name, classDef);
    
    LOG_DEBUG("Registered class '" + classDef->name + "' with " + to_string(classDef->members.size()) + " members and " + to_string(classDef->methods.size()) + " methods");
}

// Identifier访问方法
void Interpreter::visit(Identifier* id) {
    if (!id) return;
    // Identifier主要用于类型检查，不需要执行
    LOG_DEBUG("Visiting identifier: " + id->name);
}

// Variable访问方法
void Interpreter::visit(Variable* var) {
    if (!var) return;
    // Variable主要用于类型检查，不需要执行
    LOG_DEBUG("Visiting variable: " + var->name);
}

// 类实例化
Value* Interpreter::instantiateClass(ClassDefinition* classDef, vector<Value*>& args) {
    if (!classDef) return nullptr;
    
    // 创建字典来存储实例的成员
    Dict* instance = new Dict();
    
    // 根据参数顺序初始化成员
    const vector<StructMember>& members = classDef->members;
    for (size_t i = 0; i < members.size() && i < args.size(); ++i) {
        const StructMember& member = members[i];
        instance->setEntry(member.name, args[i]);
    }
    
    // 为未初始化的成员设置默认值
    for (size_t i = args.size(); i < members.size(); ++i) {
        const StructMember& member = members[i];
        if (member.defaultValue) {
            // 如果有默认值表达式，求值它
            Value* defaultVal = visit(member.defaultValue);
            instance->setEntry(member.name, defaultVal);
        } else {
            // 否则使用类型的默认值
            Value* defaultVal = createDefaultValue(member.type);
            instance->setEntry(member.name, defaultVal);
        }
    }
    
    return instance;
}

// 结构体实例化
Value* Interpreter::instantiateStruct(StructDefinition* structDef, vector<Value*>& args) {
    if (!structDef) return nullptr;
    
    // 创建字典来存储实例的成员
    Dict* instance = new Dict();
    
    // 检查是否是通过结构体实例化语法传递的参数（第一个参数是Dict）
    if (!args.empty() && dynamic_cast<Dict*>(args[0])) {
        // 结构体实例化语法：Person {name: "Alice", age: 25}
        Dict* memberDict = dynamic_cast<Dict*>(args[0]);
        const vector<StructMember>& members = structDef->members;
        
        // 按照结构体定义的成员顺序初始化
        for (const StructMember& member : members) {
            Value* memberValue = memberDict->getEntry(member.name);
            if (memberValue) {
                // 如果提供了该成员的值，使用提供的值
                instance->setEntry(member.name, memberValue);
            } else if (member.defaultValue) {
                // 如果有默认值表达式，求值它
                Value* defaultVal = visit(member.defaultValue);
                instance->setEntry(member.name, defaultVal);
            } else {
                // 否则使用类型的默认值
                Value* defaultVal = createDefaultValue(member.type);
                instance->setEntry(member.name, defaultVal);
            }
        }
    } else {
        // 构造函数语法：Person("Alice", 25) - 按参数顺序初始化
        const vector<StructMember>& members = structDef->members;
        for (size_t i = 0; i < members.size() && i < args.size(); ++i) {
            const StructMember& member = members[i];
            instance->setEntry(member.name, args[i]);
        }
        
        // 为未初始化的成员设置默认值
        for (size_t i = args.size(); i < members.size(); ++i) {
            const StructMember& member = members[i];
            if (member.defaultValue) {
                // 如果有默认值表达式，求值它
                Value* defaultVal = visit(member.defaultValue);
                instance->setEntry(member.name, defaultVal);
            } else {
                // 否则使用类型的默认值
                Value* defaultVal = createDefaultValue(member.type);
                instance->setEntry(member.name, defaultVal);
            }
        }
    }
    
    return instance;
}

// 创建默认值
Value* Interpreter::createDefaultValue(Type* type) {
    if (!type) return new Integer(0);
    
    if (type == Type::Int) {
        return new Integer(0);
    } else if (type == Type::Double) {
        return new Double(0.0);
    } else if (type == Type::Char) {
        return new Char('\0');
    } else if (type == Type::Bool) {
        return new Bool(false);
    } else if (type == Type::String) {
        return new String("");
    } else {
            return new Integer(0); // 默认返回整数0
    }
}

// 注册内置函数到作用域管理器
void Interpreter::registerBuiltinFunctionsToScope() {
    ::registerBuiltinFunctionsToScope(scopeManager);
}

// 错误处理
void Interpreter::reportError(const string& message) {
    cerr << "Error: " << message << endl;
}

void Interpreter::reportTypeError(const string& expected, const string& actual) {
    cerr << "Type Error: expected " << expected << ", got " << actual << endl;
}
