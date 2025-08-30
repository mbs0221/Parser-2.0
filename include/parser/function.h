#ifndef FUNCTION_H
#define FUNCTION_H

#include "parser/inter.h"
#include "parser/statement.h"
#include "parser/ast_visitor.h"
#include <string>
#include <vector>
#include <functional>

using namespace std;
using namespace lexer;

// 前向声明
class BlockStatement;
template<typename ReturnType>
class ASTVisitor;

// 结构体成员定义
struct StructMember {
    string name;
    Type* type;
    string visibility;  // "public", "private", "protected"
    Expression* defaultValue;
    
    StructMember(const string& memberName, Type* memberType, 
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
    void accept(StatementVisitor* visitor) override;
};

// ==================== 变量定义 ====================
// 变量定义 - 继承自Identifier，只存储编译时信息
struct Variable : public Identifier {
    Type* variableType;
    
    Variable(const string& varName, Type* type)
        : Identifier(varName), variableType(type) {}
    
    string getIdentifierType() const override {
        return "Variable";
    }
    
    // 获取变量类型
    Type* getType() const {
        return variableType;
    }
    
    void accept(StatementVisitor* visitor) override;
};

// ==================== 函数定义 ====================
// 函数原型
struct FunctionPrototype : public Statement {
    string name;
    vector<pair<string, Type*>> parameters;  // 使用pair管理参数名称和类型
    Type* returnType;
    
    FunctionPrototype(const string& funcName, const vector<pair<string, Type*>>& params, Type* retType)
        : name(funcName), parameters(params), returnType(retType) {}
    
    // 兼容性构造函数，用于向后兼容
    FunctionPrototype(const string& funcName, const vector<string>& paramNames, 
                     const vector<Type*>& paramTypes, Type* retType)
        : name(funcName), returnType(retType) {
        // 将两个向量合并为pair向量
        size_t size = min(paramNames.size(), paramTypes.size());
        for (size_t i = 0; i < size; ++i) {
            parameters.push_back(make_pair(paramNames[i], paramTypes[i]));
        }
    }
    
    // 获取参数名称向量（用于向后兼容）
    vector<string> getParameterNames() const {
        vector<string> names;
        for (const auto& param : parameters) {
            names.push_back(param.first);
        }
        return names;
    }
    
    // 获取参数类型向量（用于向后兼容）
    vector<Type*> getParameterTypes() const {
        vector<Type*> types;
        for (const auto& param : parameters) {
            types.push_back(param.second);
        }
        return types;
    }
    
    void accept(StatementVisitor* visitor) override;
};

// 函数定义 - 继承自Identifier
struct FunctionDefinition : public Identifier {
    FunctionPrototype* prototype;
    BlockStatement* body;
    
    FunctionDefinition(FunctionPrototype* proto, BlockStatement* funcBody)
        : Identifier(proto ? proto->name : ""), prototype(proto), body(funcBody) {}
    
    virtual ~FunctionDefinition() {
        if (prototype) {
            delete prototype;
            prototype = nullptr;
        }
        // 注意：body 是 AST 的一部分，由 Parser 管理，这里不删除
    }
    
    // 获取标识符类型
    string getIdentifierType() const override {
        return "FunctionDefinition";
    }
    
    void accept(StatementVisitor* visitor) override;
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
    
    void accept(StatementVisitor* visitor) override;
};

// 类方法定义
struct ClassMethod : public FunctionDefinition {
    string visibility;  // "public", "private", "protected"
    bool isStatic;
    
    ClassMethod(FunctionPrototype* proto, BlockStatement* funcBody, 
               const string& methodVisibility = "public", bool staticMethod = false)
        : FunctionDefinition(proto, funcBody), visibility(methodVisibility), isStatic(staticMethod) {}
    
    string getIdentifierType() const override {
        return "ClassMethod";
    }
    
    void accept(StatementVisitor* visitor) override;
};

// 类定义
struct ClassDefinition : public Identifier {
    string baseClass;
    vector<StructMember> members;
    vector<ClassMethod*> methods;
    
    ClassDefinition(const string& className, const string& base, 
                   const vector<StructMember>& classMembers, const vector<ClassMethod*>& classMethods)
        : Identifier(className), baseClass(base), members(classMembers), methods(classMethods) {}
    
    ~ClassDefinition() {
        for (ClassMethod* method : methods) {
            if (method) {
                delete method;
            }
        }
        methods.clear();
    }
    
    string getIdentifierType() const override {
        return "ClassDefinition";
    }

    void accept(StatementVisitor* visitor) override;
};


#endif // FUNCTION_H
