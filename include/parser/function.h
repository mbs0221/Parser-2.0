#ifndef FUNCTION_H
#define FUNCTION_H

#include "lexer/value.h"
#include "parser/inter.h"
#include "parser/statement.h"
#include <string>
#include <vector>
#include <functional>

using namespace std;

// 前向声明
class BlockStatement;
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
    void accept(ASTVisitor* visitor) override;
};

// ==================== 变量定义 ====================
// 变量定义 - 继承自Identifier
struct Variable : public Identifier {
    Value* value;
    Type* variableType;
    
    Variable(const string& varName, Type* type, Value* val = nullptr)
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
    Type* getType() const {
        return variableType;
    }
    
    void accept(ASTVisitor* visitor) override;
    
    ~Variable() {
        // 注意：这里不删除value，因为value可能被其他地方使用
    }
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
    
    void accept(ASTVisitor* visitor) override;
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
    
    // 虚函数：判断是否为内置函数
    virtual bool isBuiltin() const {
        return false;
    }
    
    // 虚函数：执行函数（内置函数使用）
    virtual Value* execute(vector<Variable*>& args) {
        // 默认实现：用户函数需要解释器执行
        return nullptr;
    }
    
    // 虚函数：执行函数（用户函数使用，需要解释器上下文）
    virtual Value* executeWithInterpreter(vector<Value*>& args, class Interpreter* interpreter) {
        // 默认实现：内置函数不需要解释器
        return nullptr;
    }
    
    void accept(ASTVisitor* visitor) override;
};

// 内置函数类型 - 继承自FunctionDefinition
class BuiltinFunction : public FunctionDefinition {
public:
    function<Value*(vector<Variable*>&)> func;
    
    BuiltinFunction(const string& funcName, function<Value*(vector<Variable*>&)> funcPtr)
        : FunctionDefinition(createBuiltinPrototype(funcName), nullptr), func(funcPtr) {
        name = funcName; // 设置名称
    }
    
    bool isBuiltin() const override {
        return true;
    }
    
    Value* execute(vector<Variable*>& args) override {
        return func(args);
    }
    
    void accept(ASTVisitor* visitor) override;

private:
    // 为内置函数创建简单的函数原型
    static FunctionPrototype* createBuiltinPrototype(const string& funcName) {
        return new FunctionPrototype(funcName, {}, nullptr); // 空参数列表，无返回类型
    }
};

// 用户函数类型 - 继承自FunctionDefinition
class UserFunction : public FunctionDefinition {
public:
    UserFunction(FunctionPrototype* proto, BlockStatement* funcBody)
        : FunctionDefinition(proto, funcBody) {}
    
    // 获取标识符类型
    string getIdentifierType() const override {
        return "UserFunction";
    }
    
    bool isBuiltin() const override {
        return false;
    }
    
    // 重写执行方法，需要解释器上下文
    Value* executeWithInterpreter(vector<Value*>& args, class Interpreter* interpreter) override;
    
    void accept(ASTVisitor* visitor) override;
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
    
    void accept(ASTVisitor* visitor) override;
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
    
    void accept(ASTVisitor* visitor) override;
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

    void accept(ASTVisitor* visitor) override;
};


#endif // FUNCTION_H
