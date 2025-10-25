#ifndef DEFINITION_H
#define DEFINITION_H

#include "inter.h"
#include "statement.h"
#include "ast_visitor.h"
#include <string>
#include <vector>
#include <functional>
#include <utility>

// 前向声明
class BlockStatement;
template<typename ReturnType>
class ASTVisitor;

// 类成员 - 声明成员，可以包含默认值
class ClassMember : public Statement {
public:
    std::string name;
    std::string type;
    std::string visibility;  // "public", "private", "protected"
    Expression* defaultValue;  // 可以包含默认值
    
    ClassMember(const std::string& name, const std::string& type, 
               const std::string& visibility = "public", 
               Expression* defaultValue = nullptr)
        : name(name), type(type), visibility(visibility), defaultValue(defaultValue) {}
    
    void accept(StatementVisitor* visitor) override {
        visitor->visit(this);
    }
};

// 结构体成员定义（保持向后兼容）
struct StructMember {
    std::string name;
    Type* type;
    std::string visibility;  // "public", "private", "protected"
    Expression* defaultValue;
    
    StructMember(const std::string& memberName, Type* memberType, 
                const std::string& memberVisibility = "public", Expression* defaultVal = nullptr)
        : name(memberName), type(memberType), visibility(memberVisibility), defaultValue(defaultVal) {}
};

// 定义基类 - 用于统一管理变量、函数、结构体、类定义
class Definition : public Statement {
public:
    std::string name;
    
    Definition(const std::string& definitionName) : name(definitionName) {}
    virtual ~Definition() = default;
    
    // 获取定义类型
    virtual std::string getDefinitionType() const = 0;
    
    // 实现Statement的accept方法
    void accept(StatementVisitor* visitor) override;
};

// ==================== 变量定义 ====================
// 变量定义 - 支持连续定义多个变量
struct VariableDefinition : public Definition {
    std::vector<std::pair<std::string, Expression*>> variables;  // 支持多个变量定义
    
    VariableDefinition() : Definition("") {}
    VariableDefinition(const std::string& name, Expression* value = nullptr) : Definition(name) {
        addVariable(name, value);
    }
    
    ~VariableDefinition() {
        for (auto& pair : variables) {
            if (pair.second) {
                delete pair.second;
                pair.second = nullptr;
            }
        }
        variables.clear();
    }
    
    void addVariable(const std::string& name, Expression* value) {
        variables.push_back(std::make_pair(name, value));
    }
    
    std::string getDefinitionType() const override {
        return "VariableDefinition";
    }
    
    // 获取变量数量
    size_t getVariableCount() const {
        return variables.size();
    }
    
    // 获取指定索引的变量名
    std::string getVariableName(size_t index) const {
        if (index < variables.size()) {
            return variables[index].first;
        }
        return "";
    }
    
    // 获取指定索引的初始化表达式
    Expression* getVariableExpression(size_t index) const {
        if (index < variables.size()) {
            return variables[index].second;
        }
        return nullptr;
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
    
    // 获取参数类型名称向量（用于运行时类型转换）
    vector<string> getParameterTypeNames() const {
        vector<string> typeNames;
        for (const auto& param : parameters) {
            if (param.second) {
                string typeName = param.second->str();
                // 检查是否为null类型
                if (param.second->Tag == NULL_VALUE) {
                    typeNames.push_back("null");
                } else {
                    typeNames.push_back(typeName);
                }
            } else {
                typeNames.push_back("auto");  // 默认类型
            }
        }
        return typeNames;
    }
    
    // 获取参数大小向量（用于内存分配）
    vector<int> getParameterSizes() const {
        vector<int> sizes;
        for (const auto& param : parameters) {
            if (param.second) {
                sizes.push_back(param.second->getSize());
            } else {
                sizes.push_back(4);  // 默认大小（int类型）
            }
        }
        return sizes;
    }
    
    // 获取函数参数总大小（用于栈帧分配）
    int getTotalParameterSize() const {
        int totalSize = 0;
        for (const auto& param : parameters) {
            if (param.second) {
                totalSize += param.second->getSize();
            } else {
                totalSize += 4;  // 默认大小
            }
        }
        return totalSize;
    }
    
    void accept(StatementVisitor* visitor) override;
};

// 函数定义 - 继承自Definition
struct FunctionDefinition : public Definition {
    FunctionPrototype* prototype;
    BlockStatement* body;
    
    FunctionDefinition(FunctionPrototype* proto, BlockStatement* funcBody)
        : Definition(proto ? proto->name : ""), prototype(proto), body(funcBody) {}
    
    virtual ~FunctionDefinition() {
        if (prototype) {
            delete prototype;
            prototype = nullptr;
        }
        // 注意：body 是 AST 的一部分，由 Parser 管理，这里不删除
    }
    
    // 获取定义类型
    string getDefinitionType() const override {
        return "FunctionDefinition";
    }
    
    // 便捷方法：获取参数名称
    vector<string> getParameterNames() const {
        if (prototype) {
            return prototype->getParameterNames();
        }
        return {};
    }
    
    // 便捷方法：获取参数类型名称
    vector<string> getParameterTypeNames() const {
        if (prototype) {
            return prototype->getParameterTypeNames();
        }
        return {};
    }
    
    // 便捷方法：获取参数类型
    vector<Type*> getParameterTypes() const {
        if (prototype) {
            return prototype->getParameterTypes();
        }
        return {};
    }
    
    // 便捷方法：获取参数大小
    vector<int> getParameterSizes() const {
        if (prototype) {
            return prototype->getParameterSizes();
        }
        return {};
    }
    
    // 便捷方法：获取参数总大小
    int getTotalParameterSize() const {
        if (prototype) {
            return prototype->getTotalParameterSize();
        }
        return 0;
    }
    
    void accept(StatementVisitor* visitor) override;
};

// ==================== 可见性语句 ====================
// 可见性声明语句，用于在类定义中设置当前可见性
struct VisibilityStatement : public Statement {
    string visibility;  // "public", "private", "protected"
    
    VisibilityStatement(const string& vis) : visibility(vis) {}
    
    string getStatementType() const {
        return "VisibilityStatement";
    }
    
    void accept(StatementVisitor* visitor) override;
};

// ==================== 结构体和类定义 ====================

// 类定义 - 基类，本质上是一组statement
struct ClassDefinition : public Definition {
    string baseClass;
    vector<string> implements;  // 实现的接口列表
    vector<Statement*> statements;  // 类体中的所有语句（包括成员、方法等）
    
    ClassDefinition(const string& className, const string& base, const vector<Statement*>& stmts)
        : Definition(className), baseClass(base), statements(stmts) {}
    
    ClassDefinition(const string& className, const string& base, const vector<string>& interfaces, const vector<Statement*>& stmts)
        : Definition(className), baseClass(base), implements(interfaces), statements(stmts) {}
    
    virtual ~ClassDefinition() {
        for (auto* stmt : statements) {
            delete stmt;
        }
    }
    
    string getDefinitionType() const override {
        return "ClassDefinition";
    }

    void accept(StatementVisitor* visitor) override;
};

// 结构体定义 - 继承自类定义，所有成员都是public
struct StructDefinition : public ClassDefinition {
    StructDefinition(const string& structName, const vector<Statement*>& stmts)
        : ClassDefinition(structName, "", stmts) {}  // 没有基类，只有语句
    
    string getDefinitionType() const override {
        return "StructDefinition";
    }
    
    void accept(StatementVisitor* visitor) override;
};

// ==================== 接口定义 ====================

// 接口定义
struct InterfaceDefinition : public Definition {
    string interfaceName;
    vector<string> extends;  // 继承的接口列表
    vector<FunctionPrototype*> methods;  // 接口方法原型
    
    InterfaceDefinition(const string& name, const vector<string>& extendsList = {}, 
                       const vector<FunctionPrototype*>& methodList = {})
        : interfaceName(name), extends(extendsList), methods(methodList) {}
    
    string getDefinitionType() const override {
        return "InterfaceDefinition";
    }
    
    void accept(StatementVisitor* visitor) override;
};

// ==================== 模块定义 ====================

// 模块定义
struct ModuleDefinition : public Definition {
    string moduleName;
    vector<Statement*> statements;  // 模块内容
    
    ModuleDefinition(const string& name, const vector<Statement*>& stmts)
        : moduleName(name), statements(stmts) {}
    
    string getDefinitionType() const override {
        return "ModuleDefinition";
    }
    
    void accept(StatementVisitor* visitor) override;
};


#endif // FUNCTION_H
