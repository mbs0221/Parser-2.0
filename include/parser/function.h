#ifndef FUNCTION_H
#define FUNCTION_H

#include "parser/inter.h"
#include "lexer/value.h"
#include "parser/statement.h"
#include <string>
#include <vector>
#include <functional>

using namespace std;

// 前向声明
class BlockStatement;

// 结构体成员定义
struct StructMember {
    string name;
    string type;
    string visibility;  // "public", "private", "protected"
    Expression* defaultValue;
    
    StructMember(const string& memberName, const string& memberType, 
                const string& memberVisibility = "public", Expression* defaultVal = nullptr)
        : name(memberName), type(memberType), visibility(memberVisibility), defaultValue(defaultVal) {}
};

// 标识符基类 - 用于统一管理变量、函数、结构体、类定义
class Identifier : public Statement {
public:
    string name;
    
    Identifier(const string& identifierName) : name(identifierName) {}
    virtual ~Identifier() = default;
    
    // 获取标识符类型
    virtual string getIdentifierType() const = 0;
    
    // 实现Statement的accept方法
    void accept(ASTVisitor* visitor) override {
        (void)visitor; // 避免未使用参数警告
    }
};

// ==================== 变量定义 ====================
// 变量定义 - 继承自Identifier
struct Variable : public Identifier {
    Value* value;
    string variableType;
    
    Variable(const string& varName, const string& type, Value* val = nullptr)
        : Identifier(varName), value(val), variableType(type) {}
    
    string getIdentifierType() const override {
        return "Variable";
    }
    
    // 设置变量值
    void setValue(Value* newValue) {
        value = newValue;
    }
    
    // 获取变量值
    Value* getValue() const {
        return value;
    }
    
    // 获取变量类型
    string getType() const {
        return variableType;
    }
    
    ~Variable() {
        // 注意：这里不删除value，因为value可能被其他地方使用
    }
};

// ==================== 函数定义 ====================
// 函数原型
struct FunctionPrototype : public Statement {
    string name;
    vector<string> parameterNames;
    vector<string> parameterTypes;
    string returnType;
    
    FunctionPrototype(const string& funcName, const vector<string>& paramNames, 
                     const vector<string>& paramTypes, const string& retType)
        : name(funcName), parameterNames(paramNames), parameterTypes(paramTypes), returnType(retType) {}
    
    void accept(ASTVisitor* visitor) override {
        (void)visitor; // 避免未使用参数警告
    }
};

// 函数定义 - 继承自Identifier
struct FunctionDefinition : public Identifier {
    FunctionPrototype* prototype;
    BlockStatement* body;
    
    FunctionDefinition(FunctionPrototype* proto, BlockStatement* funcBody)
        : Identifier(proto->name), prototype(proto), body(funcBody) {}
    
    // 获取标识符类型
    string getIdentifierType() const override {
        return "FunctionDefinition";
    }
};

// 内置函数类型 - 继承自FunctionDefinition
class BuiltinFunction : public FunctionDefinition {
public:
    function<Value*(vector<Variable*>&)> func;
    
    BuiltinFunction(const string& funcName, function<Value*(vector<Variable*>&)> funcPtr)
        : FunctionDefinition(nullptr, nullptr), func(funcPtr) {
        name = funcName; // 设置名称
    }
    
    bool isBuiltin() const {
        return true;
    }
};

// 用户函数类型 - 继承自Identifier，适合parser返回
class UserFunction : public FunctionDefinition {
public:
    FunctionPrototype* prototype;
    BlockStatement* body;
    
    UserFunction(FunctionPrototype* proto, BlockStatement* funcBody)
        : FunctionDefinition(proto, funcBody) {}
    
    // 获取标识符类型
    string getIdentifierType() const override {
        return "UserFunction";
    }
    
    bool isBuiltin() const {
        return false;
    }
};

// ==================== 结构体和类定义 ====================
// 结构体定义
struct StructDefinition : public Identifier {
    vector<StructMember> members;
    
    StructDefinition(const string& structName, const vector<StructMember>& structMembers)
        : Identifier(structName), members(structMembers) {}
    
    string getIdentifierType() const override {
        return "StructDefinition";
    }
};

// 类定义
struct ClassDefinition : public Identifier {
    string baseClass;
    vector<StructMember> members;
    vector<FunctionDefinition*> methods;
    
    ClassDefinition(const string& className, const string& base, 
                   const vector<StructMember>& classMembers, const vector<FunctionDefinition*>& classMethods)
        : Identifier(className), baseClass(base), members(classMembers), methods(classMethods) {}
    
    string getIdentifierType() const override {
        return "ClassDefinition";
    }
};


#endif // FUNCTION_H
